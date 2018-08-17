#pragma once

#include "Sprite.h"
#include "Delegate.h"

namespace ltk {

class Button : public Sprite
{
public:
    RTTI_DECLARATIONS(Button, Sprite)
    Button();
    virtual ~Button();

    virtual bool OnPaint(PaintEvent *ev) override;

    virtual bool OnMouseEnter(MouseEvent *ev) override;
    virtual bool OnMouseLeave(MouseEvent *ev) override;

    void SetText(LPCWSTR text);

    virtual bool OnLBtnDown(MouseEvent *ev) override;
    virtual bool OnLBtnUp(MouseEvent *ev) override;

    virtual void RecreateResouce(ID2D1RenderTarget *target) override;

    Delegate<void()> Clicked;

    D2D1_COLOR_F GetColor();

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);

    BEGIN_LUA_METHOD_MAP(Button)
        LUA_CHAIN_METHOD_MAP(Sprite)
    END_LUA_METHOD_MAP()

#endif // LTK_DISABLE_LUA

private:
    bool m_bMouseIn = false;
    bool m_bMousePress = false;
    enum AniState { stStoped, stNormal2Hover, stHover2Normal };
    AniState m_state = stStoped;
    int m_aniCounter = 0;
    enum { AniDuration = 200 };
    DWORD m_lastTick = 0;

    wstring m_strText;
    IDWriteTextFormat *m_text_format = nullptr;
    ID2D1SolidColorBrush *m_brush = nullptr;
    bool m_bBorder = false;
    D2D1_COLOR_F m_colorText;
    D2D1_COLOR_F m_colorBorder;
    D2D1_COLOR_F m_colorNormal;
    D2D1_COLOR_F m_colorHover;
};

} // namespace
