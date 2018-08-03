#include "stdafx.h"
#include "ApiBind.h"
#include "DukObject.h"

namespace ltk {

static const char *ApiNamespace = "ltk";

struct DukFunctionList
{
    const char *name;
    duk_c_function fn;
    duk_idx_t nargs;
};

duk_ret_t DukRegisterFunctionList(duk_context *ctx, DukFunctionList *list, const char* name)
{
    DukStackChecker check(ctx);
    duk_push_object(ctx); // {}
    auto obj = duk_get_top(ctx);
    for (UINT i = 0; list[i].fn; i ++)
    {
        duk_push_c_function(ctx, list[i].fn, list[i].nargs);
        duk_put_prop_string(ctx, obj, list[i].name);
    }
    duk_put_global_string(ctx, name);
    return check.Return(1);
}

duk_ret_t RunMessageLoop(duk_context *ctx)
{
    
    return 0;
}

} // namespace ltk
