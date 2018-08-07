/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/
#pragma once

#include "stdafx.h"
#include "Common.h"
#include "RefCounted.h"

namespace ltk {

class LuaObject : public RefCounted
{
public:
	LuaObject(void) : m_refUserData(LUA_NOREF) {}
	~LuaObject(void){}

    RTTI_DECLARATIONS(LuaObject, RefCounted);

	virtual const char* GetClassName() = 0;

	virtual void RegisterMethods(lua_State *L, int tblMethod) = 0;

	static int GCMethod(lua_State *L);

	static int GetCallbacks(lua_State *L);

	static int SetCallbacks(lua_State *L);

	// 把这个对象放入lua中 创建一个新的userdata包裹之 引用计数+1 lua栈+1
	void PushToLua(lua_State *L);

	// nargs 参数个数 不包括this
	//void CallLuaMethod(lua_State *L, const char *method, int nargs, int nresult);

	bool InvokeCallback(lua_State *L, const char *name, int nargs, int nresult);
	
private:
	static void GetWeakTable( lua_State *L );

	//int m_refEventTable;
	int m_refUserData;
};

#define BEGIN_LUA_METHOD_MAP(x)  virtual const char *GetClassName() {return #x;} \
virtual void RegisterMethods( lua_State *L, int metatable) {

#define LUA_METHOD_ENTRY(x) 	lua_pushcfunction(L, x); lua_setfield(L, metatable, #x);

#define END_LUA_METHOD_MAP() }

#define LUA_CHAIN_METHOD_MAP(parentType) parentType::RegisterMethods(L, metatable);

template<typename T>
T* LuaRegisterClass(lua_State *L, const char *className)
{
    LuaStackCheck check(L);

    int ret = luaL_newmetatable(L, className);
    assert(ret);
    int metatable = lua_gettop(L); // local metatable = {}
    lua_newtable(L);
    int methods = lua_gettop(L); // local methods = {}

    lua_pushvalue(L, methods);
    lua_setfield(L, metatable, "__index");  // metatable.__index = methods

    lua_pushcfunction(L, GCMethod);
    lua_setfield(L, metatable, "__gc"); // metatable.__gc = GCMethod

    lua_pushvalue(L, methods);
    lua_setfield(L, metatable, "__metatable"); // 防止lua里篡改

    lua_pushcfunction(L, T::LuaConstructor);
    lua_setfield(L, methods, "new");

    lua_pushcfunction(L, GetCallbacks);
    lua_setfield(L, methods, "GetCallbacks");

    lua_pushcfunction(L, SetCallbacks);
    lua_setfield(L, methods, "SetCallbacks");

    // 注册方法
    T::RegisterMethods(L, methods);

    lua_pushvalue(L, methods);
    lua_setglobal(L, className); // 设置成全局这样lua也可以添加方法在上面
    
    lua_pop(L, 1); // for metatable
    lua_pop(L, 1); // for methods

    return check.SetReturn(0);
}

template<class T>
T* CheckLuaObject( lua_State *L, int idx)
{
    RTTI **ppObj = (RTTI **)lua_touserdata(L, idx);
	if (ppObj)
	{
        RTTI* obj = *ppObj;
		assert(obj);
		if (obj->Is(T::TypeIdClass()))
		{
			return (T*)obj;
		}
	}
	luaL_error(L, "C object type checking failed: #%d is not a %s", idx, T::TypeName().c_str());
	return NULL;
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
        if (lua_gettop(L) > m_top)
        {
            assert(false);
            //int top = lua_gettop(L);
            lua_settop(L, m_top);
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
