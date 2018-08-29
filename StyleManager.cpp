#include "stdafx.h"
#include "StyleManager.h"
#include "ltk.h"

namespace ltk {

StyleManager::StyleManager()
{
    for (int i = 0; i < clrLast; i ++) {
        m_colors.push_back(D2D1::ColorF(D2D1::ColorF::Cyan));
    }
}


StyleManager::~StyleManager()
{
    if (m_bitmap) {
        m_bitmap->Release();
    }
    m_bitmap = INVALID_POINTER(ID2D1Bitmap);
}

StyleManager * StyleManager::Instance()
{
    if (!m_instance) {
        m_instance = new StyleManager;
    }
    return m_instance;
}

void StyleManager::Free()
{
    delete m_instance;
}

D2D1_COLOR_F StyleManager::GetColor(Colors clr)
{
    return m_colors.at((size_t)clr);
}

ID2D1Bitmap *StyleManager::GetBitmap(ID2D1RenderTarget *target, UINT idx)
{
    if (!m_bitmap) {
        HRESULT hr = LoadBitmapFromFile(target, L"E:\\myworks\\ltk\\res\\atlas.png", &m_bitmap);
        LTK_ASSERT(SUCCEEDED(hr));
    }
    return m_bitmap;
}

D2D1_COLOR_F StyleManager::ColorFromString(const char *psz)
{
    if (strlen(psz) != 7) {
        return D2D1::ColorF(D2D1::ColorF::Cyan);
    }
    if (psz[0] != '#') {
        return D2D1::ColorF(D2D1::ColorF::Cyan);
    }
    long bin = strtol(psz + 1, NULL, 16);
    D2D1_COLOR_F clr;
    clr.a = 1.0f;
    clr.b = (bin & 0xFF) / 256.0f;
    bin >>= 8;
    clr.g = (bin & 0xFF) / 256.0f;
    bin >>= 8;
    clr.r = (bin & 0xFF) / 256.0f;
    bin >>= 8;
    return clr;
}

int StyleManager::LuaConstructor(lua_State *L)
{
    return 0;
}

int StyleManager::SetColorScheme(lua_State *L)
{
    StyleManager *thiz = Instance();
    luaL_checktype(L, 2, LUA_TTABLE);
    lua_len(L, 2);
    size_t size = (size_t)lua_tointeger(L, -1);
    if (size < clrLast) {
        luaL_error(L, "not enough colors");
    }
    lua_pop(L, 1); // for length
    thiz->m_colors.clear();
    int i = 1;
    lua_geti(L, 2, i);
    while (lua_isstring(L, -1)) {
        const char *psz = lua_tostring(L, -1);
        thiz->m_colors.push_back(ColorFromString(psz));
        i++;
        lua_pop(L, 1); // for the color string
        lua_geti(L, 2, i);
    }
    lua_pop(L, 1); // for nil
    return 0;
}

StyleManager * StyleManager::m_instance = nullptr;

}
