#pragma once

#include "LuaObject.h"

namespace ltk {

class Window;
class AbstractBackgroundPainter;

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

    static int LuaConstructor(lua_State *L);
    static int SetColorScheme(lua_State *L);

    BEGIN_LUA_METHOD_MAP(StyleManager)
        LUA_METHOD_ENTRY(SetColorScheme)
    END_LUA_METHOD_MAP()

private:
    StyleManager();
    ~StyleManager();
    static StyleManager *m_instance;

    std::vector<D2D1_COLOR_F> m_colors;
    std::unordered_map<std::string, AbstractBackgroundPainter *> m_mapBackgroundStyle;
};

class AbstractBackgroundPainter : public LuaObject
{
public:
    virtual void Paint(Window *wnd, ID2D1RenderTarget *targe, const RectF &rc, UINT state, float blend) = 0;
};

struct FourStateColor {
    D2D1_COLOR_F clrNormal;
    D2D1_COLOR_F clrHover;
    D2D1_COLOR_F clrPressed;
    D2D1_COLOR_F clrDisable;
};

class VectorBackgroundPainter : public AbstractBackgroundPainter
{
public:
    virtual void Paint(Window *wnd, ID2D1RenderTarget *targe, const RectF &rc, UINT state, float blend) override;

    FourStateColor borderColors;
    FourStateColor backgroundColors;
    // TODO add Gradient
    bool hasBorder = true;
    float roundCorner = 0.0f;
};

class BitmapBackgroundPainter : public AbstractBackgroundPainter
{

public:
    virtual void Paint(Window *wnd, ID2D1RenderTarget *targe, const RectF &rc, UINT state, float blend) override;

    TextureInfo texNormal;
    TextureInfo texHover;
    TextureInfo texPressed;
    TextureInfo texDisable;
};


}
