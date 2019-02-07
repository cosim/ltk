#include "stdafx.h"
#include "StyleManager.h"
#include "ltk.h"
#include "Window.h"

namespace ltk {

StyleManager * StyleManager::m_instance = nullptr;

StyleManager::StyleManager()
{
    for (int i = 0; i < clrLast; i ++) {
        m_colors.push_back(D2D1::ColorF(D2D1::ColorF::Cyan));
    }
}


StyleManager::~StyleManager()
{
    for (auto bg : m_mapBackgroundStyle) {
        bg.second->Release();
    }
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

ltk::AbstractBackground * StyleManager::GetBackgroundStyle(const char *name) const
{
    std::string strName(name);
    auto iter = m_mapBackgroundStyle.find(strName);
    if (iter != m_mapBackgroundStyle.end()) {
        iter->second->AddRef();
        return iter->second;
    }
    else {
        return nullptr;
    }
}

bool StyleManager::AddBackgroundStyle(const char *name, AbstractBackground *bg)
{
    std::string strName(name);
    auto iter = m_mapBackgroundStyle.find(strName);
    if (iter == m_mapBackgroundStyle.end()) {
        m_mapBackgroundStyle[strName] = bg;
        bg->AddRef();
        return true;
    }
    else {
        return false;
    }
}

TextureInfo StyleManager::CheckTextureInfo(lua_State *L, int idx)
{
    TextureInfo info;

    if (!lua_istable(L, idx)) goto Error;
    lua_getfield(L, -1, "atlas"); // [...][atlas]
    if (!lua_istable(L, -1)) goto Error;
    info.atlas = LuaCheckRectF(L, -1);
    lua_pop(L, 1); // [...]

    lua_getfield(L, -1, "margin"); // [...][margin]
    if (!lua_istable(L, -1)) goto Error;
    info.margin = LuaCheckMargin(L, -1);
    lua_pop(L, 1); // [...]

    LuaGetField(L, idx, "scale", info.scale);
    return info;
Error:
    luaL_error(L, "TextureInfo format error.");
    return info;
}

int StyleManager::RegisterNinePatchStyle(lua_State *L)
{
    LuaStackCheck chk(L);
    StyleManager *thiz = Instance();
    auto name = luaL_checkstring(L, 2);
    luaL_checktype(L, 3, LUA_TTABLE);

    lua_getfield(L, 3, "normal"); // [][][style][normal]
    TextureInfo normal = CheckTextureInfo(L, -1);
    lua_pop(L, 1); // [][][style]

    TextureInfo hover;
    lua_getfield(L, 3, "hover"); // [][][style][hover]
    if (!lua_istable(L, -1)) {
        hover = normal;
    }
    else {
        hover = CheckTextureInfo(L, -1);
    }
    lua_pop(L, 1); // [][][style]

    TextureInfo pressed;
    lua_getfield(L, 3, "pressed"); // [][][style][pressed]
    if (!lua_istable(L, -1)) {
        pressed = normal;
    }
    else {
        pressed = CheckTextureInfo(L, -1);
    }
    lua_pop(L, 1); // [][][style]

    TextureInfo disable;
    lua_getfield(L, 3, "disable"); // [][][style][disable]
    if (!lua_istable(L, -1)) {
        disable = normal;
    }
    else {
        disable = CheckTextureInfo(L, -1);
    }
    lua_pop(L, 1); // [][][style]

    auto bg = new NinePatchBackground();
    bg->texNormal = normal;
    bg->texHover = hover;
    bg->texPressed = pressed;
    bg->texDisable = disable;
    thiz->AddBackgroundStyle(name, bg);
    bg->Release();

    return 0;
}

int StyleManager::RegisterOnePatchStyle(lua_State *L)
{
    LuaStackCheck chk(L);
    StyleManager *thiz = Instance();
    const char *name = luaL_checkstring(L, 2);
    luaL_checktype(L, 3, LUA_TTABLE);

    IconInfo normal;
    IconInfo hover;
    IconInfo pressed;
    IconInfo disable;

    lua_getfield(L, 3, "normal"); // [][][style][normal]
    normal.atlas = LuaCheckRectF(L, -1);
    lua_pop(L, 1); // [][][style]

    lua_getfield(L, 3, "hover"); // [][][style][hover]
    if (!lua_istable(L, -1)) {
        hover = normal;
    }
    else {
        hover.atlas = LuaCheckRectF(L, -1);
    }
    lua_pop(L, 1); // [][][style]

    lua_getfield(L, 3, "pressed"); // [][][style][pressed]
    if (!lua_istable(L, -1)) {
        pressed = normal;
    }
    else {
        pressed.atlas = LuaCheckRectF(L, -1);
    }
    lua_pop(L, 1); // [][][style]

    lua_getfield(L, 3, "disable"); // [][][style][disable]
    if (!lua_istable(L, -1)) {
        disable = normal;
    }
    else {
        disable.atlas = LuaCheckRectF(L, -1);
    }
    lua_pop(L, 1); // [][][style]

    auto bg = new OnePatchBackground();
    bg->iconNormal = normal;
    bg->iconHover = hover;
    bg->iconPressed = pressed;
    bg->iconDisable = disable;
    thiz->AddBackgroundStyle(name, bg);
    bg->Release();

    return 0;
}

int StyleManager::LuaConstructor(lua_State *L)
{
    luaL_error(L, "StyleManager is singleton.");
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

void NinePatchBackground::Draw(Window *wnd, ID2D1RenderTarget *targe, const RectF &rc, State state, float blend)
{
    auto bmp = wnd->GetAtlasBitmap();
    TextureInfo *tex = nullptr;

    switch (state) {
    case Normal:
        tex = &texNormal;
        break;;
    case Hover:
        tex = &texHover;
        break;
    case Normal2Hover:
    case Hover2Normal:
        DrawTextureNineInOne(targe, bmp, texNormal.atlas, texNormal.margin, rc, 1.0f - blend, texNormal.scale);
        DrawTextureNineInOne(targe, bmp, texHover.atlas, texHover.margin, rc, blend, texHover.scale);
        return;
    case Pressed:
        tex = &texPressed;
        break;
    case Disable:
        tex = &texDisable;
        break;
    }
    DrawTextureNineInOne(targe, bmp, tex->atlas, tex->margin, rc, blend, tex->scale);
}

void OnePatchBackground::Draw(Window *wnd, ID2D1RenderTarget *targe, const RectF &rc, State state, float blend)
{
    auto bmp = wnd->GetAtlasBitmap();
    IconInfo *icon = nullptr;

    switch (state) {
    case Normal:
        icon = &iconNormal;
        break;
    case Hover:
        icon = &iconHover;
        break;
    case Normal2Hover:
    case Hover2Normal:
        targe->DrawBitmap(bmp, D2D1RectF(rc), 1.0f - blend,
            D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
            D2D1RectF(iconNormal.atlas));
        targe->DrawBitmap(bmp, D2D1RectF(rc), blend,
            D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
            D2D1RectF(iconHover.atlas));
        return;
    case Pressed:
        icon = &iconPressed;
        break;
    case Disable:
        icon = &iconDisable;
        break;
    }
    targe->DrawBitmap(bmp, D2D1RectF(rc), 1.0f,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
        D2D1RectF(icon->atlas));
}

}
