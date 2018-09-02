/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/

#include "stdafx.h"
#include "Common.h"
#include "UniConversion.h"

CStringW Utf8ToUtf16(LPCSTR strA, int len)
{
	if(len < 0)
	{
		len = (int)strlen(strA);
	}
	CStringW strW;
	int lenW = UTF16Length(strA, len);
	wchar_t *pbuff = strW.GetBuffer(lenW + 1);
	int ret = UTF16FromUTF8(strA, len, pbuff, lenW);
	strW.ReleaseBuffer(ret);
	return strW;
}

CStringA Utf16ToUtf8(LPCTSTR strW, int len)
{
    if (len < 0) {
        len = (int)wcslen(strW);
    }
	CStringA strA;
	int lenA = UTF8Length(strW, len);
	char *pbuff = strA.GetBuffer(lenA + 1);
	UTF8FromUTF16(strW, len, pbuff, lenA);
	strA.ReleaseBuffer(lenA);
	return strA;
}

// http://stackoverflow.com/questions/12256455/print-stacktrace-from-c-code-with-embedded-lua
int LuaTraceBack(lua_State *L) {
    if (!lua_isstring(L, 1))  /* 'message' not a string? */
        return 1;  /* keep it intact */
    // lua_getfield(L, LUA_GLOBALSINDEX, "debug");
    lua_getglobal(L, "debug");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 1;
    }
    lua_getfield(L, -1, "traceback");
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return 1;
    }
    lua_pushvalue(L, 1);  /* pass error message */
    lua_pushinteger(L, 2);  /* skip this function and traceback */
    lua_call(L, 2, 1);  /* call debug.traceback */
    return 1;
}

bool LuaPCall(lua_State *L, int nargs, int nresults)
{
    lua_pushcfunction(L, LuaTraceBack);
    lua_insert(L, lua_gettop(L) - nargs - 1);
    int error = lua_pcall(L, nargs, nresults, lua_gettop(L) - nargs - 1);
    if (error == 0) {
        //lua_pop(L, 1);
        lua_remove(L, lua_gettop(L) - nresults);
    }
    else {
        assert(lua_isstring(L, -1));
        const char *pszError = lua_tostring(L, -1);//luaL_checkstring(L, -1);
        int msg = lua_gettop(L); // msg
        // 3�ִ��������ѡ�� ѡ�Ǹ�?
        lua_getglobal(L, "print"); // msg, print
        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, msg); // msg, print, msg
            lua_call(L, 1, 0); // msg
        }
        //LuaLog(pszError);
#ifdef _DEBUG
        ::PostQuitMessage(1); // ��ô�ļ򵥴ֱ� ����ȥ�س����� ֱ�ӹҵ�
#else
        // ����ĸ��ںв���
        // TODO ҪŪ������ ��ֹ�����浯��
        CStringW strError = Utf8ToUtf16(pszError);
        ::MessageBox(NULL, strError, NULL, 0);
#endif
        lua_pop(L, 2); // stack trace string and the LuaTraceBack function
    }
    //LuaCheckStack(L);
    return error == 0;
}


CString LuaCheckWString(lua_State *L, int index)
{
    size_t len;
    const char *pText = luaL_checklstring(L, index, &len);
    char ivc;
    if (GetInvalidUtf8SymbolPosition(pText, ivc) == -1)
        return Utf8ToUtf16(pText, (int)len);
    else
        return L"invalide utf8 string";
}

void LuaPushWString(lua_State *L, LPCTSTR psz, int len /* = -1 */)
{
    if (len < 0)
    {
        len = (int)wcslen(psz);
    }
    CStringA strA = Utf16ToUtf8(psz, len);
    lua_pushlstring(L, strA, strA.GetLength());
}

D2D1_COLOR_F LuaCheckColor(lua_State *L, int index)
{
    D2D1_COLOR_F color;
    color.r = (float)luaL_checknumber(L, index + 0);
    color.g = (float)luaL_checknumber(L, index + 1);
    color.b = (float)luaL_checknumber(L, index + 2);
    color.a = 1.0f;
    return color;
}

void LuaShowStack(lua_State *L)
{
    const int COUNT = 10;
    CStringA msg;
    msg.AppendFormat("(top:%d) ", lua_gettop(L));
    for (int i = lua_gettop(L); i >= 1 && i >= lua_gettop(L) - COUNT; i--) {
        switch (lua_type(L, i)) {
        case LUA_TSTRING:
            msg.AppendFormat("(%s:%s) ", lua_typename(L, lua_type(L, i)), lua_tostring(L, i));
            break;
        case LUA_TNUMBER:
            msg.AppendFormat("(%s:%f) ", lua_typename(L, lua_type(L, i)), lua_tonumber(L, i));
            break;
        default:
            msg.AppendFormat("(%s:%p) ", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
        }
    }
    OutputDebugStringA(msg);
}

void LuaGetField(lua_State *L, int idx, const char *name, float &value)
{
    lua_getfield(L, idx, name);
    if (!lua_isnumber(L, -1)) {
        lua_pop(L, 1); 
        luaL_error(L, "#%d[\"%s\"] is not a number.", idx, name);
        return;
    }
    value = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);
}

ltk::Margin LuaCheckMargin(lua_State *L, int idx)
{
    ltk::Margin margin;
    luaL_checktype(L, idx, LUA_TTABLE);
    LuaGetField(L, idx, "left", margin.left);
    LuaGetField(L, idx, "top", margin.top);
    LuaGetField(L, idx, "right", margin.right);
    LuaGetField(L, idx, "bottom", margin.bottom);
    return margin;
}

RectF LuaCheckRectF(lua_State *L, int idx)
{
    RectF rc;
    luaL_checktype(L, idx, LUA_TTABLE);
    LuaGetField(L, idx, "x", rc.X);
    LuaGetField(L, idx, "y", rc.Y);
    LuaGetField(L, idx, "w", rc.Width);
    LuaGetField(L, idx, "h", rc.Height);
    return rc;
}

void LuaPushRectF(lua_State *L, const RectF &rc)
{
    lua_newtable(L);
    int tRect = lua_gettop(L);
    lua_pushnumber(L, rc.X);
    lua_setfield(L, tRect, "x");
    lua_pushnumber(L, rc.Y);
    lua_setfield(L, tRect, "y");
    lua_pushnumber(L, rc.Width);
    lua_setfield(L, tRect, "w");
    lua_pushnumber(L, rc.Height);
    lua_setfield(L, tRect, "h");
}

