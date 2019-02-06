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

	// ������������lua�� ����һ���µ�userdata����֮ ���ü���+1 luaջ+1
    void PushToLua(lua_State *L, const char* clsName);

	// nargs �������� ������this
	//void CallLuaMethod(lua_State *L, const char *method, int nargs, int nresult);

    static int LuaAddEventListener(lua_State *L);
    static int LuaRemoveListener(lua_State *L);
    static int LuaRemoveAllListener(lua_State *L);
    bool LuaDispatchEvent(lua_State *L, const char *name, int nargs, int nresult);
	
private:
    void CallOnEvent(lua_State *L, int listener, const char *name, int nargs);
    static void GetWeakTable(lua_State *L);
    
    ArrayList<int> m_listListener;
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
    lua_setfield(L, metatable, "__metatable"); // ��ֹlua��۸�

    lua_pushcfunction(L, T::LuaConstructor);
    lua_setfield(L, methods, "new");

    lua_pushcfunction(L, T::LuaAddEventListener);
    lua_setfield(L, methods, "AddEventListener");

    lua_pushcfunction(L, T::LuaRemoveListener);
    lua_setfield(L, methods, "RemoveListener");

    lua_pushcfunction(L, T::LuaRemoveAllListener);
    lua_setfield(L, methods, "RemoveAllListener");

    lua_pushcfunction(L, T::GetHandle);
    lua_setfield(L, methods, "GetHandle");

    lua_pushcfunction(L, T::ReleaseReference);
    lua_setfield(L, methods, "Unref");

    // ע�᷽��
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

// ����ʱ�Զ�ƽ��lua��ջ
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
