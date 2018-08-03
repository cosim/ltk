#pragma once

namespace ltk {

    duk_ret_t ApiBindInit(duk_context *ctx);

    duk_ret_t RunMessageLoop(duk_context *ctx);

} // namespace ltk