#include "stdafx.h"
#include "ImageSprite.h"
#include "StyleManager.h"
#include "ltk.h"

namespace ltk {

ImageSprite::~ImageSprite()
{
    delete m_icon;
    m_icon = INVALID_POINTER(IconInfo);

    delete m_texture;
    m_texture = INVALID_POINTER(TextureInfo);
}

bool ImageSprite::OnPaint(PaintEvent *ev)
{
    RectF rc = this->GetClientRect();
    if (m_icon) {
        RectF rc3;
        rc3.X = (rc.Width - m_icon->atlas.Width * m_icon->scale) / 2.0f;
        rc3.Y = (rc.Height - m_icon->atlas.Height * m_icon->scale) / 2.0f;
        rc3.Width = m_icon->atlas.Width * m_icon->scale;
        rc3.Height = m_icon->atlas.Height * m_icon->scale;
        auto bitmap = StyleManager::Instance()->GetBitmap();
        ev->target->DrawBitmap(bitmap, D2D1RectF(rc3), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            D2D1RectF(m_icon->atlas));
    }
    if (m_texture) {
        DrawTextureNineInOne(
            ev->target, 
            StyleManager::Instance()->GetBitmap(m_texture->idx), 
            m_texture->atlas,
            m_texture->margin, 
            rc, 
            1.0f, 
            m_texture->scale);
    }
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

void ImageSprite::SetTexture(const RectF &atlas, const Margin &margin, float scale, UINT idx)
{
    if (!m_texture) {
        m_texture = new TextureInfo;
    }
    m_texture->atlas = atlas;
    m_texture->margin = margin;
    m_texture->scale = scale;
    this->Invalidate();
}

#ifndef LTK_DISABLE_LUA

int ImageSprite::LuaConstructor(lua_State *L)
{
    ImageSprite *thiz = new ImageSprite;
    thiz->PushToLua(L, "ImageSprite");
    thiz->Release();
    return 1;
}

int ImageSprite::SetTexture(lua_State *L)
{
    ImageSprite *thiz = CheckLuaObject<ImageSprite>(L, 1);
    RectF atlas = LuaCheckRectF(L, 2);
    Margin margin = LuaCheckMargin(L, 3);
    float scale = (float)luaL_checknumber(L, 4);
    UINT idx = 0;
    if (!lua_isnone(L, 5)) {
        idx = (UINT)luaL_checkinteger(L, 5);
    }
    thiz->SetTexture(atlas, margin, scale, idx);
    return 0;
}

#endif // LTK_DISABLE_LUA

}