#include "stdafx.h"
#include "ApiBinding.h"
#include "Common.h"
#include "ltk.h"


static int l_PostQuitMessage(lua_State *L)
{
    int code = (int)luaL_checkinteger(L, 1);
    ::PostQuitMessage(code);
    return 0;
}

static int l_RunMessageLoop(lua_State *L)
{
    lua_gc(L, LUA_GCCOLLECT, 0); // û�����õĻص����ᱻ�ͷ� ������
    LTK_LOG("MessageLoop BEGIN");
    MSG msg;
    BOOL bRet;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            LTK_LOG("MessageLoop ERROR");
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    LTK_LOG("MessageLoop END");
    ::Sleep(2000);
    ltk::LtkUninitialize();
    return 0;
}


static int l_GetUtf16Table(lua_State *L)
{
    const char* psz = luaL_checkstring(L, 1);
    UINT codePage = (UINT)luaL_checkinteger(L, 2);
    int len = ::MultiByteToWideChar(codePage, 0, psz, -1, NULL, 0);
    if (len > 0)
    {
        std::unique_ptr<wchar_t> pwsz;
        pwsz.reset(new wchar_t[len]);
        ::MultiByteToWideChar(codePage, 0, psz, -1, pwsz.get(), len);

        lua_newtable(L);
        for (int i = 0; i < len - 1; i++)
        {
            assert(pwsz.get()[i] != 0);
            lua_pushinteger(L, pwsz.get()[i]);
            lua_rawseti(L, -2, i + 1);
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

static int l_GetLogicalDrivers(lua_State *L)
{
    DWORD drivers = ::GetLogicalDrives();
    int i = 0;
    CStringA strDrive;
    lua_newtable(L);
    int idx = 1;
    while (i < 32)
    {
        if (drivers & 1)
        {
            strDrive.Format("%c:\\", 'A' + i);
            lua_pushinteger(L, idx);
            lua_pushstring(L, strDrive);
            lua_settable(L, -3);
            idx++;
            //CsDebugA("l_get_logical_drivers() %s\r\n", strDrive);
        }
        drivers = drivers >> 1;
        i++;
    }
    return 1;
}

static int l_SafeWriteFile(lua_State *L)
{
    return 0;
}

static int l_PathFileExists(lua_State *L)
{
    CStringW path = LuaCheckWString(L, 1);
    BOOL ret = ::PathFileExists(path);
    lua_pushboolean(L, ret);
    return 1;
}

static int l_Trace(lua_State *L)
{
    CStringW log(L"<LUA> ");
    for (int i = 1; i <= lua_gettop(L); i++)
    {
        switch (lua_type(L, i))
        {
        case LUA_TNUMBER:
            log.AppendFormat(L"%.2f ", lua_tonumber(L, i));
            break;
        case LUA_TSTRING:
            log.AppendFormat(L"%s ", LuaCheckWString(L, i));
            break;
        default:
            log.AppendFormat(L"(%S:%p) ", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
        }
    }
    log.Append(L"\r\n");
    ::OutputDebugStringW(log);
    return 0;
}

static int l_SetCursor(lua_State *L)
{
    int id = (int)luaL_checkinteger(L, 1);
    // IDC_ARROW
    ::SetCursor(::LoadCursor(NULL, reinterpret_cast<LPCTSTR>(id)));
    return 0;
}

static int l_GetModuleFileName(lua_State *L)
{
    int hMod = 0;
    if (lua_gettop(L) > 1)
    {
        hMod = (int)luaL_checkinteger(L, 1);
    }
    wchar_t path[MAX_PATH];
    ::GetModuleFileNameW((HMODULE)hMod, path, MAX_PATH);
    LuaPushWString(L, path);
    //CStringA str = Utf16ToUtf8(path, wcslen(path));
    //lua_pushstring(L, str);
    return 1;
}

static int l_SHGetFolderPath(lua_State *L)
{
    int folder = CSIDL_APPDATA;
    if (lua_gettop(L) > 1)
    {
        folder = (int)luaL_checkinteger(L, 1);
    }
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(::SHGetFolderPath(NULL, folder, NULL, SHGFP_TYPE_CURRENT, path)))
    {
        LuaPushWString(L, path);
        return 1;
    }
    return 0;
}

static int l_GetOpenFileName(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TNUMBER);

    std::vector<wchar_t> filters;
    int i = 1;
    for (;;)
    {
        lua_pushinteger(L, i);
        lua_gettable(L, 1); // t1[i]
        if (lua_type(L, -1) == LUA_TSTRING)
        {
            CStringW str = LuaCheckWString(L, -1);
            for (int j = 0; j < str.GetLength(); j++)
            {
                filters.push_back(str.GetAt(j));
            }
            lua_pop(L, 1);
            filters.push_back(0);
            i++; // TODO ����ǲ��ǻ�Ҫ�������Ƿ�Ϊż����? 
        }
        else
        {
            lua_pop(L, 1); // nil
            break;
        }
    }
    if (filters.size() == 0)
    {
        return 0;
    }
    filters.push_back(0);

    WCHAR buf[MAX_PATH] = { 0 };

    OPENFILENAME ofn;
    ::ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    // �������Ҫ��HWND �������һ����ģ̬�Ի��� ��lua�ű���������
    ofn.hwndOwner = reinterpret_cast<HWND>(lua_tointeger(L, 2));
    ofn.lpstrFile = buf;
    ofn.nMaxFile = ARRAYSIZE(buf);
    ofn.lpstrFilter = filters.data();
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    BOOL ret = ::GetOpenFileName(&ofn);
    if (ret)
    {
        LuaPushWString(L, buf);
        return 1;
    }
    else
    {
        return 0;
    }
}

static int l_GetTickCount(lua_State *L)
{
    auto tick = (long long)::GetTickCount64();
    lua_pushinteger(L, (lua_Integer)tick);
    return 1;
}

static int l_PrintEx(lua_State *L)
{
    CStringA log;
    for (int i = 1; i <= lua_gettop(L); i++)
    {
        switch (lua_type(L, i))
        {
        case LUA_TNUMBER:
            log.AppendFormat("%g ", lua_tonumber(L, i));
            break;
        case LUA_TSTRING:
            log.AppendFormat("%s ", lua_tostring(L, i));
            break;
        case LUA_TNIL:
            log.Append("(nil) ");
            break;
        default:
            log.AppendFormat("(%S:%p) ", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
        }
    }
    lua_Debug ar;
    if (lua_getstack(L, 1, &ar)) {
        lua_getinfo(L, "Sl", &ar);
        log.AppendFormat("\r\n\t%s:%d:", ar.source, ar.currentline);
    }
    log.Append("\r\n");
    fwrite((LPCSTR)log, 1, log.GetLength(), stdout);
    fflush(stdout);
    return 0;
}

static const struct luaL_Reg cfunctions[] = {
    { "RunMessageLoop", l_RunMessageLoop },
    { "PostQuitMessage", l_PostQuitMessage },
    { "GetUtf16Table", l_GetUtf16Table },
    { "GetLogicalDrivers", l_GetLogicalDrivers },
    { "PathFileExists", l_PathFileExists },
    { "SafeWriteFile", l_SafeWriteFile },
    { "Trace", l_Trace },
    { "SetCursor", l_SetCursor },
    { "GetModuleFileName", l_GetModuleFileName },
    { "SHGetFolderPath", l_SHGetFolderPath },
    { "GetTickCount", l_GetTickCount },
    { "PrintEx", l_PrintEx },
    { NULL, NULL }
};

namespace ltk {

void ApiBindingInit(lua_State *L)
{
    luaL_register(L, "LtkApi", cfunctions);
}

} // namespace ltk