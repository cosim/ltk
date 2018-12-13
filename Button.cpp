#include "stdafx.h"
#include "Button.h"
#include "Label.h"
#include "ltk.h"
#include "StyleManager.h"
#include "ImageSprite.h"

namespace ltk {

Button::Button() : BoxLayout(BoxLayout::Horizontal)
{
    m_colorBorder = StyleManager::Instance()->GetColor(StyleManager::clrBorder);
    m_colorNormal = StyleManager::Instance()->GetColor(StyleManager::clrNormal);
    m_colorHover = StyleManager::Instance()->GetColor(StyleManager::clrHover);
    m_colorPressed = StyleManager::Instance()->GetColor(StyleManager::clrHighlight);
}

Button::~Button()
{
    if (m_label) m_label->Release();
    m_label = INVALID_POINTER(Label);

    if (m_brush) m_brush->Release();
    m_brush = INVALID_POINTER(ID2D1SolidColorBrush);

    if (m_image) m_image->Release();
    m_image = INVALID_POINTER(ImageSprite);

    this->EndAnimation();
}

bool Button::OnPaint(PaintEvent *ev)
{
    Update();

    m_brush->SetColor(this->GetColor());

    if (m_bMousePress) {
        m_brush->SetColor(m_colorPressed);
    }
    auto rc = this->GetRect();
    rc.X = 0;
    rc.Y = 0;
    //rc.Height -= 1; // TODO FIXME
    //rc.Width -= 1;
    auto rc2 = D2D1RectF(rc);
    //auto rrc = D2D1::RoundedRect(rc2, 4, 4);
    ev->target->FillRectangle(rc2, m_brush);

    if (m_bMouseIn) {
        m_brush->SetColor(m_colorBorder);
        DrawRectSnapped(ev->target, rc, m_brush);
    }
    return true;
}

void Button::Update()
{
    DWORD timeDiff = ::GetTickCount() - m_lastTick;
    //LOG("timeDiff: " << timeDiff);
    m_lastTick = ::GetTickCount();
    if (m_state == stNormal2Hover)
    {
        m_aniCounter += timeDiff;
        if (m_aniCounter >= AniDuration)
        {
            this->EndAnimation();
        }
    }
    else if (m_state == stHover2Normal)
    {
        m_aniCounter -= timeDiff;
        if (m_aniCounter <= 0)
        {
            this->EndAnimation();
        }
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
    m_state = stNormal2Hover;
    m_lastTick = ::GetTickCount();
    return true;
}

bool Button::OnMouseLeave(MouseEvent *ev)
{
    m_bMouseIn = false;
    this->BeginAnimation();
    m_state = stHover2Normal;
    m_lastTick = ::GetTickCount();
    return true;
}

bool Button::OnLBtnDown(MouseEvent *ev)
{
    m_bMousePress = true;
    //this->SetCapture();
    this->Invalidate();
    return true;
}

bool Button::OnLBtnUp(MouseEvent *ev)
{
    m_bMousePress = false;
    //this->ReleaseCapture();

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
    HRESULT hr;
    SAFE_RELEASE(m_brush);
    hr = target->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White), &m_brush);
    assert(SUCCEEDED(hr));
}

void Button::SetText(LPCWSTR text)
{
    if (!m_label) {
        m_label = new Label;
        this->AddLayoutItem(m_label, 0.0f, 1.0f);
    }
    m_label->SetText(text);
}

D2D1_COLOR_F Button::GetColor()
{
    if (m_aniCounter > AniDuration)
        m_aniCounter = AniDuration;
    if (m_aniCounter < 0)
        m_aniCounter = 0;

    float ratio = (float)m_aniCounter / (float)AniDuration;
    float dRed = m_colorHover.r - m_colorNormal.r;
    float dGreen = m_colorHover.g - m_colorNormal.g;
    float dBlue = m_colorHover.b - m_colorNormal.b;

    D2D1_COLOR_F result;
    result.a = 1.0f;
    result.r = dRed * ratio + m_colorNormal.r;
    result.g = dGreen * ratio + m_colorNormal.g;
    result.b = dBlue * ratio + m_colorNormal.b;

    return result;
}

Label *Button::GetLabel()
{
    return m_label;
}

void Button::SetNormalColor(D2D1_COLOR_F clr)
{
    m_colorNormal = clr;
    this->Invalidate();
}

void Button::SetHoverColor(D2D1_COLOR_F clr)
{
    m_colorHover = clr;
    this->Invalidate();
}

void Button::SetIcon(const RectF &rc, float scale, UINT idx)
{
    if (!m_image) {
        m_image = new ImageSprite();
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
