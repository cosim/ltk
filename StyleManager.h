#pragma once

#include "LuaObject.h"

namespace ltk {

class Window;

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

    ID2D1Bitmap *GetBitmap(UINT idx = 0);

    ID2D1SolidColorBrush *GetStockBrush();

    void RecreateResource(ID2D1RenderTarget *target);

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
    ID2D1Bitmap *m_bitmap = nullptr;
};

class IbackgroundPainter
{
public:
    virtual void Paint(Window *wnd, ID2D1RenderTarget *targe, const RectF &rc, UINT state, float ratio) = 0;
    virtual bool LoadFromXML(tinyxml2::XMLNode *node) = 0;
};

class VectorBackgroundPainter : public IbackgroundPainter
{
public:

    virtual void Paint(Window *wnd, ID2D1RenderTarget *targe, const RectF &rc, UINT state, float ratio) override;
    bool LoadFromXML(tinyxml2::XMLNode *node) override;

    D2D1_COLOR_F clrBorderNormal;
    D2D1_COLOR_F clrBorderHover;
    D2D1_COLOR_F clrBorderPressed;
    D2D1_COLOR_F clrBorderDisable;

    D2D1_COLOR_F clrBackgroundNormal;
    D2D1_COLOR_F clrBackgroundHover;
    D2D1_COLOR_F clrBackgroundPressed;
    D2D1_COLOR_F clrBackgroundDisable;

    bool hasBorder = true;
    float roundCorner = 0.0f;
};

class BitmapBackgroundPainter : public IbackgroundPainter
{

public:
    virtual void Paint(Window *wnd, ID2D1RenderTarget *targe, const RectF &rc, UINT state, float ratio) override;
    bool LoadFromXML(tinyxml2::XMLNode *node) override;

    std::unique_ptr<TextureInfo> texNormal;
    std::unique_ptr<TextureInfo> texHover;
    std::unique_ptr<TextureInfo> texPressed;
    std::unique_ptr<TextureInfo> texDisable;
};

}
