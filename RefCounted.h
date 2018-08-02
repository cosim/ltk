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
    void Ref()
    {
        m_refCount++;
    }
    void Unref()
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

    RTTI_DECLARATIONS(RefCounted, RTTI);

private:
    DISALLOW_COPY_AND_ASSIGN(RefCounted);
    int m_refCount;
};

} // namespace ltk