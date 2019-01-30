#pragma once

#include "Sprite.h"

namespace ltk {

class Button;

class ScrollBar : public Sprite
{
public:
    RTTI_DECLARATIONS(ScrollBar, Sprite)

    virtual void OnEvent(Event *ev) override;

    enum Mode {
        Horizontal, Vertical
    };

    ScrollBar(Mode mode);
    ~ScrollBar();

    void SetContentSize(float size);

    void SetPosition(float pos);

    void Update();

    void OnSilderEvent(MouseEvent *ev, bool &bHandled);

    static const float SLIDER_MIN;

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);
    static int Update(lua_State *L);
    static int SetContentSize(lua_State *L);
    static int SetPosition(lua_State *L);

    BEGIN_LUA_METHOD_MAP(ScrollBar)
        LUA_CHAIN_METHOD_MAP(Sprite)
        LUA_METHOD_ENTRY(Update)
        LUA_METHOD_ENTRY(SetContentSize)
        LUA_METHOD_ENTRY(SetPosition)
    END_LUA_METHOD_MAP()
#endif // LTK_DISABLE_LUA


    virtual bool OnPaint(PaintEvent *ev) override;
    virtual bool OnMouseMove(MouseEvent *ev) override;

    virtual bool OnLBtnUp(MouseEvent *ev) override;

    virtual bool OnThumbDragging(float pos);

    virtual void RecreateResouce(ID2D1RenderTarget *target);

private:
    Mode m_mode;
    Button *m_slider = nullptr;
    float m_contentSize = 100.0f;
    float m_position = 0.0f;
    bool m_bDrag = false;
    float m_deltaX = 0.0f;
    float m_deltaY = 0.0f;
    ID2D1SolidColorBrush *m_brush = nullptr;
};

}