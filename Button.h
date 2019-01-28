#pragma once

#include "Delegate.h"
#include "BoxLayout.h"
#include "StyleManager.h"

namespace ltk {

class Label;
class IconSprite;

class Button : public BoxLayout
{
public:
    RTTI_DECLARATIONS(Button, BoxLayout);
    Button();
    virtual ~Button();

    typedef AbstractBackground::State State;

    void SetBackgroundStyle(const char *style);
    void SetText(LPCWSTR text);
    Label *GetLabel();
    void EnableCapture(bool);

    void SetIcon(const RectF &rc, float scale, UINT idx = 0);

    virtual bool OnPaint(PaintEvent *ev) override;

    void Update();

    virtual bool OnEvent(Event *ev) override;

    virtual bool OnMouseEnter(MouseEvent *ev) override;
    virtual bool OnMouseLeave(MouseEvent *ev) override;
    virtual bool OnLBtnDown(MouseEvent *ev) override;
    virtual bool OnLBtnUp(MouseEvent *ev) override;

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
    bool m_bCaptureMouse = true;

    static const int AniDuration = 200;

    State m_state = State::Normal;
    int m_aniCounter = 0;
    DWORD m_lastTick = 0;

    Label *m_label = nullptr;
    IconSprite *m_image = nullptr;
    AbstractBackground *m_background = nullptr;
};

} // namespace
