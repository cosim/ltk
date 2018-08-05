#include "stdafx.h"
#include "DukObject.h"
#include "Common.h"

namespace ltk {

const char * DukThisSymbol = DUK_HIDDEN_SYMBOL("LtkObject");

DukObject * DukCheckType(duk_context *ctx, duk_idx_t idx, size_t type_id)
{
    duk_get_prop_string(ctx, idx, DukThisSymbol);
    RTTI *rtti = (RTTI *)duk_require_pointer(ctx, idx);
    duk_pop(ctx);

    if (!rtti || !rtti->Is(type_id)) {
        return nullptr;
    }
    return (DukObject *)rtti;
}

void DukPrintStack(duk_context *ctx)
{
    auto top = duk_get_top(ctx);
    std::stringstream out;

    for (duk_idx_t i = 0; i < top; i++)
    {
        auto type = duk_get_type(ctx, i);
        switch (type)
        {
        case DUK_TYPE_STRING:
        {
            auto str = duk_get_string(ctx, i);
            out << "[" << i << "] str:\"" << str << "\"\r\n";
        }
        break;
        case DUK_TYPE_NUMBER:
        {
            auto num = duk_get_number(ctx, i);
            out << "[" << i << "] num: " << num << "\r\n";
        }
        break;
        case DUK_TYPE_OBJECT:
        {
            // TODO 
        }
        break;
        default:
            break;
        }
    }
}

bool DukPCall(duk_context *ctx, duk_idx_t nargs)
{
    auto ret = duk_pcall(ctx, nargs);
    if (ret == DUK_EXEC_SUCCESS) {
        return true;
    }
    else {
        if (duk_is_error(ctx, -1)) {
            /* Accessing .stack might cause an error to be thrown, so wrap this
            * access in a duk_safe_call() if it matters.
            */
            duk_get_prop_string(ctx, -1, "stack");
            ::OutputDebugStringA(duk_safe_to_string(ctx, -1));
            duk_pop(ctx);
        }
        else {
            /* Non-Error value, coerce safely to string. */
            ::OutputDebugStringA(duk_safe_to_string(ctx, -1));
        }
        ::PostQuitMessage(0);
    }
    return false;
}

DukObject::DukObject()
{
}

DukObject::~DukObject()
{
}

DWORD DukObject::GetNextId()
{
    return _InterlockedIncrement(&m_sNextId);
}

duk_ret_t DukObject::AddListener(duk_context *ctx)
{
    DukStackChecker chk(ctx);

    DukObject *thiz = DukCheckThis<DukObject>(ctx);
    if (!thiz) return DUK_RET_TYPE_ERROR;

    auto pszEvent = duk_require_string(ctx, 0);
    if (!pszEvent) return DUK_RET_TYPE_ERROR;

    duk_require_function(ctx, 1);

    std::string strEvent(pszEvent);
    DukCallbackInfo cb_info;
    cb_info.stashId = GetNextId();
    duk_push_heap_stash(ctx);
    duk_dup(ctx, 1);
    // stack: event_name func stash func
    duk_put_prop_index(ctx, -1, cb_info.stashId); // stack -1
    duk_pop(ctx); // pop stash

    auto &cb_map = thiz->m_callbackMap;
    auto iter = cb_map.find(strEvent);
    if (iter == cb_map.end())
    {
        auto res = cb_map.emplace(std::make_pair(strEvent, std::vector<DukCallbackInfo>()));
        assert(res.second); // inserted 
        iter = res.first;
    }
    iter->second.push_back(cb_info);

    return 0;
}

void DukObject::DispatchEvent(duk_context *ctx, const char *event_name, duk_idx_t nargs)
{
    DukStackChecker chk(ctx);
    std::string strEvent(event_name);
    auto &cb_map = m_callbackMap;
    auto iter = cb_map.find(strEvent);
    if (iter != cb_map.end())
    {
        auto first_arg = duk_get_top(ctx) - nargs;
        duk_push_heap_stash(ctx);
        auto &cb_vec = iter->second;
        for (UINT i = 0; i < cb_vec.size(); i++)
        {
            // duk_push_number(ctx, (double)cb_vec[i].stashId);
            duk_get_prop_index(ctx, -1, (duk_uarridx_t)cb_vec[i].stashId);
            if (duk_is_function(ctx, -1))
            {
                // stack: nargs stash func
                for (int j = 0; j < nargs; j++)
                {
                    duk_dup(ctx, j + first_arg);
                }
                DukPCall(ctx, nargs);
                duk_pop(ctx); // discard func return
            }
            else
            {
                LOG("type:" << duk_get_type(ctx, -1)); // DUK_TYPE_UNDEFINED
                // TODO: if not exists remove current entry
            }
            duk_pop(ctx); // pop func
        }
        duk_pop(ctx); // pop stash
    }
    duk_pop_n(ctx, nargs); // pop nargs
}

DWORD DukObject::m_sNextId = 0;

} // namespace ltk
