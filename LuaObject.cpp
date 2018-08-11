/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "StdAfx.h"
#include "LuaObject.h"
#include "UniConversion.h"
#include "Common.h"

namespace ltk {

void LuaObject::PushToLua( lua_State *L, const char* clsName )
{
	LuaStackCheck check(L);
	this->Ref();
	LuaObject ** ppThis = (LuaObject **)lua_newuserdata(L, sizeof(LuaObject *));
	*ppThis = this;
	int udata = lua_gettop(L);

	// 检查下是不是已经注册过了
	luaL_getmetatable(L, clsName);
	if(!lua_istable(L, -1))
	{
        LOG("NOT REGISTERD!!! " << clsName);
		lua_pop(L, 1); // pop nil
		lua_pushnil(L);
        check.SetReturn(1);
        return;
	}
	lua_setmetatable(L, udata); // udata
	//int type = lua_type(L, lua_gettop(L));
	check.SetReturn(1);
}

int LuaObject::GCMethod( lua_State *L )
{
	LuaObject **ppObj = (LuaObject **)lua_touserdata(L, 1);
    LuaObject* thiz = nullptr;
    if (ppObj) // 是不是userdata
	{
        thiz = *ppObj;
        if (!thiz->Is(TypeIdClass()))
		{
			assert(false);
			OutputDebugStringA("GCMethod TypeError\r\n");
			return 0;
		}
	}
	if (thiz->RefCount() == 1)
	{
		if (LUA_NOREF != thiz->m_refUserData)
		{
			thiz->GetWeakTable(L);
			luaL_unref(L, -1, thiz->m_refUserData);
		}
	}
	thiz->Unref();

	return 0;
}

int LuaObject::GetEventHandler( lua_State *L )
{
	LuaObject *thiz = CheckLuaObject<LuaObject>(L, 1);
	if (LUA_NOREF == thiz->m_refUserData)
	{
		lua_pushnil(L);
		return 1;
	}
	GetWeakTable(L);
	int ref_table = lua_gettop(L);
	lua_rawgeti(L, ref_table, thiz->m_refUserData);
	return 1;
}

int LuaObject::SetEventHandler(lua_State *L)
{
	LuaObject *thiz = CheckLuaObject<LuaObject>(L, 1);
	luaL_checktype(L, 2, LUA_TTABLE); // thiz event
	GetWeakTable(L); // thiz event weak
	int ref_table = lua_gettop(L); 
	if (LUA_NOREF != thiz->m_refUserData)
	{
		luaL_unref(L, ref_table, thiz->m_refUserData);
	}
	lua_pushvalue(L, 2); // thiz event weak event
	thiz->m_refUserData = luaL_ref(L, ref_table); // thiz event weak
	lua_pop(L, 1); // for the weak ref_table
    lua_pushvalue(L, 2); // thiz event event
    return 1;
}

bool LuaObject::CallEventHandler( lua_State *L, const char *name, int nargs, int nresult )
{
	if (LUA_NOREF == m_refUserData)
	{
		goto failed;
	}
	GetWeakTable(L); // ref_table
	lua_rawgeti(L, -1, m_refUserData);
	int tblMessage = lua_gettop(L); // ref_table, tblMessage
	lua_remove(L, -2); // for ref_table
	if(!lua_istable(L, -1))
	{
		goto failed;
	}
	lua_getfield(L, -1, name);      // tblMessage, callback
	if (lua_isfunction(L, -1))
	{
		lua_remove(L, -2);              // callback
		lua_insert(L, lua_gettop(L) - nargs);
		//lua_call(L, nargs, nresult);
		LuaPCall(L, nargs, nresult);
	}
	else
	{
		lua_pop(L, 2); // pop nil, tblMessage
		goto failed;
	}
	return true;

failed:
	while (nresult > 0)
	{
		lua_pushnil(L);
		nresult --;
	}
	return false;
}

void LuaObject::GetWeakTable( lua_State *L )
{
	lua_pushliteral(L, "LuaObjectWeakRef");
	lua_rawget(L, LUA_REGISTRYINDEX);
	if (lua_istable(L, -1))
	{
		return;
	}
	else
	{
		lua_newtable(L); // 1 local t = {}
		int ref_table = lua_gettop(L);
		lua_pushvalue(L, ref_table);     // 2
		lua_setmetatable(L, ref_table); // 1 setmetatable(t, t);
		lua_pushliteral(L, "__mode");  // 2
		lua_pushliteral(L, "v");     // 3
		lua_rawset(L, ref_table);   // 1 t.__mode = "v"

		lua_pushliteral(L, "LuaObjectWeakRef"); // 2
		lua_pushvalue(L, ref_table); // 3
		lua_rawset(L, LUA_REGISTRYINDEX); //1
	}
}

} // namespace ltk
