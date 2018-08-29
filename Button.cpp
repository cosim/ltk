#include "stdafx.h"
#include "Button.h"
#include "Label.h"
#include "ltk.h"
#include "StyleManager.h"

namespace ltk {

Button::Button()
{
    m_colorBorder = StyleManager::Instance()->GetColor(StyleManager::clrBorder);
    m_colorNormal = StyleManager::Instance()->GetColor(StyleManager::clrNormal);
    m_colorHover = StyleManager::Instance()->GetColor(StyleManager::clrHover);
    m_colorPressed = StyleManager::Instance()->GetColor(StyleManager::clrHighlight);
}

Button::~Button()
{
    if (m_label) m_label->Unref();
    m_label = INVALID_POINTER(Label);

    if (m_brush) m_brush->Release();
    m_brush = INVALID_POINTER(ID2D1SolidColorBrush);

    if (m_icon) delete m_icon;
    m_icon = INVALID_POINTER(IconInfo);

    this->EndAnimation();
}

bool Button::OnPaint(PaintEvent *ev)
{
    Update();

    m_brush->SetColor(this->GetColor());

    auto rc = this->GetRect();

    if (m_bMousePress) {
        m_brush->SetColor(m_colorPressed);
    }
    rc.X = 0;
    rc.Y = 0;
    rc.Height -= 2; // TODO FIXME
    auto rc2 = D2D1RectF(rc);
    //auto rrc = D2D1::RoundedRect(rc2, 4, 4);
    ev->target->FillRectangle(rc2, m_brush);

    if (m_bBorder) {
        m_brush->SetColor(m_colorBorder);
        ev->target->DrawRectangle(rc2, m_brush);
    }

    if (m_icon) {
        if (m_label) {

        }
        else {
            RectF rc3;
            rc3.X = (rc.Width - m_icon->atlas.Width * m_icon->scale) / 2.0f;
            rc3.Y = (rc.Height - m_icon->atlas.Height * m_icon->scale) / 2.0f;
            rc3.Width = m_icon->atlas.Width * m_icon->scale;
            rc3.Height = m_icon->atlas.Height * m_icon->scale;
            auto bitmap = StyleManager::Instance()->GetBitmap(ev->target);
            ev->target->DrawBitmap(bitmap, D2D1RectF(rc3), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                D2D1RectF(m_icon->atlas));
        }
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
    this->SetCapture();
    this->Invalidate();
    return true;
}

bool Button::OnLBtnUp(MouseEvent *ev)
{
    m_bMousePress = false;
    this->ReleaseCapture();

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

bool Button::OnSize(SizeEvent *ev)
{
    if (m_icon) {
        if (m_label) {

        }
    }
    else {
        if (m_label) {
            m_label->SetRect(RectF(0.0f, 0.0f, ev->width, ev->height));
        }
    }
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
        this->AddChild(m_label);
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

void Button::SetIcon(const RectF &rc, float scale, bool iconOnTop, UINT idx)
{
    if (!m_icon) {
        m_icon = new IconInfo;
    }
    m_icon->atlas = rc;
    m_icon->scale = scale;
    m_icon->bIconOnTop = iconOnTop;
    m_icon->idx = idx;
    this->Invalidate();
}

#ifndef LTK_DISABLE_LUA

int Button::LuaConstructor(lua_State *L)
{
    Button *thiz = new Button;
    thiz->PushToLua(L, "Button");
    thiz->Unref();
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
