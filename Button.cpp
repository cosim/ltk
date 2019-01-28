#include "stdafx.h"
#include "Button.h"
#include "Label.h"
#include "ltk.h"
#include "StyleManager.h"
#include "ImageSprite.h"

namespace ltk {

Button::Button() : BoxLayout(BoxLayout::Horizontal)
{
    m_background = StyleManager::Instance()->GetBackgroundStyle("default_button");
}

Button::~Button()
{
    RELEASE_AND_INVALIDATE(Label, m_label);
    RELEASE_AND_INVALIDATE(IconSprite, m_image);
    RELEASE_AND_INVALIDATE(AbstractBackground, m_background);

    this->EndAnimation();
}

void Button::SetBackgroundStyle(const char *style)
{
    SAFE_RELEASE(m_background);
    m_background = StyleManager::Instance()->GetBackgroundStyle(style);
}

bool Button::OnPaint(PaintEvent *ev)
{
    Update();

    Window *wnd = this->GetWindow();
    auto rc = this->GetClientRect();
    if (m_background) {
        m_background->Draw(wnd, ev->target, rc, m_state, (float)m_aniCounter / AniDuration);
    }
    return true;
}

void Button::Update()
{
    DWORD timeDiff = ::GetTickCount() - m_lastTick;
    //LTK_LOG("timeDiff: %d", timeDiff);
    m_lastTick = ::GetTickCount();
    if (m_state == State::Normal2Hover)
    {
        m_aniCounter += timeDiff;
        if (m_aniCounter >= AniDuration)
        {
            this->EndAnimation();
        }
    }
    else if (m_state == State::Hover2Normal)
    {
        m_aniCounter -= timeDiff;
        if (m_aniCounter <= 0)
        {
            this->EndAnimation();
        }
    }
    else
    {
        this->EndAnimation();
    }
}

bool Button::OnEvent(Event *ev)
{
    bool bHandled = false;
    if (ev->id > eMouseFirst && ev->id < eMouseLast) {
        this->MouseEventDelegate.Invoke((MouseEvent *)ev, std::ref(bHandled));
    }
    if (!bHandled) {
        return Sprite::OnEvent(ev);
    }
    else {
        return true;
    }
}

bool Button::OnMouseEnter(MouseEvent *ev)
{
    m_bMouseIn = true;
    this->BeginAnimation();
    this->TrackMouseLeave();
    m_state = State::Normal2Hover;
    m_lastTick = ::GetTickCount();
    return true;
}

bool Button::OnMouseLeave(MouseEvent *ev)
{
    m_bMouseIn = false;
    this->BeginAnimation();
    m_state = State::Hover2Normal;
    m_lastTick = ::GetTickCount();
    return true;
}

bool Button::OnLBtnDown(MouseEvent *ev)
{
    m_bMousePress = true;
    if (m_bCaptureMouse) {
        this->SetCapture();
    }
    this->Invalidate();
    return true;
}

bool Button::OnLBtnUp(MouseEvent *ev)
{
    m_bMousePress = false;
    if (m_bCaptureMouse){
        this->ReleaseCapture();
    }
    auto rc = this->GetClientRect();
    if (rc.Contains(Gdiplus::PointF(ev->x, ev->y))) {
        this->Clicked.Invoke();
        this->CallEventHandler(GetGlobalLuaState(), "OnClick", 0, 0);
    }
    else {
        this->OnMouseLeave(ev);
    }
    this->Invalidate();
    return true;
}

void Button::RecreateResouce(ID2D1RenderTarget *target)
{
}

void Button::SetText(LPCWSTR text)
{
    if (!m_label) {
        m_label = new Label;
        this->AddLayoutItem(m_label, 0.0f, 1.0f);
    }
    m_label->SetText(text);
}

Label *Button::GetLabel()
{
    return m_label;
}

void Button::EnableCapture(bool v)
{
    m_bCaptureMouse = v;
}

void Button::SetIcon(const RectF &rc, float scale, UINT idx)
{
    if (!m_image) {
        m_image = new IconSprite();
        m_image->SetIcon(rc, scale, idx);
        this->InsertLayoutItem(0, m_image, rc.Width * scale, 0.0f);
    }
    m_image->SetIcon(rc, scale, idx);
    this->Invalidate();
}

#ifndef LTK_DISABLE_LUA

int Button::LuaConstructor(lua_State *L)
{
    Button *thiz = new Button;
    thiz->PushToLua(L, "Button");
    thiz->Release();
    return 1;
}

int Button::SetText(lua_State *L)
{
    Button *thiz = CheckLuaObject<Button>(L, 1);
    auto text = LuaCheckWString(L, 2);
    thiz->SetText(text);
    return 0;
}

int Button::GetLabel(lua_State *L)
{
    Button *thiz = CheckLuaObject<Button>(L, 1);
    Label *label = thiz->GetLabel();
    if (label) {
        label->PushToLua(L, "Label");
    }
    else {
        lua_pushnil(L);
    }
    return 1;
}

int Button::SetIcon(lua_State *L)
{
    Button *thiz = CheckLuaObject<Button>(L, 1);
    RectF atlas = LuaCheckRectF(L, 2);
    float scale = (float)luaL_checknumber(L, 3);
    thiz->SetIcon(atlas, scale);
    return 0;
}

#endif // LTK_DISABLE_LUA

} // namespace
