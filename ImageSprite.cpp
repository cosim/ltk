#include "stdafx.h"
#include "ImageSprite.h"
#include "StyleManager.h"
#include "ltk.h"
#include "Window.h"

namespace ltk {

bool IconSprite::OnPaint(PaintEvent *ev)
{
    RectF rc = this->GetClientRect();
    RectF rc3;
    rc3.X = (rc.Width - m_info.atlas.Width * m_info.scale) / 2.0f;
    rc3.Y = (rc.Height - m_info.atlas.Height * m_info.scale) / 2.0f;
    rc3.Width = m_info.atlas.Width * m_info.scale;
    rc3.Height = m_info.atlas.Height * m_info.scale;
    auto bitmap = this->GetWindow()->GetAtlasBitmap();
    ev->target->DrawBitmap(bitmap, D2D1RectF(rc3), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
        D2D1RectF(m_info.atlas));
    return true;
}

int IconSprite::LuaConstructor(lua_State *L)
{
    IconSprite *thiz = new IconSprite;
    thiz->PushToLua(L, "IconSprite");
    thiz->Release();
    return 1;
}

void IconSprite::SetIcon(const RectF &atlas, float scale, UINT idx)
{
    m_info.atlas = atlas;
    m_info.scale = scale;
    m_info.idx = idx;
    this->Invalidate();
}

void IconSprite::SetIcon(const IconInfo &v)
{
    m_info = v;
    this->Invalidate();
}

int IconSprite::SetIcon(lua_State *L)
{
    IconSprite *thiz = CheckLuaObject<IconSprite>(L, 1);
    RectF atlas = LuaCheckRectF(L, 2);
    float scale = (float)luaL_checknumber(L, 3);
    UINT idx = 0;
    thiz->SetIcon(atlas, scale, idx);
    return 0;
}

int TextureSprite::LuaConstructor(lua_State *L)
{
    TextureSprite *thiz = new TextureSprite;
    thiz->PushToLua(L, "TextureSprite");
    thiz->Release();
    return 1;
}

bool TextureSprite::OnPaint(PaintEvent *ev)
{
    RectF rc = this->GetClientRect();
    DrawTextureNineInOne(
        ev->target,
        this->GetWindow()->GetAtlasBitmap(),
        m_info.atlas,
        m_info.margin,
        rc,
        1.0f,
        m_info.scale);
    return true;
}

void TextureSprite::SetTexture(const RectF &atlas, const Margin &margin, float scale, UINT idx)
{
    m_info.atlas = atlas;
    m_info.margin = margin;
    m_info.scale = scale;
    this->Invalidate();
}

int TextureSprite::SetTexture(lua_State *L)
{
    TextureSprite *thiz = CheckLuaObject<TextureSprite>(L, 1);
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

ImageSprite::~ImageSprite()
{
    RELEASE_AND_INVALIDATE(ID2D1Bitmap, m_bitmap);
}

bool ImageSprite::OnPaint(PaintEvent *ev)
{
    RectF rc = this->GetClientRect();
    if (m_bitmap) {
    }
    return true;
}

bool ImageSprite::SetImage(const wchar_t *path)
{

    return false;
}

int ImageSprite::SetImage(lua_State *L)
{
    return 0;
}

void ImageSprite::RecreateResouce(ID2D1RenderTarget *target)
{
    throw std::logic_error("The method or operation is not implemented.");
}

int ImageSprite::LuaConstructor(lua_State *L)
{
    ImageSprite *thiz = new ImageSprite;
    thiz->PushToLua(L, "ImageSprite");
    thiz->Release();
    return 1;
}


} //namespace ltk 