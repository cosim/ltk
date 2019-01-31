#pragma once
#include "Sprite.h"
#include "ltk.h"

namespace ltk {

class IconSprite : public Sprite
{
public:
    RTTI_DECLARATIONS(IconSprite, Sprite);
    IconSprite() {}
    ~IconSprite() {}

    virtual bool OnPaint(PaintEvent *ev) override;
    void SetIcon(const RectF &atlas, float scale, UINT idx);
    void SetIcon(const IconInfo &v);

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);
    static int SetIcon(lua_State *L);

    BEGIN_LUA_METHOD_MAP(IconSprite)
        LUA_CHAIN_METHOD_MAP(Sprite)
        LUA_METHOD_ENTRY(SetIcon)
    END_LUA_METHOD_MAP()
#endif // LTK_DISABLE_LUA

private:
    IconInfo m_info;
};

class TextureSprite : public Sprite
{
public:
    RTTI_DECLARATIONS(TextureSprite, Sprite);
    TextureSprite() {}
    ~TextureSprite() {}

    virtual bool OnPaint(PaintEvent *ev) override;
    void SetTexture(const RectF &atlas, const Margin &margin, float scale, UINT idx);

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);
    static int SetTexture(lua_State *L);

    BEGIN_LUA_METHOD_MAP(TextureSprite)
        LUA_CHAIN_METHOD_MAP(Sprite)
        LUA_METHOD_ENTRY(SetTexture)
    END_LUA_METHOD_MAP()
#endif // LTK_DISABLE_LUA

private:
    TextureInfo m_info;
};

class ImageSprite : public Sprite
{
public:
    RTTI_DECLARATIONS(ImageSprite, Sprite);
    ImageSprite() {}
    ~ImageSprite();

    virtual bool OnPaint(PaintEvent *ev) override;
    virtual void RecreateResouce(ID2D1RenderTarget *target);
    bool SetImage(const wchar_t *path);

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);
    static int SetImage(lua_State *L);

    BEGIN_LUA_METHOD_MAP(ImageSprite)
        LUA_CHAIN_METHOD_MAP(Sprite)
        LUA_METHOD_ENTRY(SetImage)
    END_LUA_METHOD_MAP()
#endif // LTK_DISABLE_LUA

private:
    ID2D1Bitmap *m_bitmap = nullptr;
    std::wstring m_path;
};

}
