#pragma once
#include "LuaObject.h"

namespace ltk {

class Sprite;

class LayoutItem : public LuaObject
{
public:
    RTTI_DECLARATIONS(LayoutItem, LuaObject)
    virtual void SetRect(RectF rc) {}
    virtual LayoutItem *GetChildLayout(size_t i) { return nullptr; }
};

struct BoxLayoutParam {
    LayoutItem *item = nullptr;
    float size = 20.0f;
    float growFactor = 0.0f;
};

class SpaceItem : public LayoutItem
{
public:
    virtual void SetRect(RectF rc) {}
};

class BoxLayout : public LayoutItem
{
public:
    RTTI_DECLARATIONS(BoxLayout, LayoutItem)
    enum Mode {
        Horizontal, Vertical
    };
    BoxLayout(Mode m);
    virtual ~BoxLayout();

    virtual void SetRect(RectF rc) override;

    void SetMargin(float margin);

    void AddLayoutItem(LayoutItem *sp, float preferedSize, float growFactor = 0.0f);

    void SetSprite(Sprite *sp);

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);
    static int AddLayoutItem(lua_State *L);

    BEGIN_LUA_METHOD_MAP(BoxLayout)
        LUA_METHOD_ENTRY(AddLayoutItem)
    END_LUA_METHOD_MAP()

#endif // LTK_DISABLE_LUA

private:
    std::vector<BoxLayoutParam> m_params; // item: owner
    Sprite *m_sprite = nullptr; // weak
    Mode m_mode;
    float m_margin = 5.0f;
};

} // namespace ltk
