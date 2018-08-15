#include "stdafx.h"
#include "BoxLayout.h"
#include "Sprite.h"

namespace ltk {

BoxLayout::BoxLayout()
{
}


BoxLayout::~BoxLayout()
{
}

void BoxLayout::AddItem(LayoutItem *item, float preferedSize)
{
    if (m_sprite && item->Is(Sprite::TypeIdClass())) {
        m_sprite->AddChild(item->As<Sprite>());
    }
    BoxLayoutParam param;
    param.item = item;
    param.size = preferedSize;
    m_params.push_back(param);
}

void BoxLayout::SetRect(RectF rc)
{
    int cnt = 0;
    for (size_t i = 0; i < m_params.size(); i ++) {
        if (m_params[i].size == 0.0f) {
            cnt++;
        }
    }
    float sum_size = 0.0f;
    for (size_t i = 0; i < m_params.size(); i++) {
        sum_size += m_params[i].size;
    }
}

} // namespace ltk
