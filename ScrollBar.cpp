#include "stdafx.h"
#include "ScrollBar.h"
#include "Button.h"
#include "StyleManager.h"

namespace ltk {

const float ScrollBar::SLIDER_MIN = 10.0f;

int ScrollBar::LuaConstructor(lua_State *L)
{
    const char *options[] = {
        "horizontal", "vertical", nullptr
    };
    auto m = (Mode)luaL_checkoption(L, 2, nullptr, options);
    ScrollBar *thiz = new ScrollBar(m);
    thiz->PushToLua(L, "ScrollBar");
    thiz->Unref();
    return 1;
}

ScrollBar::ScrollBar(Mode mode) : m_mode(mode)
{
    m_slider = new Button;
    this->AddChild(m_slider);
}

ScrollBar::~ScrollBar()
{
    if (m_slider) {
        m_slider->Unref();
    }
    m_slider = INVALID_POINTER(Button);
}

void ScrollBar::SetContentSize(float size)
{
    m_contentSize = size;
}

void ScrollBar::SetPosition(float pos)
{
    m_position = pos;
}

void ScrollBar::Update(float size, float pos)
{
    m_contentSize = size;
    m_position = pos;

    auto rc = this->GetRect();

    float view_size;
    if (m_mode == Horizontal) {
        view_size = rc.Width;
    }
    else {
        view_size = rc.Height;
    }
    if (m_contentSize <= view_size) {
        m_slider->SetVisible(false);
        return;
    }
    else {
        m_slider->SetVisible(true);
    }
    float slider_size = view_size / m_contentSize * view_size;
    slider_size = max(SLIDER_MIN, slider_size);
    float slider_pos = m_position / (m_contentSize - view_size)
        * (view_size - slider_size);

    RectF rc2;
    if (m_mode == Horizontal) {
        rc2.X = slider_pos;
        rc2.Y = 0;
        rc2.Width = slider_size;
        rc2.Height = rc.Height;
    }
    else {
        rc2.X = 0;
        rc2.Y = slider_size;
        rc2.Width = rc.Width;
        rc2.Height = slider_size;
    }
    m_slider->SetRect(rc2);
}

int ScrollBar::Update(lua_State *L)
{
    ScrollBar *thiz = CheckLuaObject<ScrollBar>(L, 1);
    float size = (float)luaL_checknumber(L, 2);
    float pos = (float)luaL_checknumber(L, 3);
    thiz->Update(size, pos);
    return 0;
}

bool ScrollBar::OnPaint(PaintEvent *ev)
{
    Update(m_contentSize, m_position);
    auto brush = StyleManager::Instance()->GetStockBrush();
    brush->SetColor(D2D1::ColorF(D2D1::ColorF::DarkGray));
    DrawRectSnapped(ev->target, this->GetClientRect(), brush);
    return true;
}

void ScrollBar::OnSilderEvent(MouseEvent *ev, bool &bHandled)
{

}

}