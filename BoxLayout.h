#pragma once
#include "LuaObject.h"
#include "Sprite.h"

namespace ltk {

struct BoxLayoutParam {
    Sprite *item = nullptr;
    float size = 20.0f;
    float growFactor = 0.0f;
};

class SpaceItem : public Sprite
{
public:
};

class BoxLayout : public Sprite
{
public:
    RTTI_DECLARATIONS(BoxLayout, Sprite)
    enum Mode {
        Horizontal, Vertical
    };
    BoxLayout(Mode m);
    virtual ~BoxLayout();

    virtual bool OnSize(SizeEvent *ev) override;

    void SetMargin(float margin);

    void AddLayoutItem(Sprite *sp, float preferedSize, float growFactor = 0.0f);

    void AddSpaceItem(float preferedSize, float growFactor);

    void DoLayout();

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);
    static int AddLayoutItem(lua_State *L);
    static int AddSpaceItem(lua_State *L);
    static int DoLayout(lua_State *L);

    BEGIN_LUA_METHOD_MAP(BoxLayout)
        LUA_CHAIN_METHOD_MAP(Sprite)
        LUA_METHOD_ENTRY(AddLayoutItem)
        LUA_METHOD_ENTRY(AddSpaceItem)
        LUA_METHOD_ENTRY(DoLayout)
    END_LUA_METHOD_MAP()

#endif // LTK_DISABLE_LUA

private:
    std::vector<BoxLayoutParam> m_params; // item: owner
    Sprite *m_sprite = nullptr; // weak
    Mode m_mode;
    float m_margin = 5.0f;
};

} // namespace ltk
