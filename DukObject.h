#pragma once
#include "RefCounted.h"
#include "stdafx.h"

namespace ltk {

class DukObject : public RefCounted
{
protected:
    virtual ~DukObject();

public:
    static const char *DukPropName;

    DukObject();

    static DukObject *DukCheckType(duk_context *ctx, duk_idx_t idx, size_t type_id);

    // regiseter motheds to prototype object which is on the top of stack.
    // virtual void RegisterMethods(duk_context *ctx) = 0;

    duk_ret_t AddListener(duk_context *ctx);

    duk_ret_t RemoveListener(duk_context *ctx);

    duk_ret_t RemoveAllListeners(duk_context *ctx);

    RTTI_DECLARATIONS(DukObject, RefCounted)
};

#define BEGIN_DUK_METHOD_MAP(cls) virtual const char *DukClassName() { return #cls; } \
static void RegisterMethods(duk_context *ctx) {

#define DUK_METHOD_ENTRY(x, nargs) duk_push_c_function(ctx, x, nargs); duk_put_prop_string(ctx, -2, #x);

#define DUK_CHAIN_METHOD_MAP(parentType) parentType::RegisterMethods(L);

#define END_DUK_METHOD_MAP() }

void DukPrintStack(duk_context *ctx);

} // namespace ltk