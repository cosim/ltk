#pragma once

#include "Sprite.h"
#include "Delegate.h"

namespace ltk {

class Button : public Sprite
{
public:
    Button();
    virtual ~Button();

    virtual bool OnPaint(PaintEvent *ev) override;

    virtual bool OnMouseEnter(MouseEvent *ev) override;
    virtual bool OnMouseLeave(MouseEvent *ev) override;

    void SetText(LPCWSTR text);

    virtual bool OnLBtnDown(MouseEvent *ev) override;
    virtual bool OnLBtnUp(MouseEvent *ev) override;

    virtual void RecreateResouce(ID2D1RenderTarget *target) override;

    Delegate<void()> Clicked;

    D2D1_COLOR_F GetColor();

private:
    bool m_bMouseIn = false;
    bool m_bMousePress = false;
    wstring m_strText;
    IDWriteTextFormat *m_text_format = nullptr;
    ID2D1SolidColorBrush *m_brush = nullptr;

    enum AniState { stStoped, stNormal2Hover, stHover2Normal };
    AniState m_state = stStoped;
    int m_aniCounter = 0;
    enum { AniDuration = 200 };
    DWORD m_lastTick = 0;
};

} // namespace
