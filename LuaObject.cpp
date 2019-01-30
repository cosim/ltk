/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "StdAfx.h"
#include "LuaObject.h"
#include "UniConversion.h"
#include "Common.h"

namespace ltk {

LuaObject::~LuaObject(void)
{
    lua_State *L = GetGlobalLuaState();
    GetWeakTable(L); // [week]
    m_listListener.ForEach([=](int ref){
        luaL_unref(L, -1, ref);
    });
    lua_pop(L, 1);
}

void LuaObject::PushToLua( lua_State *L, const char* clsName )
{
	LuaStackCheck check(L);
	this->AddRef();
	LuaObject ** ppThis = (LuaObject **)lua_newuserdata(L, sizeof(LuaObject *));
	*ppThis = this;
	int udata = lua_gettop(L);

	luaL_getmetatable(L, clsName);
	if(!lua_istable(L, -1))
	{
        LTK_LOG("<%s> IS NOT REGISTERD!!! ", clsName);
        __debugbreak();
	}
	lua_setmetatable(L, udata); // udata
	check.SetReturn(1);
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

int LuaObject::AddEventListener(lua_State *L)
{
	LuaObject *thiz = CheckLuaObject<LuaObject>(L, 1);
	luaL_checktype(L, 2, LUA_TTABLE); // thiz event
	GetWeakTable(L); // thiz event weak
	int ref_table = lua_gettop(L); 
	lua_pushvalue(L, 2); // thiz event weak event
	int ref = luaL_ref(L, ref_table); // thiz event weak
	lua_pop(L, 1); // thiz event
    // TODO skip if has same table in the list
    thiz->m_listListener.PushBack(ref);
    return 0;
}

int LuaObject::RemoveListener(lua_State *L)
{
    // TODO we should not to use Cookie here, instead we could use the lua table as parameter
    LuaObject *thiz = CheckLuaObject<LuaObject>(L, 1);
    luaL_checktype(L, 2, LUA_TUSERDATA);
    auto cookie = lua_touserdata(L, 2);
    int ref = thiz->m_listListener.GetData(cookie);
    thiz->GetWeakTable(L);
    luaL_unref(L, -1, ref);
    thiz->m_listListener.GetData(cookie) = LUA_NOREF;
    lua_pop(L, 1);
    return 0;
}

int LuaObject::RemoveAllListener(lua_State *L)
{
    LuaObject *thiz = CheckLuaObject<LuaObject>(L, 1);
    GetWeakTable(L); // [week]
    auto node = thiz->m_listListener.GetHead();
    while (node) {
        luaL_unref(L, -1, thiz->m_listListener.GetData(node));
        thiz->m_listListener.GetData(node) = LUA_NOREF;
        node = thiz->m_listListener.Next(node);
    }
    lua_pop(L, 1);
    return 0;
}

void LuaObject::CallOnEvent(lua_State *L, int listener, const char *name, int nargs)
{
    LuaStackCheck chk(L);
    // [nargs...] [week]
    lua_rawgeti(L, -1, listener); // [nargs...] [week] [listener]
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1); // [nargs...] [week]
        return; // maybe the listener was freed by GC
    }
    lua_getfield(L, -1, "OnEvent");  // [nargs...] [week] [listener] [OnEvent]
    if (lua_isfunction(L, -1))
    {
        lua_pushvalue(L, -2); // [nargs...] [week] [listener] [OnEvent] [listener]
        this->PushToLua(L, this->TypeNameInstance().c_str()); // [nargs...] [week] [listener] [OnEvent] [listener] [sender]
        lua_pushstring(L, name); // [nargs...] [week] [listener] [OnEvent] [listener] [sender] [name]
        for (int i = 0; i < nargs; i ++) {
            lua_pushvalue(L, -6 - nargs + i);
        } // [nargs...] [week] [listener] [OnEvent] [listener] [sender] [name] [nargs...]
        LuaPCall(L, nargs + 3, 0); // [nargs...] [week] [listener]
        lua_pop(L, 1); // [nargs...] [week]
    }
    else
    {
        lua_pop(L, 2); // [nargs...] [week]
    }
}

bool LuaObject::LuaDispatchEvent( lua_State *L, const char *name, int nargs, int nresult )
{
    LuaStackCheck chk(L);
    if (m_listListener.IsEmpty()) {
        return false;
	}
    GetWeakTable(L); // [nargs...] [week]
    m_listListener.ForEach([=](int ref){
        CallOnEvent(L, ref, name, nargs);
    });
    lua_pop(L, nargs + 1);
    chk.SetReturn(-nargs);
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

} // namespace ltk
