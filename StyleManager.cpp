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
    RELEASE_AND_INVALIDATE(ID2D1Bitmap, m_bitmap);
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

ID2D1Bitmap *StyleManager::GetBitmap(UINT idx)
{
    return m_bitmap;
}

void StyleManager::RecreateResource(ID2D1RenderTarget *target)
{
    // TODO FIXME m_bitmap should be per Window data. because of differnet target
    SAFE_RELEASE(m_bitmap);
    LTK_LOG("creating atlas bitmap");
    HRESULT hr = LoadBitmapFromFile(target, L"E:\\myworks\\ltk\\res\\atlas.png", &m_bitmap);
    LTK_ASSERT(SUCCEEDED(hr));
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
    size_t size = lua_objlen(L, 2);
    if (size < clrLast) {
        luaL_error(L, "not enough colors");
    }
    thiz->m_colors.clear();
    int i = 1;
    LuaGetI(L, 2, i);
    while (lua_isstring(L, -1)) {
        const char *psz = lua_tostring(L, -1);
        thiz->m_colors.push_back(ColorFromString(psz));
        i++;
        lua_pop(L, 1); // for the color string
        LuaGetI(L, 2, i);
    }
    lua_pop(L, 1); // for nil
    return 0;
}

StyleManager * StyleManager::m_instance = nullptr;

}
