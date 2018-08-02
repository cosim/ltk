#include "stdafx.h"
#include "DukObject.h"

namespace ltk {

const char * DukObject::DukPropName = "\ffLtkObject";

DukObject::DukObject()
{
}

DukObject::~DukObject()
{
}

DukObject * DukObject::CheckType(duk_context *ctx, duk_idx_t idx, size_t type_id)
{
    duk_get_prop_string(ctx, 0, DukPropName);
    RTTI *rtti = (RTTI *)duk_require_pointer(ctx, idx);

    if (!rtti || !rtti->Is(type_id)) {
        return nullptr;
    }
}


} // namespace ltk
