#include "stdafx.h"
#include "CheckBox.h"
#include "ImageSprite.h"
#include "Label.h"

namespace ltk {

CheckBox::CheckBox() : BoxLayout(BoxLayout::Horizontal)
{
    m_box.Reset(new IconSprite);
    this->AddLayoutItem(m_box.Get(), 24, 0.0f);
    m_label.Reset(new Label);
    m_label->SetTextAlign(DWRITE_TEXT_ALIGNMENT_LEADING);
    this->AddLayoutItem(m_label.Get(), 0, 1.0f);
    this->UpdateIcon();
}

CheckBox::~CheckBox()
{
}

void CheckBox::SetText(const wchar_t *t)
{
    m_label->SetText(t);
}

int CheckBox::SetText(lua_State *L)
{
    auto thiz = CheckLuaObject<CheckBox>(L, 1);
    auto text = LuaCheckWString(L, 2);
    thiz->SetText(text);
    return 0;
}

bool CheckBox::IsChecked()
{
    return m_checked;
}

int CheckBox::IsChecked(lua_State *L)
{
    auto thiz = CheckLuaObject<CheckBox>(L, 1);
    lua_pushboolean(L, thiz->IsChecked() ? 1 : 0);
    return 1;
}

bool CheckBox::OnLBtnDown(MouseEvent *ev)
{
    m_checked = !m_checked;
    this->UpdateIcon();
    return true;
}

bool CheckBox::OnMouseEnter(MouseEvent *ev)
{
    return true;
}

bool CheckBox::OnMouseLeave(MouseEvent *ev)
{
    return true;
}

int CheckBox::LuaConstructor(lua_State *L)
{
    auto obj = new CheckBox();
    obj->PushToLua(L, "CheckBox");
    obj->Release();
    return 1;
}

void CheckBox::UpdateIcon()
{
    if (m_checked) {
        m_box->SetIcon(RectF(892, 2, 66, 62), 16.0f / 70.0f, 0);
    }
    else {
        m_box->SetIcon(RectF(958, 2, 66, 62), 16.0f / 70.0f, 0);
    }
}

} // namespace ltk
