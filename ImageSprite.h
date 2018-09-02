#pragma once
#include "Sprite.h"
#include "ltk.h"

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

    void SetIcon(const RectF &atlas, float scale, UINT idx);

    void SetTexture(const RectF &atlas, const Margin &margin, float scale, UINT idx);

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);
    static int SetTexture(lua_State *L);

    BEGIN_LUA_METHOD_MAP(ImageSprite)
        LUA_CHAIN_METHOD_MAP(Sprite)
        LUA_METHOD_ENTRY(SetTexture)
    END_LUA_METHOD_MAP()
#endif // LTK_DISABLE_LUA

private:
    IconInfo *m_icon = nullptr;
    TextureInfo *m_texture = nullptr;
    ID2D1Bitmap *m_bitmap = nullptr;
};

}
