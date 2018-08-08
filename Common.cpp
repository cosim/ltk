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

