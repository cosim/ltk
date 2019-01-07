#pragma once

#include "Delegate.h"
#include "BoxLayout.h"

namespace ltk {

class Label;
class ImageSprite;
struct IconInfo;

class Button : public BoxLayout
{
public:
    RTTI_DECLARATIONS(Button, BoxLayout)

    enum Mode {
        Text, Icon, IconLeft, IconTop
    };

    Button();
    virtual ~Button();

    void SetText(LPCWSTR text);
    Label *GetLabel();
    void EnableCapture(bool);

    void SetNormalColor(D2D1_COLOR_F clr);
    void SetHoverColor(D2D1_COLOR_F clr);

    void SetIcon(const RectF &rc, float scale, UINT idx = 0);

    virtual bool OnPaint(PaintEvent *ev) override;

    void Update();

    virtual bool OnEvent(Event *ev) override;

    virtual bool OnMouseEnter(MouseEvent *ev) override;
    virtual bool OnMouseLeave(MouseEvent *ev) override;
    virtual bool OnLBtnDown(MouseEvent *ev) override;
    virtual bool OnLBtnUp(MouseEvent *ev) override;
    //virtual bool OnSize(SizeEvent *ev) override;

    virtual void RecreateResouce(ID2D1RenderTarget *target) override;

    Delegate<void()> Clicked;
    Delegate<void(MouseEvent *, bool &)> MouseEventDelegate;

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);
    static int SetText(lua_State *L);
    static int GetLabel(lua_State *L);
    static int SetIcon(lua_State *L);

    BEGIN_LUA_METHOD_MAP(Button)
        LUA_CHAIN_METHOD_MAP(BoxLayout)
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
    static const int AniDuration = 200;
    DWORD m_lastTick = 0;

    ID2D1SolidColorBrush *m_brush = nullptr;
    bool m_bBorder = false;
    D2D1_COLOR_F m_colorBorder;
    D2D1_COLOR_F m_colorNormal;
    D2D1_COLOR_F m_colorHover;
    D2D1_COLOR_F m_colorPressed;

    Label *m_label = nullptr;
    ImageSprite *m_image = nullptr;

    bool m_bCaptureMouse = true;
};

} // namespace
