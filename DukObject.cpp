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
    DukStackChecker chk(ctx);
    duk_push_context_dump(ctx);
    CStringA log;
    log.Format("%s\r\n", duk_to_string(ctx, -1));
    DukLog(log);
    duk_pop(ctx);
}

void DukLog(const char *psz)
{
    ::OutputDebugStringA(psz);
    // TODO log to file
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
            DukLog(duk_safe_to_string(ctx, -1));
            duk_pop(ctx);
        }
        else {
            /* Non-Error value, coerce safely to string. */
            DukLog(duk_safe_to_string(ctx, -1));
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

duk_ret_t DukObject::DukFinalizer(duk_context *ctx)
{
    DukStackChecker chk(ctx);
    duk_get_prop_string(ctx, 0, DukThisSymbol);
    if (!duk_is_pointer(ctx, -1)) {
        duk_pop(ctx); // pop undefined
        return 0; // prototype itself
    }
    RTTI *rtti = (RTTI *)duk_get_pointer(ctx, -1);
    duk_pop(ctx); // pop pointer
    if (!rtti || !rtti->Is(DukObject::TypeIdClass())) {
        __debugbreak();
    }
    auto thiz = (DukObject *)rtti;
    thiz->Unref();
    return 0;
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
    duk_get_global_string(ctx, "__ltk_callbacks");
    duk_dup(ctx, 1);
    // stack: event_name func stash func
    DukPrintStack(ctx);
    auto ret = duk_put_prop_index(ctx, -2, cb_info.stashId); // stack -1
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

    duk_push_uint(ctx, cb_info.stashId);
    return chk.Return(1);
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
        duk_get_global_string(ctx, "__ltk_callbacks");
        auto &cb_vec = iter->second;
        for (UINT i = 0; i < cb_vec.size();)
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
                i ++;
            }
            else
            {
                LOG("remove listener:" << cb_vec[i].stashId); // DUK_TYPE_UNDEFINED
                duk_pop(ctx); // pop undefined
                // if not exists remove current entry
                for (UINT j = i; j + 1 < cb_vec.size(); j++)
                {
                    cb_vec[j] = cb_vec[j + 1];
                }
                cb_vec.pop_back();
            }
        }
        duk_pop(ctx); // pop stash
    }
    duk_pop_n(ctx, nargs); // pop nargs
}

DWORD DukObject::m_sNextId = (DWORD)(-1);

} // namespace ltk
