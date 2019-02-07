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

CStringA Utf16ToGbk(LPCTSTR strW, int len)
{
    if (len < 0) {
        len = (int)wcslen(strW);
    }
    int lenA = ::WideCharToMultiByte(936, 0, strW, len, NULL, 0, NULL, NULL);
    CStringA strA;
    char *pbuff = strA.GetBuffer(lenA);
    ::WideCharToMultiByte(936, 0, strW, len, pbuff, lenA, NULL, NULL);
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
        // 3种错误输出的选择 选那个?
        lua_getglobal(L, "print"); // msg, print
        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, msg); // msg, print, msg
            lua_call(L, 1, 0); // msg
        }
        //LuaLog(pszError);
#ifndef NDEBUG
        //::PostQuitMessage(1); // 多么的简单粗暴 不用去关程序了 直接挂掉
        ::TerminateProcess(::GetCurrentProcess(), 1);
#else
        // 弹框的给黑盒测试
        // TODO 要弄个开关 防止发布版弹框
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
    // TODO avoid lua longjmp()
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
    msg.Append("\r\n");
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

ltk::IconInfo LuaCheckIconInfo(lua_State *L, int idx)
{
    ltk::IconInfo info;
    luaL_checktype(L, idx, LUA_TTABLE);
    lua_getfield(L, idx, "atlas");
    info.atlas = LuaCheckRectF(L, -1);
    lua_pop(L, 1);
    LuaGetField(L, idx, "scale", info.scale);
    return info;
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

static int lua_absindex(lua_State *L, int i) {
    if (i < 0 && i > LUA_REGISTRYINDEX)
        i += lua_gettop(L) + 1;
    return i;
}

int LuaGetI(lua_State *L, int index, lua_Integer i) {
    index = lua_absindex(L, index);
    lua_pushinteger(L, i);
    lua_gettable(L, index);
    return lua_type(L, -1);
}

static bool g_ltk_log_to_ods = true;
static bool g_ltk_log_to_file = true;
static HANDLE g_ltk_log_file_handle = 0;

static const wchar_t *LOG_DIR = L"C:\\ZLOG";
static const int MAX_LOG_NUM = 15;

static void CreateLogFile()
{
    if (g_ltk_log_file_handle) {
        return;
    }
    if (!::PathFileExists(LOG_DIR)) {
        ::CreateDirectory(LOG_DIR, NULL);
    }
    wchar_t buffer[64];
    tm tm_;
    time_t time_ = time(NULL);
    localtime_s(&tm_, &time_);
    wcsftime(buffer, ARRAYSIZE(buffer), L"%Y%m%d-%H%M%S", &tm_);
    std::wstring path = LOG_DIR;
    path += L"\\";
    path += buffer;
    path += L".log";
    g_ltk_log_file_handle = ::CreateFile(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == g_ltk_log_file_handle) {
        LTK_ASSERT(false);
    }
}

static bool Compare(const WIN32_FIND_DATA &lhs, const WIN32_FIND_DATA &rhs)
{
    return ::CompareFileTime(&lhs.ftLastWriteTime, &rhs.ftLastWriteTime) < 0;
}

static void DeleteOldLog()
{
    std::vector<WIN32_FIND_DATA> vecFiles;
    WIN32_FIND_DATA wfd;
    std::wstring path = LOG_DIR;
    path += L"\\*.log";
    HANDLE hFind = ::FindFirstFile(path.c_str(), &wfd);
    if (hFind != INVALID_HANDLE_VALUE) {
        vecFiles.push_back(wfd);
        while (::FindNextFile(hFind, &wfd))
        {
            vecFiles.push_back(wfd);
        }
        ::FindClose(hFind);
    }
    if (vecFiles.size() > MAX_LOG_NUM) {
        std::sort(vecFiles.begin(), vecFiles.end(), Compare);
        for (size_t i = 0; i < vecFiles.size() && i <= vecFiles.size() - MAX_LOG_NUM; ++i) {
            std::wstring file = LOG_DIR;
            file += L"\\";
            file += vecFiles.at(i).cFileName;
            ::DeleteFile(file.c_str());
        }
    }
}

void LtkLogInit()
{
    if (::PathFileExists(LOG_DIR)) {
        DeleteOldLog();
        CreateLogFile();
    }
}

void LtkLogImpl(const char *source, int line, const char *format, ...)
{
    const size_t SIZE = 1024 * 2;
    char buffer[SIZE];
    va_list varg;
    va_start(varg, format);
    ::StringCbVPrintfA(buffer, SIZE, format, varg);
    va_end(varg);
    char buffer2[SIZE];
    ::StringCbPrintfA(buffer2, SIZE, "%s\t%s(%d)\r\n", buffer, source, line);

    if (g_ltk_log_to_ods) {
        ::OutputDebugStringA(buffer2);
    }
    if (g_ltk_log_to_file) {
        size_t len = strlen(buffer2);
        DWORD out_len = 0;
        ::WriteFile(g_ltk_log_file_handle, buffer2, (DWORD)len, &out_len, NULL);
    }
}

