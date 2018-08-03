#include "stdafx.h"
#include "DukObject.h"

namespace ltk {

const char * DukObject::DukPropName = DUK_HIDDEN_SYMBOL("LtkObject");

DukObject::DukObject()
{
}

DukObject::~DukObject()
{
}

DukObject * DukObject::DukCheckType(duk_context *ctx, duk_idx_t idx, size_t type_id)
{
    duk_get_prop_string(ctx, idx, DukPropName);
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


} // namespace ltk
