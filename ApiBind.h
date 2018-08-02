#pragma once

namespace ltk {

    struct DukFunctionList;

    duk_ret_t DukRegisterFunctionList(duk_context *ctx, DukFunctionList *list, const char* name);

    duk_ret_t RunMessageLoop(duk_context *ctx);

} // namespace ltk