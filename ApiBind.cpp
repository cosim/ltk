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
    //////////////////////////////////////////
    CONSTANT_ENTRY(SW_HIDE),
    CONSTANT_ENTRY(SW_SHOWNORMAL),
    CONSTANT_ENTRY(SW_SHOWMINIMIZED),
    CONSTANT_ENTRY(SW_SHOWMAXIMIZED),
    CONSTANT_ENTRY(SW_SHOWNOACTIVATE),
    CONSTANT_ENTRY(SW_SHOW),
    CONSTANT_ENTRY(SW_MINIMIZE),
    CONSTANT_ENTRY(SW_SHOWMINNOACTIVE),
    CONSTANT_ENTRY(SW_SHOWNA),
    CONSTANT_ENTRY(SW_RESTORE),
    CONSTANT_ENTRY(SW_SHOWDEFAULT),
    CONSTANT_ENTRY(SW_FORCEMINIMIZE),
    { NULL, 0.0 }
};

#undef CONSTANT_ENTRY

class DtorTest
{
public:
    ~DtorTest() { LOG("."); }
};

duk_ret_t _PostQuitMessage(duk_context *ctx)
{
    DtorTest d;
    duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "test");
    duk_throw(ctx);
    ::PostQuitMessage(0);
    return 0;
}

static const duk_function_list_entry api_list[] = {
    {"PostQuitMessage", _PostQuitMessage, 0},
    {nullptr, nullptr, 0}
};

duk_ret_t RunMessageLoop(duk_context *ctx)
{
    
    return 0;
}

duk_ret_t ApiBindInit(duk_context *ctx)
{
    duk_push_object(ctx);
    duk_put_number_list(ctx, -1, w32_consts);
    duk_put_function_list(ctx, -1, api_list);
    duk_put_global_string(ctx, "win32");
    return 0;
}

bool DukGetRect(duk_context *ctx, duk_idx_t idx, Gdiplus::RectF &out)
{
    DukStackChecker chk(ctx);

    if (!duk_is_object(ctx, idx)) return false;

    duk_get_prop_string(ctx, idx, "x");
    if (!duk_is_number(ctx, -1)) {
        duk_pop(ctx);
        return false;
    }
    out.X = (float)duk_get_number(ctx, -1);
    duk_pop(ctx);

    duk_get_prop_string(ctx, idx, "y");
    if (!duk_is_number(ctx, -1)) {
        duk_pop(ctx);
        return false;
    }
    out.Y = (float)duk_get_number(ctx, -1);
    duk_pop(ctx);

    duk_get_prop_string(ctx, idx, "w");
    if (!duk_is_number(ctx, -1)) {
        duk_pop(ctx);
        return false;
    }
    out.Width = (float)duk_get_number(ctx, -1);
    duk_pop(ctx);

    duk_get_prop_string(ctx, idx, "h");
    if (!duk_is_number(ctx, -1)) {
        duk_pop(ctx);
        return false;
    }
    out.Height = (float)duk_get_number(ctx, -1);
    duk_pop(ctx);

    return true;
}

} // namespace ltk
