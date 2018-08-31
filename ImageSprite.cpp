#include "stdafx.h"
#include "ImageSprite.h"
#include "StyleManager.h"
#include "ltk.h"

namespace ltk {

ImageSprite::~ImageSprite()
{
    delete m_icon;
    m_icon = INVALID_POINTER(IconInfo);
}

bool ImageSprite::OnPaint(PaintEvent *ev)
{
    RectF rc = this->GetClientRect();
    RectF rc3;
    rc3.X = (rc.Width - m_icon->atlas.Width * m_icon->scale) / 2.0f;
    rc3.Y = (rc.Height - m_icon->atlas.Height * m_icon->scale) / 2.0f;
    rc3.Width = m_icon->atlas.Width * m_icon->scale;
    rc3.Height = m_icon->atlas.Height * m_icon->scale;
    auto bitmap = StyleManager::Instance()->GetBitmap(ev->target);
    ev->target->DrawBitmap(bitmap, D2D1RectF(rc3), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
        D2D1RectF(m_icon->atlas));
    return true;
}

void ImageSprite::RecreateResouce(ID2D1RenderTarget *target)
{
}


void ImageSprite::SetIcon(const RectF &rc, float scale, UINT idx)
{
    if (!m_icon) {
        m_icon = new IconInfo;
    }
    m_icon->atlas = rc;
    m_icon->scale = scale;
    m_icon->idx = idx;
    this->Invalidate();
}

}