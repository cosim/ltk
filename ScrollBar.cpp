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
    thiz->Release();
    return 1;
}

ScrollBar::ScrollBar(Mode mode) : m_mode(mode)
{
    m_slider = new Button;
    this->AddChild(m_slider);

    using namespace std::placeholders;
    m_slider->MouseEventDelegate.Attach(std::bind(&ScrollBar::OnSilderEvent, this, _1, _2));
}

ScrollBar::~ScrollBar()
{
    if (m_slider) {
        m_slider->Release();
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
        rc2.Y = 1;
        rc2.Width = slider_size;
        rc2.Height = rc.Height - 2;
    }
    else {
        rc2.X = 1;
        rc2.Y = slider_size;
        rc2.Width = rc.Width - 2;
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
    LTK_LOG("m_position %f m_mode: %d", m_position, m_mode);
    if (!m_bDrag) {
        Update(m_contentSize, m_position);
    }
    auto brush = StyleManager::Instance()->GetStockBrush();
    brush->SetColor(D2D1::ColorF(D2D1::ColorF::DarkGray));
    DrawRectSnapped(ev->target, this->GetClientRect(), brush);
    return true;
}

bool ScrollBar::OnMouseMove(MouseEvent *ev)
{
    //LTK_LOG("OnMouseMove %f %f", ev->x, ev->y);
    if (m_bDrag) {
        float x = ev->x - m_deltaX;
        float y = ev->y - m_deltaY;
        auto rcSlider = m_slider->GetRect();
        auto rcRoot = this->GetRect();

        if (m_mode == Horizontal) {
            x = max(x, 0.0f);
            x = min(x, rcRoot.Width - rcSlider.Width);
            y = max(y, 0.0f);
            LTK_LOG("m_slider->SetRect %f 1.0", x);
            m_slider->SetRect(RectF(x, 1.0f, rcSlider.Width, rcSlider.Height));
            m_position = x / (rcRoot.Width - rcSlider.Width) * (m_contentSize - rcRoot.Width);
        }
        else {
            x = max(x, 0.0f);
            y = max(y, 0.0f);
            y = min(y, rcRoot.Height - rcSlider.Height);
            LTK_LOG("m_slider->SetRect 1.0 %f", y);
            m_slider->SetRect(RectF(1.0f, y, rcSlider.Width, rcSlider.Height));
            m_position = y / (rcRoot.Height - rcSlider.Height) * (m_contentSize - rcRoot.Height);
        }
    }
    return true;
}

bool ScrollBar::OnLBtnUp(MouseEvent *ev)
{
    LTK_LOG("OnLBtnUp");
    m_bDrag = false;
    this->ReleaseCapture();
    m_slider->OnLBtnUp(ev);
    this->Invalidate();
    return true;
}

void ScrollBar::OnSilderEvent(MouseEvent *ev, bool &bHandled)
{
    switch (ev->id) {
    case eLBtnDown:
        LTK_LOG("eLBtnDown %f %f", ev->x, ev->y);
        m_bDrag = true;
        m_deltaX = ev->x;
        m_deltaY = ev->y;
        this->SetCapture();
        break;
    case eLBtnUp:

        break;
    case eMouseMove:
        //LTK_LOG("eMouseMove %f %f", ev->x, ev->y);
        if (m_bDrag) {
            int i = 1;
            i = 1;
        }
        break;
    }
    bHandled = false;
}

}