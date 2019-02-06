/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/
#pragma once

#include "stdafx.h"
#include "Common.h"
#include "RefCounted.h"
#include "Container.h"

namespace ltk {

class LuaObject : public RefCounted
{
public:
	LuaObject(void){}
	~LuaObject(void);

    RTTI_DECLARATIONS(LuaObject, RefCounted);

	static int GCMethod(lua_State *L);
    static int GetHandle(lua_State *L);
    static int ReleaseReference(lua_State *L);

    static int GetEventHandler(lua_State *L);
    static int SetEventHandler(lua_State *L);

    bool CallEventHandler(lua_State *L, const char *name, int nargs, int nresult);
    // 把这个对象放入lua中 创建一个新的userdata包裹之 引用计数+1 lua栈+1
    void PushToLua(lua_State *L, const char* clsName);

	// nargs 参数个数 不包括this
	//void CallLuaMethod(lua_State *L, const char *method, int nargs, int nresult);

	
private:
    static void GetWeakTable(lua_State *L);
    
    int m_refUserData;
};

#define BEGIN_LUA_METHOD_MAP(x)  static void RegisterMethods(lua_State *L, int metatable) {

#define LUA_METHOD_ENTRY(x) 	lua_pushcfunction(L, x); lua_setfield(L, metatable, #x);

#define END_LUA_METHOD_MAP() }

#define LUA_CHAIN_METHOD_MAP(parentType) parentType::RegisterMethods(L, metatable);

template<typename T>
void LuaRegisterClass(lua_State *L, const char *className)
{
    LuaStackCheck check(L);

    int ret = luaL_newmetatable(L, className);
    assert(ret);
    int metatable = lua_gettop(L); // local metatable = {}
    lua_newtable(L);
    int methods = lua_gettop(L); // local methods = {}

    lua_pushvalue(L, methods);
    lua_setfield(L, metatable, "__index");  // metatable.__index = methods

    lua_pushcfunction(L, T::GCMethod);
    lua_setfield(L, metatable, "__gc"); // metatable.__gc = GCMethod

    lua_pushvalue(L, methods);
    lua_setfield(L, metatable, "__metatable"); // 防止lua里篡改

    lua_pushcfunction(L, T::LuaConstructor);
    lua_setfield(L, methods, "new");

    lua_pushcfunction(L, T::GetEventHandler);
    lua_setfield(L, methods, "GetEventHandler");

    lua_pushcfunction(L, T::SetEventHandler);
    lua_setfield(L, methods, "SetEventHandler");

    lua_pushcfunction(L, T::GetHandle);
    lua_setfield(L, methods, "GetHandle");

    lua_pushcfunction(L, T::ReleaseReference);
    lua_setfield(L, methods, "Unref");

    // 注册方法
    T::RegisterMethods(L, methods);

    lua_getglobal(L, "Ltk");
    if (!lua_istable(L, -1)) {
        LTK_LOG("<FATAL> Ltk is missing.");
        __debugbreak();
    }
    else{
        lua_pushvalue(L, methods);
        lua_setfield(L, -2, className);
    }
    lua_pop(L, 1); // for Ltk
    lua_pop(L, 1); // for metatable
    lua_pop(L, 1); // for methods
}

template<class T>
T* CheckLuaObject( lua_State *L, int idx)
{
    RTTI **ppObj = (RTTI **)lua_touserdata(L, idx);
	if (ppObj)
	{
        RTTI* obj = *ppObj;
        if (obj) {
            if (obj->Is(T::TypeIdClass()))
            {
                return (T*)obj;
            }
            else{
                luaL_error(L, "TypeError: #%d is not a %s", idx, obj->TypeNameInstance().c_str());
                return nullptr;
            }
        }
        else {
            luaL_error(L, "TypeError: #%d invalid reference.");
            return nullptr;
        }
	}
    else {
        luaL_error(L, "TypeError: #%d is not a userdata", idx);
        return nullptr;
    }
}

// 析构时自动平衡lua堆栈
class LuaStackCheck
{
public:
    explicit LuaStackCheck(lua_State *L)
    {
        m_L = L;
        m_top = lua_gettop(L);
    }
    ~LuaStackCheck()
    {
        lua_State *L = m_L;
        int top = lua_gettop(L);
        if (top > m_top)
        {
            __debugbreak();
        }
    }
    int SetReturn(int n)
    {
        m_top += n;
        return n;
    }

private:
    lua_State *m_L;
    int m_top;
};

} // namespace ltk
