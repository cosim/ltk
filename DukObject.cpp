#include "stdafx.h"
#include "DukObject.h"

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
    DukObject *thiz = DukCheckThis<DukObject>(ctx);
    if (!thiz) return DUK_RET_TYPE_ERROR;
    auto pszEvent = duk_require_string(ctx, 0);
    if (!pszEvent) return DUK_RET_TYPE_ERROR;

    return 0;
}

DWORD DukObject::m_sNextId = 0;

} // namespace ltk
