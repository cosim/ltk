#include "stdafx.h"
#include "Button.h"
#include "LuaUI2.h"

namespace cs {

Button::Button()
{
    
}

Button::~Button()
{
    SAFE_RELEASE(m_text_format);
    SAFE_RELEASE(m_brush);
}

bool Button::OnPaint(PaintEvent *ev)
{
    m_brush->SetColor(D2D1::ColorF(0xDDDDFF));
    if (m_bMouseIn)
    {
        m_brush->SetColor(D2D1::ColorF(0xEEEEFF));
    }
    auto rc = this->GetRect();

    if (m_bMousePress)
        rc.Y = 2;
    else 
        rc.Y = 0;

    rc.X = 0;
    rc.Height -= 2;
    auto rc2 = GdipRectF2D2D1RectF(rc);
    //ev->graphics->FillRectangle(&brush, rc);
    ev->target->FillRectangle(rc2, m_brush);

    m_brush->SetColor(D2D1::ColorF(0.5f, 0.5f, 1.0f, 1.0f));
    ev->target->DrawRectangle(rc2, m_brush);

    m_brush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
    ev->target->DrawText(m_strText.c_str(), m_strText.length(), m_text_format, rc2, m_brush);

/*
    brush.SetColor(Color(0,0,0));
    StringFormat fmt;
    fmt.SetAlignment(StringAlignmentCenter);
    fmt.SetLineAlignment(StringAlignmentCenter);
    ev->graphics->DrawString(m_strText.c_str(), m_strText.length(), &m_font, rc, &fmt, &brush);
*/
    return true;
}

bool Button::OnMouseEnter(MouseEvent *ev)
{
    m_bMouseIn = true;
    this->Invalidate();
    this->TrackMouseLeave();
    return true;
}

bool Button::OnMouseLeave(MouseEvent *ev)
{
    m_bMouseIn = false;
    this->Invalidate();
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
    this->Invalidate();
    return true;
}

bool Button::OnLBtnUp(MouseEvent *ev)
{
    m_bMousePress = false;
    this->Invalidate();
    this->Clicked.Invoke();
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

} // namespace
