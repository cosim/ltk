#pragma once
#include "Sprite.h"

namespace ltk {

struct IconInfo {
    UINT idx = 0;
    RectF atlas;
    bool bIconOnTop = false;
    float scale = 1.0f;
};

class ImageSprite : public Sprite
{
public:
    RTTI_DECLARATIONS(ImageSprite, Sprite);
    ImageSprite() {}
    ~ImageSprite();

    virtual bool OnPaint(PaintEvent *ev);

    virtual void RecreateResouce(ID2D1RenderTarget *target);

    void SetIcon(const RectF &rc, float scale, UINT idx);

private:
    IconInfo *m_icon = nullptr;
    ID2D1Bitmap *m_bitmap = nullptr;
};

}
