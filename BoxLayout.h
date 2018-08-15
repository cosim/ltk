#pragma once
#include "LuaObject.h"

namespace ltk {

class Sprite;

class LayoutItem : public LuaObject
{
public:
    virtual void SetRect(RectF rc) {}
};

struct BoxLayoutParam {
    LayoutItem *item = nullptr;
    float size = 20.0f;
};

class BoxLayout : public LayoutItem
{
public:
    BoxLayout();
    virtual ~BoxLayout();

    virtual void SetRect(RectF rc) override;

    void AddItem(LayoutItem *sp, float preferedSize);

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);

    BEGIN_LUA_METHOD_MAP(BoxLayout)

    END_LUA_METHOD_MAP()

#endif // LTK_DISABLE_LUA

private:
    std::vector<BoxLayoutParam> m_params;
    Sprite *m_sprite = nullptr;
};

} // namespace ltk
