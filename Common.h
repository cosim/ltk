#pragma once

/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/


CStringW Utf8ToUtf16(LPCSTR strA, int len = -1);
CStringA Utf16ToUtf8(LPCTSTR strW, int len);
CString LuaCheckWString(lua_State *L, int index);
void LuaPushWString(lua_State *L, LPCTSTR psz, int len  = -1);

bool LuaPCall(lua_State *L, int nargs, int nresults);
void LuaShowStack(lua_State *L);
D2D1_COLOR_F LuaCheckColor(lua_State *L, int index);
RectF LuaCheckRectF(lua_State *L, int index);
void LuaPushRectF(lua_State *L, const RectF &rc);

#define LOG(msg) do\
{\
	std::stringstream ss;\
	ss << __FUNCTION__ << "() " msg << std::endl;\
	OutputDebugStringA(ss.str().c_str());\
} while (0)

#define LOGW(msg) do\
{\
	std::wstringstream ss;\
	ss << __FUNCTIONW__ << L"() " msg << std::endl;\
	OutputDebugString(ss.str().c_str());\
} while (0)

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);             \
	void operator=(const TypeName&)

// https://stackoverflow.com/questions/3060006/is-it-worth-setting-pointers-to-null-in-a-destructor
#ifndef INVALID_POINTER
#define INVALID_POINTER(type) reinterpret_cast<type *>(0xDEADBEEF)
#endif

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

