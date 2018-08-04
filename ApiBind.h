#pragma once

namespace ltk {

    duk_ret_t ApiBindInit(duk_context *ctx);

    duk_ret_t RunMessageLoop(duk_context *ctx);

    bool DukGetRect(duk_context *ctx, duk_idx_t idx, Gdiplus::RectF &out);

} // namespace ltk