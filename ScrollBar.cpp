#include "stdafx.h"
#include "ScrollBar.h"
#include "Button.h"

namespace ltk {

const float ScrollBar::SLIDER_MIN = 10.0f;

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

void ScrollBar::Update()
{
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
    }
    else {
        m_slider->SetVisible(true);
    }
    float slider_size = view_size / m_contentSize * view_size;
    slider_size = max(SLIDER_MIN, slider_size);

}

}