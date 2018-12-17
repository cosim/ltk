#pragma once

#include "RTTI.h"
#include "Common.h"
#include "stdafx.h"

namespace ltk {

class RefCounted : public RTTI
{
protected:
    RefCounted()
    {
        m_refCount = 1;
    }
    virtual ~RefCounted()
    {
    }

public:
    void AddRef()
    {
        m_refCount++;
    }
    void Release()
    {
        m_refCount--;
        assert(m_refCount >= 0);
        if (m_refCount == 0)
        {
            delete this;
        }
    }
    int RefCount()
    {
        return m_refCount;
    }
    bool HasOneRef()
    {
        return m_refCount == 1;
    }

    RTTI_DECLARATIONS(RefCounted, RTTI);

private:
    DISALLOW_COPY_AND_ASSIGN(RefCounted);
    int m_refCount;
};

template<typename T>
class RefPtr
{
public:
    RefPtr() : m_ptr(nullptr) {
        static_assert(std::is_convertible<T*, RefCounted*>::value,
            "RefCounted * required!");
    }
    explicit RefPtr(T *ptr) : m_ptr(ptr) {}

    RefPtr(const RefPtr &rhs) {
        m_ptr = rhs.m_ptr;
        m_ptr->AddRef();
    }

    RefPtr(RefPtr &&rhs) {
        m_ptr = rhs.m_ptr;
        rhs.m_ptr = nullptr;
    }

    ~RefPtr() {
        if (m_ptr) {
            m_ptr->Release();
        }
    }

    T *operator->() {
        return m_ptr;
    }

    operator bool() {
        return m_ptr != nullptr;
    }

    void operator=(const RefPtr &rhs) {
        if (m_ptr) {
            m_ptr->Release();
        }
        m_ptr = rhs.m_ptr;
        m_ptr->AddRef();
    }

    void operator=(RefPtr &&rhs) {
        if (m_ptr) {
            m_ptr->Release();
        }
        m_ptr = rhs.m_ptr;
        rhs.m_ptr = nullptr;
    }

    template <typename Q>
    T* operator=(const RefPtr<Q>& ptr) throw()
    {
        if (ptr->Is<T>()) {
            if (m_ptr) {
                m_ptr->Release();
            }
            m_ptr = ptr->As<T>();
        }
    }

    T *Get() {
        return m_ptr;
    }

    void Reset(T *ptr) {
        if (m_ptr) {
            m_ptr->Release();
        }
        m_ptr = ptr;
        m_ptr->AddRef();
    }

private:
    T *m_ptr;
};

} // namespace ltk