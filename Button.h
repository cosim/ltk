#pragma once

#include "Delegate.h"
#include "Sprite.h"

namespace ltk {

class Label;

struct IconInfo {
    UINT idx = 0;
    RectF atlas;
    bool bIconOnTop = false;
    float scale = 1.0f;
};

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

    void SetIcon(const RectF &rc, float scale, bool iconOnTop = false, UINT idx = 0);

    virtual bool OnPaint(PaintEvent *ev) override;

    void Update();

    virtual bool OnMouseEnter(MouseEvent *ev) override;
    virtual bool OnMouseLeave(MouseEvent *ev) override;
    virtual bool OnLBtnDown(MouseEvent *ev) override;
    virtual bool OnLBtnUp(MouseEvent *ev) override;
    virtual bool OnSize(SizeEvent *ev) override;

    virtual void RecreateResouce(ID2D1RenderTarget *target) override;

    Delegate<void()> Clicked;

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);
    static int SetText(lua_State *L);
    static int GetLabel(lua_State *L);
    static int SetIcon(lua_State *L);

    BEGIN_LUA_METHOD_MAP(Button)
        LUA_CHAIN_METHOD_MAP(Sprite)
        LUA_METHOD_ENTRY(SetText)
        LUA_METHOD_ENTRY(GetLabel)
        LUA_METHOD_ENTRY(SetIcon)
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
    IconInfo *m_icon = nullptr;
};

} // namespace
