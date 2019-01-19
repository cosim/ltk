#include "stdafx.h"
#include "CheckBox.h"
#include "ImageSprite.h"
#include "Label.h"

namespace ltk {

CheckBox::CheckBox() : BoxLayout(BoxLayout::Horizontal)
{
    m_box.Reset(new ImageSprite);
    m_box->SetIcon(RectF(895, 0, 70, 70), 16.0f / 70.0f, 0);
    this->AddLayoutItem(m_box.Get(), 24, 0.0f);
    m_label.Reset(new Label);
    this->AddLayoutItem(m_label.Get(), 0, 1.0f);
}

CheckBox::~CheckBox()
{
}

void CheckBox::SetText(const wchar_t *t)
{
    m_label->SetText(t);
}

} // namespace ltk
