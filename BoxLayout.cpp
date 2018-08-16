#include "stdafx.h"
#include "BoxLayout.h"
#include "Sprite.h"

namespace ltk {

BoxLayout::BoxLayout(Mode m) :  m_mode(m)
{
}


BoxLayout::~BoxLayout()
{
    for (size_t i = 0; i < m_params.size(); i++) {
        m_params[i].item->Unref();
    }
    m_sprite = INVALID_POINTER(Sprite);
}

void BoxLayout::AddLayoutItem(LayoutItem *item, float preferedSize, float growFactor)
{
    if (m_sprite && item->Is(Sprite::TypeIdClass())) {
        m_sprite->AddChild(item->As<Sprite>());
    }
    if (item->Is(BoxLayout::TypeIdClass())) {
        item->As<BoxLayout>()->m_sprite = m_sprite;
    }
    item->Ref();
    BoxLayoutParam param;
    param.item = item;
    param.size = preferedSize;
    param.growFactor = growFactor;
    m_params.push_back(param);
}

void BoxLayout::SetRect(RectF rc)
{
    float sum_size = 0.0f;
    float sum_factor = 0.0f;
    for (size_t i = 0; i < m_params.size(); i++) {
        sum_size += m_params[i].size;
        sum_factor += m_params[i].growFactor;
    }
    sum_size += m_margin * (m_params.size() + 1);

    float remain;
    if (m_mode == Horizontal) {
        remain = rc.Width - sum_size;
    }
    else {
        remain = rc.Height - sum_size;
    }
    remain = max(0.0f, remain);
    float x = m_margin;
    float y = m_margin;
    for (size_t i = 0; i < m_params.size(); i++) {
        RectF rc2;
        float size;
        if (sum_factor > 0.0f) {
            size = m_params[i].size + remain * (m_params[i].growFactor / sum_factor);
        }
        else {
            size = m_params[i].size;
        }
        if (m_mode == Horizontal) {
            rc2.X = x;
            rc2.Y = y;
            rc2.Width = size;
            rc2.Height = rc.Height - m_margin - m_margin;
            x += size + m_margin;
        }
        else {
            rc2.X = x;
            rc2.Y = y;
            rc2.Width = rc.Width - m_margin - m_margin;
            rc2.Height = size;
            y += size + m_margin;
        }
        m_params[i].item->SetRect(rc2);
    }
}

void BoxLayout::SetMargin(float margin)
{
    if (margin <= 0.0f) {
        __debugbreak();
    }
    m_margin = margin;
}

void BoxLayout::SetSprite(Sprite *sp)
{
    m_sprite = sp; // child to parent weak ref
}

#ifndef LTK_DISABLE_LUA

int BoxLayout::LuaConstructor(lua_State *L)
{
    const char *options[] = {
        "horizontal", "vertical", nullptr
    };
    auto m = (Mode)luaL_checkoption(L, 2, nullptr, options);
    BoxLayout *box = new BoxLayout(m);
    box->PushToLua(L, "BoxLayout");
    box->Unref();
    return 1;
}

int BoxLayout::AddLayoutItem(lua_State *L)
{
    BoxLayout *thiz = CheckLuaObject<BoxLayout>(L, 1);
    LayoutItem *item = CheckLuaObject<LayoutItem>(L, 2);
    float size = (float)luaL_checknumber(L, 3);
    float grow = (float)luaL_optnumber(L, 4, 0.0f);
    thiz->AddLayoutItem(item, size, grow);
    return 0;
}

#endif // LTK_DISABLE_LUA

} // namespace ltk

