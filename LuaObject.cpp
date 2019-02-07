/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "StdAfx.h"
#include "LuaObject.h"
#include "UniConversion.h"
#include "Common.h"

namespace ltk {

std::unordered_map<LuaObject *, int> LuaObject::m_mapUserdata;

LuaObject::~LuaObject(void)
{
}

void LuaObject::PushToLua( lua_State *L, const char* clsName )
{
	LuaStackCheck check(L, 1);
    auto iter = m_mapUserdata.find(this);
    if (iter == m_mapUserdata.end()) {
        LuaObject ** ppThis = (LuaObject **)lua_newuserdata(L, sizeof(LuaObject *));
        *ppThis = this; // [udata]
        this->AddRef();
        int udata = lua_gettop(L);

        luaL_getmetatable(L, clsName); // [udata] [meta]
        if (!lua_istable(L, -1))
        {
            LTK_LOG("<%s> IS NOT REGISTERD!!! ", clsName);
            __debugbreak();
        }
        lua_setmetatable(L, udata); // [udata]
        
        this->GetWeakTable(L); // [udata] [weak]
        lua_pushvalue(L, -2); // [udata] [weak] [udata]
        int ref = luaL_ref(L, -2); // [udata] [weak]
        lua_pop(L, 1); // [udata] 
        m_mapUserdata[this] = ref;
    }
    else {
        this->GetWeakTable(L); // [weak]
        lua_rawgeti(L, -1, iter->second); // [weak] [udata]
        lua_remove(L, -2); // [udata]
        if (!lua_isuserdata(L, -1)) {
            __debugbreak(); // what if the weak ref has been GCed.
        }
    }
}

int LuaObject::GCMethod( lua_State *L )
{
	LuaObject **ppObj = (LuaObject **)lua_touserdata(L, 1);
    LuaObject* thiz = nullptr;
    if (ppObj) // ÊÇ²»ÊÇuserdata
	{
        thiz = *ppObj;
        if (!thiz) {
            return 0;
        }
        if (!thiz->Is(TypeIdClass()))
		{
			LTK_ASSERT(false);
			return 0;
		}
	}
	thiz->Release();

	return 0;
}

int LuaObject::GetHandle(lua_State *L)
{
    LuaObject *thiz = CheckLuaObject<LuaObject>(L, 1);
    lua_pushlightuserdata(L, thiz);
    return 1;
}

void LuaObject::GetWeakTable( lua_State *L )
{
    LuaStackCheck chk(L, 1);
	lua_pushliteral(L, "LuaObjectWeakRef");
	lua_rawget(L, LUA_REGISTRYINDEX);
	if (lua_istable(L, -1))
	{
		return;
	}
	else
	{
        lua_pop(L, 1);
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

int LuaObject::ReleaseReference(lua_State *L)
{
    RTTI **ppObj = (RTTI **)lua_touserdata(L, 1);
    if (ppObj)
    {
        RTTI* obj = *ppObj;
        if (obj) {
            if (obj->Is(LuaObject::TypeIdClass()))
            {
                LuaObject *lobj = obj->As<LuaObject>();
                lobj->Release();
                *ppObj = nullptr;
                return 0;
            }
            else{
                return luaL_error(L, "TypeError: #1 is not a Ltk.Object");
            }
        }
        else {
            return luaL_error(L, "TypeError: #1 invalid reference.");
        }
    }
    else {
        return luaL_error(L, "TypeError: #1 is not a userdata");
    }
}

int LuaObject::GetEventHandler(lua_State *L)
{
    LuaObject *thiz = CheckLuaObject<LuaObject>(L, 1);
    if (LUA_NOREF == thiz->m_refEventHandler)
    {
        lua_pushnil(L);
        return 1;
    }
    GetWeakTable(L);
    int ref_table = lua_gettop(L);
    lua_rawgeti(L, ref_table, thiz->m_refEventHandler);
    return 1;
}

int LuaObject::SetEventHandler(lua_State *L)
{
    LuaObject *thiz = CheckLuaObject<LuaObject>(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE); // thiz event
    GetWeakTable(L); // thiz event weak
    int ref_table = lua_gettop(L);
    if (LUA_NOREF != thiz->m_refEventHandler)
    {
        luaL_unref(L, ref_table, thiz->m_refEventHandler);
    }
    lua_pushvalue(L, 2); // thiz event weak event
    thiz->m_refEventHandler = luaL_ref(L, ref_table); // thiz event weak
    lua_pop(L, 1); // for the weak ref_table
    lua_pushvalue(L, 2); // thiz event event
    return 1;
}

bool LuaObject::CallEventHandler(lua_State *L, const char *name, int nargs, int nresult)
{
    if (LUA_NOREF == m_refEventHandler)
    {
        goto failed;
    }
    GetWeakTable(L); // ref_table
    lua_rawgeti(L, -1, m_refEventHandler);
    int tblMessage = lua_gettop(L); // ref_table, tblMessage
    lua_remove(L, -2); // for ref_table
    if (!lua_istable(L, -1))
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
        nresult--;
    }
    return false;
}

} // namespace ltk
