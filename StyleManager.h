#pragma once

#include "LuaObject.h"

namespace ltk {

class Window;
class AbstractBackground;

class StyleManager : public LuaObject
{
public:
    RTTI_DECLARATIONS(StyleManager, LuaObject);
    static StyleManager *Instance();
    static void Free();

    enum Colors {
        clrBackground,
        clrNormal,
        clrHover,
        clrText,
        clrHighlight,
        clrBorder,
        clrCaption,
        clrLast
    };
    D2D1_COLOR_F GetColor(Colors clr);

    static D2D1_COLOR_F ColorFromString(const char *psz);

    AbstractBackground *GetBackgroundStyle(const char *name) const;
    bool AddBackgroundStyle(const char *name, AbstractBackground *); // bg RefCount +1

    static int LuaConstructor(lua_State *L);
    static int SetColorScheme(lua_State *L);
    static int RegisterNinePatchStyle(lua_State *L);
    static int RegisterOnePatchStyle(lua_State *L);

    BEGIN_LUA_METHOD_MAP(StyleManager)
        LUA_METHOD_ENTRY(SetColorScheme)
        LUA_METHOD_ENTRY(RegisterNinePatchStyle)
    END_LUA_METHOD_MAP()

    static TextureInfo CheckTextureInfo(lua_State *L, int idx);

private:
    StyleManager();
    ~StyleManager();
    static StyleManager *m_instance;

    std::vector<D2D1_COLOR_F> m_colors;
    std::unordered_map<std::string, AbstractBackground *> m_mapBackgroundStyle;
};

class AbstractBackground : public LuaObject
{
public:
    enum State {Normal, Hover, Normal2Hover, Hover2Normal, Pressed, Disable};
    virtual void Draw(Window *wnd, ID2D1RenderTarget *targe, const RectF &rc,
        State state, float blend) = 0;
};

class NinePatchBackground : public AbstractBackground
{

public:
    virtual void Draw(Window *wnd, ID2D1RenderTarget *targe, const RectF &rc,
        State state, float blend) override;

    TextureInfo texNormal;
    TextureInfo texHover;
    TextureInfo texPressed;
    TextureInfo texDisable;
};

class OnePatchBackground : public AbstractBackground
{
public:
    virtual void Draw(Window *wnd, ID2D1RenderTarget *targe, const RectF &rc,
        State state, float blend) override;
    
    IconInfo iconNormal;
    IconInfo iconHover;
    IconInfo iconPressed;
    IconInfo iconDisable;
};


struct FourStateColor {
    D2D1_COLOR_F clrNormal;
    D2D1_COLOR_F clrHover;
    D2D1_COLOR_F clrPressed;
    D2D1_COLOR_F clrDisable;
};

class VectorBackground : public AbstractBackground
{
public:
    virtual void Draw(Window *wnd, ID2D1RenderTarget *targe, const RectF &rc,
        State state, float blend) override;

    FourStateColor borderColors;
    FourStateColor backgroundColors;
    // TODO add Gradient
    bool hasBorder = true;
    float roundCorner = 0.0f;
};

}
