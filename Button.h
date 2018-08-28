#pragma once

#include "Delegate.h"
#include "Sprite.h"

namespace ltk {

class Label;

class Button : public Sprite
{
public:
    RTTI_DECLARATIONS(Button, Sprite)

    enum Mode {
        Text, Icon, IconLeft, IconTop
    };

    Button();
    virtual ~Button();

    void SetText(LPCWSTR text);
    Label *GetLabel();

    void SetNormalColor(D2D1_COLOR_F clr);
    void SetHoverColor(D2D1_COLOR_F clr);

    virtual bool OnPaint(PaintEvent *ev) override;
    virtual bool OnMouseEnter(MouseEvent *ev) override;
    virtual bool OnMouseLeave(MouseEvent *ev) override;
    virtual bool OnLBtnDown(MouseEvent *ev) override;
    virtual bool OnLBtnUp(MouseEvent *ev) override;
    virtual bool OnSize(SizeEvent *ev) override;

    virtual void RecreateResouce(ID2D1RenderTarget *target) override;

    Delegate<void()> Clicked;

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);
    static int GetLabel(lua_State *L);

    BEGIN_LUA_METHOD_MAP(Button)
        LUA_CHAIN_METHOD_MAP(Sprite)
        LUA_METHOD_ENTRY(GetLabel)
    END_LUA_METHOD_MAP()
#endif // LTK_DISABLE_LUA

private:
    D2D1_COLOR_F GetColor();

    bool m_bMouseIn = false;
    bool m_bMousePress = false;
    enum AniState { stStoped, stNormal2Hover, stHover2Normal };
    AniState m_state = stStoped;
    int m_aniCounter = 0;
    enum { AniDuration = 200 };
    DWORD m_lastTick = 0;

    ID2D1SolidColorBrush *m_brush = nullptr;
    bool m_bBorder = false;
    D2D1_COLOR_F m_colorBorder;
    D2D1_COLOR_F m_colorNormal;
    D2D1_COLOR_F m_colorHover;
    D2D1_COLOR_F m_colorPressed;
    Label *m_label = nullptr;
};

} // namespace
