#include "stdafx.h"
#include "CheckBox.h"
#include "ImageSprite.h"
#include "Label.h"
#include "StyleManager.h"

namespace ltk {

CheckBox::CheckBox() : BoxLayout(BoxLayout::Horizontal)
{
    m_box = new IconSprite;
    this->AddLayoutItem(m_box, 24, 0.0f);
    m_label = new Label;
    m_label->SetTextAlign(DWRITE_TEXT_ALIGNMENT_LEADING);
    this->AddLayoutItem(m_label, 0, 1.0f);
    this->UpdateIcon();
}

CheckBox::~CheckBox()
{
    if (m_radioGroup) {
        m_radioGroup->Dettach(this);
    }
    RELEASE_AND_INVALIDATE(IconSprite, m_box);
    RELEASE_AND_INVALIDATE(Label, m_label);
    RELEASE_AND_INVALIDATE(RadioGroup, m_radioGroup);
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

void CheckBox::SetCheckState(State s)
{
    m_state = s;
    this->UpdateIcon();
}

bool CheckBox::IsChecked()
{
    return m_state == Checked;
}

int CheckBox::IsChecked(lua_State *L)
{
    auto thiz = CheckLuaObject<CheckBox>(L, 1);
    lua_pushboolean(L, thiz->IsChecked() ? 1 : 0);
    return 1;
}

void CheckBox::SetRadioGroup(RadioGroup *group)
{
    SAFE_RELEASE(m_radioGroup);
    group->AddRef();
    group->Attach(this);
    m_radioGroup = group;
}

int CheckBox::SetRadioGroup(lua_State *L)
{
    auto thiz = CheckLuaObject<CheckBox>(L, 1);
    auto radio = CheckLuaObject<RadioGroup>(L, 2);
    thiz->SetRadioGroup(radio);
    return 0;
}

bool CheckBox::OnLBtnDown(MouseEvent *ev)
{
    if (m_radioGroup) {
        m_state = Checked;
        m_radioGroup->SetCheck(this);
    }
    else {
        if (m_state == Checked) {
            m_state = Unchecked;
        }
        else {
            m_state = Checked;
        }
    }
    this->UpdateIcon();
    return true;
}

bool CheckBox::OnMouseEnter(MouseEvent *ev)
{
    m_label->SetTextColor(StyleManager::Instance()->GetColor(StyleManager::clrBorder));
    this->TrackMouseLeave();
    return true;
}

bool CheckBox::OnMouseLeave(MouseEvent *ev)
{
    m_label->SetTextColor(StyleManager::Instance()->GetColor(StyleManager::clrText));
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
    if (m_state == Unchecked) {
        m_box->SetIcon(RectF(1, 71, 23, 23), 1.0f, 0);
    }
    else if (m_state == Checked) {
        m_box->SetIcon(RectF(27, 71, 23, 23), 1.0f, 0);
    }
}

void RadioGroup::Attach(CheckBox *box)
{
    m_setCheck.insert(box);
}

void RadioGroup::Dettach(CheckBox *box)
{
    auto iter = m_setCheck.find(box);
    if (iter != m_setCheck.end()) {
        m_setCheck.erase(iter);
    }
}

void RadioGroup::SetCheck(CheckBox *box)
{
    for (auto item : m_setCheck) {
        if (item != box) {
            item->SetCheckState(CheckBox::Unchecked);
        }
    }
}

int RadioGroup::LuaConstructor(lua_State *L)
{
    auto obj = new RadioGroup;
    obj->PushToLua(L, "RadioGroup");
    obj->Release();
    return 1;
}

} // namespace ltk
