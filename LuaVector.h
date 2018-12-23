#pragma once

#include "LuaObject.h"

template <typename T>
class LuaVector : public LuaObject
{
public:
    LuaVector() {}
    ~LuaVector() {}

    static int LuaConstructor(lua_State *L);
    static int Reserve(lua_State *L);
    static int Get(lua_State *L);
    static int Set(lua_State *L);
    static int PushBack(lua_State *L);

    BEGIN_LUA_METHOD_MAP(Button)
        LUA_METHOD_ENTRY(Reserve)
        LUA_METHOD_ENTRY(Get)
        LUA_METHOD_ENTRY(Set)
        LUA_METHOD_ENTRY(PushBack)
    END_LUA_METHOD_MAP()

private:
    std::vector<T> m_vec; // TODO uvector
};

template <typename T>
const char *get_vec_typename()
{
    throw("never reach here");
}

template <>
const char *get_vec_typename<unsigned char>()
{
    return "VecU8";
}

template <typename T>
int LuaVector<T>::LuaConstructor(lua_State *L)
{
    LuaVector<T> *thiz = new LuaVector<T>;
    thiz->PushToLua(L, get_vec_typename<T>());
    thiz->Release();
    return 1;
}

template <typename T>
int LuaVector<T>::PushBack(lua_State *L)
{
    LuaVector<T> *thiz = CheckLuaObject<LuaVector<T>>(L, 1);
    size_t elem = (size_t)luaL_checkinteger(L, 2);
    thiz->m_vec.reserve(elem);
    return 0;
}

template <typename T>
int LuaVector<T>::Set(lua_State *L)
{
    LuaVector<T> *thiz = CheckLuaObject<LuaVector<T>>(L, 1);
    size_t idx = (size_t)luaL_checkinteger(L, 2);
    T elem = (T)luaL_checknumber(L, 3);

    idx--;
    if (idx >= thiz->m_vec.size()) {
        luaL_error(L, "out of range");
    }
    thiz->m_vec[idx] = elem;
    return 0;
}

template <typename T>
int LuaVector<T>::Get(lua_State *L)
{
    LuaVector<T> *thiz = CheckLuaObject<LuaVector<T>>(L, 1);
    size_t idx = (size_t)luaL_checkinteger(L, 2);

    if (idx >= thiz->m_vec.size()) {
        luaL_error(L, "out of range");
    }
    T elem = thiz->m_vec[idx];
    lua_pushnumber(L, (double)elem);
    return 1;
}

template <typename T>
int LuaVector<T>::Reserve(lua_State *L)
{
    return 0;
}
