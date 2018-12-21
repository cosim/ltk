#pragma once
#include "ltk.h"

/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/


CStringW Utf8ToUtf16(LPCSTR strA, int len = -1);
CStringA Utf16ToUtf8(LPCTSTR strW, int len);
CString LuaCheckWString(lua_State *L, int index);
void LuaPushWString(lua_State *L, LPCTSTR psz, int len  = -1);

bool LuaPCall(lua_State *L, int nargs, int nresults);
void LuaShowStack(lua_State *L);
void LuaGetField(lua_State *L, int idx, const char *name, double &value);
ltk::Margin LuaCheckMargin(lua_State *L, int idx);
D2D1_COLOR_F LuaCheckColor(lua_State *L, int index);
RectF LuaCheckRectF(lua_State *L, int index);
void LuaPushRectF(lua_State *L, const RectF &rc);
int LuaGetI(lua_State *L, int index, lua_Integer i);

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

#define SAFE_RELEASE(p) if (p) { (p)->Release(); (p) = NULL; }

#define SAFE_RELEASE_AND_MAKR(type, p) if (p) { (p)->Release(); (p) = reinterpret_cast<type *>(0xDEADBEEF); }

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#define LTK_ASSERT(expr) if (!(expr)) {\
    CStringW msg; msg.Format(L"Assertion Failed: %s\r\n%s(%d)", L#expr, __FILEW__, __LINE__);\
    ::MessageBoxW(0, msg, 0, 0);__debugbreak();}


#define  LTK_LOG(...) LtkLogImpl(__FILE__, __LINE__, __VA_ARGS__)

void LtkLogInit();
void LtkLogImpl(const char *source, int line, const char *format, ...);
