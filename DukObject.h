#pragma once
#include "RefCounted.h"
#include "stdafx.h"

namespace ltk {

#define BEGIN_DUK_METHOD_MAP(cls) static void DukRegisterMethods(duk_context *ctx) {

#define DUK_METHOD_ENTRY(x, nargs) duk_push_c_function(ctx, x, nargs); duk_put_prop_string(ctx, -2, #x);

#define DUK_CHAIN_METHOD_MAP(parentType) parentType::DukRegisterMethods(ctx);

#define END_DUK_METHOD_MAP() }

struct DukCallbackInfo
{
    DWORD stashId;
};

class DukObject : public RefCounted
{
protected:
    virtual ~DukObject();

public:
    static DWORD m_sNextId;

    DukObject();

    static DWORD GetNextId();

    static duk_ret_t DukFinalizer(duk_context *ctx);

    static duk_ret_t AddListener(duk_context *ctx);

    static duk_ret_t RemoveListener(duk_context *ctx);

    static duk_ret_t RemoveAllListeners(duk_context *ctx);

    void DispatchEvent(duk_context *ctx, const char *event_name, duk_idx_t nargs);

    RTTI_DECLARATIONS(DukObject, RefCounted)

    BEGIN_DUK_METHOD_MAP(DukObject)
        DUK_METHOD_ENTRY(AddListener, 2)
    END_DUK_METHOD_MAP()

private:
    std::unordered_map<std::string, std::vector<DukCallbackInfo>> m_callbackMap;
};

extern const char *DukThisSymbol;

DukObject *DukCheckType(duk_context *ctx, duk_idx_t idx, size_t type_id);

template<typename T>
T *DukCheckThis(duk_context *ctx)
{
    duk_push_this(ctx);
    auto thiz = (T *)DukCheckType(ctx, -1, T::TypeIdClass());
    duk_pop(ctx); // for this
    return thiz;
}


void DukPrintStack(duk_context *ctx);

void DukLog(const char *psz);

bool DukPCall(duk_context *ctx, duk_idx_t nargs);

template<typename T>
void DukRegisterClass(duk_context *ctx, const char *name)
{
    DukStackChecker check(ctx);
    duk_push_c_function(ctx, T::DukConstructor, 0); // ctor
    duk_push_object(ctx); // ctor proto
    duk_push_c_function(ctx, DukObject::DukFinalizer, 2);
    duk_set_finalizer(ctx, -2);
    T::DukRegisterMethods(ctx);
    duk_put_prop_string(ctx, -2, "prototype"); // ctor
    duk_put_global_string(ctx, name); // empty
}

class DukStackChecker
{
public:
    DukStackChecker(duk_context *ctx_)
    {
        ctx = ctx_;
        init_top = duk_get_top(ctx);
        num_ret = 0;
    }

    ~DukStackChecker()
    {
        auto top = duk_get_top(ctx);
        if (top - num_ret != init_top)
        {
            __debugbreak();
        }
    }

    duk_idx_t Return(duk_idx_t num)
    {
        num_ret = num;
        return num_ret;
    }

private:
    duk_context *ctx;
    duk_idx_t init_top;
    duk_idx_t num_ret;
};

} // namespace ltk