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
        if (m_params[i].item) {
            m_params[i].item->Unref();
        }
    }
    m_sprite = INVALID_POINTER(Sprite);
}

void BoxLayout::AddLayoutItem(Sprite *item, float preferedSize, float growFactor)
{
    Sprite::AddChild(item);
    item->Ref();
    BoxLayoutParam param;
    param.item = item;
    param.size = preferedSize;
    param.growFactor = growFactor;
    m_params.push_back(param);
}

void BoxLayout::AddSpaceItem(float preferedSize, float growFactor)
{
    BoxLayoutParam param;
    param.size = preferedSize;
    param.growFactor = growFactor;
    m_params.push_back(param);
}

bool BoxLayout::OnSize(SizeEvent *ev)
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
        remain = ev->width - sum_size;
    }
    else {
        remain = ev->height - sum_size;
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
            rc2.Height = ev->height - m_margin - m_margin;
            x += size + m_margin;
        }
        else {
            rc2.X = x;
            rc2.Y = y;
            rc2.Width = ev->width - m_margin - m_margin;
            rc2.Height = size;
            y += size + m_margin;
        }
        if (m_params[i].item) {
            m_params[i].item->SetRect(rc2);
        }
    }
    return true;
}

void BoxLayout::SetMargin(float margin)
{
    if (margin < 0.0f) {
        __debugbreak();
    }
    m_margin = margin;
}

void BoxLayout::DoLayout()
{
    RectF rc = this->GetRect();
    SizeEvent ev;
    ev.id = eSizeChanged;
    ev.width = rc.Width;
    ev.height = rc.Height;
    this->OnEvent(&ev);
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
    Sprite *item = CheckLuaObject<Sprite>(L, 2);
    float size = (float)luaL_checknumber(L, 3);
    float grow = (float)luaL_optnumber(L, 4, 0.0);
    thiz->AddLayoutItem(item, size, grow);
    return 0;
}

int BoxLayout::AddSpaceItem(lua_State *L)
{
    BoxLayout *thiz = CheckLuaObject<BoxLayout>(L, 1);
    float size = (float)luaL_checknumber(L, 2);
    float grow = (float)luaL_checknumber(L, 3);
    thiz->AddSpaceItem(size, grow);
    return 0;
}

int BoxLayout::DoLayout(lua_State *L)
{
    BoxLayout *thiz = CheckLuaObject<BoxLayout>(L, 1);
    thiz->DoLayout();
    return 0;
}

#endif // LTK_DISABLE_LUA

} // namespace ltk

