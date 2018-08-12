#include "stdafx.h"
#include "Button.h"
#include "ltk.h"

namespace ltk {

Button::Button()
{
    
}

Button::~Button()
{
    SAFE_RELEASE(m_text_format);
    SAFE_RELEASE(m_brush);
    this->EndAnimation();
}

bool Button::OnPaint(PaintEvent *ev)
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

    m_brush->SetColor(this->GetColor());

    auto rc = this->GetRect();

    //if (m_bMousePress)
    //    rc.Y = 2;
    //else 
        rc.Y = 0;

    rc.X = 0;
    rc.Height -= 2;
    auto rc2 = GdipRectF2D2D1RectF(rc);
    //ev->graphics->FillRectangle(&brush, rc);
    ev->target->FillRectangle(rc2, m_brush);

    m_brush->SetColor(D2D1::ColorF(0.5f, 0.5f, 1.0f, 1.0f));
    ev->target->DrawRectangle(rc2, m_brush);

    m_brush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
    ev->target->DrawText(m_strText.c_str(), (UINT32)m_strText.length(), m_text_format, rc2, m_brush);

    return true;
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

void Button::SetText(LPCWSTR text)
{
    m_strText = text;
    this->Invalidate();
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
    this->OnMouseLeave(ev);
    auto rc = this->GetClientRect();
    this->Invalidate();
    if (rc.Contains(Gdiplus::PointF(ev->x, ev->y))) {
        this->Clicked.Invoke();
    }
    return true;
}

void Button::RecreateResouce(ID2D1RenderTarget *target)
{
    HRESULT hr;
    SAFE_RELEASE(m_text_format);
    hr = GetDWriteFactory()->CreateTextFormat(
        L"Î¢ÈíÑÅºÚ",
        NULL,
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        14.0f,
        L"zh-cn",
        &m_text_format
        );
    assert(SUCCEEDED(hr));
    m_text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    m_text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    SAFE_RELEASE(m_brush);
    hr = target->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White), &m_brush);
    assert(SUCCEEDED(hr));
}

D2D1_COLOR_F Button::GetColor()
{
    if (m_aniCounter > AniDuration)
        m_aniCounter = AniDuration;
    if (m_aniCounter < 0)
        m_aniCounter = 0;

    UINT32 db = 0xEE - 0xCC;
    UINT32 dg = 0xEE - 0xCC;
    
    //UINT32 diff = 0xEEEEFF - 0xDDDDFF;
    UINT32 b = db * m_aniCounter / AniDuration + 0xCC;
    UINT32 g = dg * m_aniCounter / AniDuration + 0xCC;
    UINT32 clr = (b << 16) + (g << 8) + 0xFF;
    return D2D1::ColorF((UINT32)clr);
}

} // namespace
