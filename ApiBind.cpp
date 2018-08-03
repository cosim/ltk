#include "stdafx.h"
#include "ApiBind.h"
#include "DukObject.h"

namespace ltk {

static const char *ApiNamespace = "ltk";

#define CONSTANT_ENTRY(x) { #x, (double)x }

static const duk_number_list_entry w32_consts[] = {
    CONSTANT_ENTRY(WS_OVERLAPPEDWINDOW),
    CONSTANT_ENTRY(WS_OVERLAPPED),
    CONSTANT_ENTRY(WS_POPUP),
    CONSTANT_ENTRY(WS_CHILD),
    CONSTANT_ENTRY(WS_MINIMIZE),
    CONSTANT_ENTRY(WS_VISIBLE),
    CONSTANT_ENTRY(WS_DISABLED),
    CONSTANT_ENTRY(WS_CLIPSIBLINGS),
    CONSTANT_ENTRY(WS_CLIPCHILDREN),
    CONSTANT_ENTRY(WS_MAXIMIZE),
    CONSTANT_ENTRY(WS_CAPTION),
    CONSTANT_ENTRY(WS_BORDER),
    CONSTANT_ENTRY(WS_DLGFRAME),
    CONSTANT_ENTRY(WS_VSCROLL),
    CONSTANT_ENTRY(WS_HSCROLL),
    CONSTANT_ENTRY(WS_SYSMENU),
    CONSTANT_ENTRY(WS_THICKFRAME),
    CONSTANT_ENTRY(WS_GROUP),
    CONSTANT_ENTRY(WS_TABSTOP),
    CONSTANT_ENTRY(WS_MINIMIZEBOX),
    CONSTANT_ENTRY(WS_MAXIMIZEBOX),
    { NULL, 0.0 }
};

#undef CONSTANT_ENTRY

duk_ret_t RunMessageLoop(duk_context *ctx)
{
    
    return 0;
}

duk_ret_t ApiBindInit(duk_context *ctx)
{
    duk_push_object(ctx);
    duk_put_number_list(ctx, -1, w32_consts);
    duk_put_global_string(ctx, "win32");
    return 0;
}

} // namespace ltk
