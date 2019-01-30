// Original author: Paul Varcholik
// Forked author: Shao Voon Wong
// version 0.1: Changed sRunTimeTypeId type from unsigned int to size_t
// version 0.2: Replaced sRunTimeTypeId with a static local variable, so the class is a header only. Credit: Andrew Fedoniouk aka c-smile
// https://www.codeproject.com/Tips/1068171/Cplusplus-Custom-RTTI

#pragma once

#include <string>
#include "Container.h"

namespace ltk {
    struct Event;

    class RTTI
    {
    public:
        virtual bool OnEvent(Event *ev) { return false; }

        virtual const size_t TypeIdInstance() const = 0;
        virtual std::string TypeNameInstance() const = 0;

        virtual RTTI* QueryInterface(const size_t)
        {
            return NULL;
        }
        virtual const RTTI* QueryInterface(const size_t) const
        {
            return NULL;
        }

        virtual bool Is(const size_t id) const
        {
            return false;
        }

        //virtual bool Is(const std::string& name) const
        //{
        //    return false;
        //}

        template <typename T>
        T* As()
        {
            if (Is(T::TypeIdClass()))
            {
                return (T*)this;
            }

            return NULL;
        }
        template <typename T>
        const T* As() const
        {
            if (Is(T::TypeIdClass()))
            {
                return (T*)this;
            }

            return NULL;
        }
    };

#define RTTI_DECLARATIONS(Type, ParentType)                            \
    public:                                                            \
        virtual std::string TypeNameInstance() const { return std::string(#Type); }   \
        virtual const size_t TypeIdInstance() const                    \
        { return Type::TypeIdClass(); }                                \
        static const size_t TypeIdClass()                              \
        { static int d = 0; return (size_t) &d; }                      \
        virtual RTTI* QueryInterface( const size_t id )                \
        {                                                              \
            if (id == TypeIdClass())                                   \
                { return (RTTI*)this; }                                \
            else                                                       \
                { return ParentType::QueryInterface(id); }             \
        }                                                              \
        virtual const RTTI* QueryInterface( const size_t id ) const    \
        {                                                              \
            if (id == TypeIdClass())                                   \
                { return (RTTI*)this; }                                \
            else                                                       \
                { return ParentType::QueryInterface(id); }             \
        }                                                              \
        virtual bool Is(const size_t id) const                         \
        {                                                              \
            if (id == TypeIdClass())                                   \
                { return true; }                                       \
            else                                                       \
                { return ParentType::Is(id); }                         \
        }                                                              
/*        virtual bool Is(const std::string& name) const                 \
            {                                                              \
                if (name == TypeNameInstance())                                    \
                    { return true; }                                       \
                else                                                       \
                    { return ParentType::Is(name); }                       \
            }   */                                                           


    class EventSource
    {
    public:
        void AddEventListener(RTTI *v)
        {
            for (UINT i = 0; i < m_listener.Length(); i++) {
                if (m_listener[i] == v) {
                    return;
                }
            }
            m_listener.PushBack(v);
        }

        void DispatchEvent(Event *e)
        {
            for (UINT i = 0; i < m_listener.Length(); i++) {
                m_listener[i]->OnEvent(e);
            }
        }

    private:
        ArrayList<RTTI *> m_listener;
    };

} // namespace ltk
