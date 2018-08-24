#include "stdafx.h"
#include "StyleManager.h"

namespace ltk {

StyleManager::StyleManager()
{
    const D2D1_COLOR_F tbl[] = {
        { 0.9f, 0.9f, 0.9f },  // clrBackground1
        { 0.95f, 0.95f, 0.95f }, // clrBackground2
        { 0.7f, 0.7f, 0.7f }, // clrNormal
        { 0.7f, 0.7f, 0.9f }, // clrHover
        { 0.1f, 0.1f, 0.1f }, // clrText
        { 0.7f, 0.7f, 0.9f }, // clrHighlight
        { 0.7f, 0.7f, 0.9f }, // clrBorder
        { 0.95f, 0.95f, 0.95f }, // clrCaption
    };
    for (int i = 0; i < ARRAYSIZE(tbl); i ++) {
        m_colors.push_back(tbl[i]);
    }
}


StyleManager::~StyleManager()
{
}

StyleManager * StyleManager::Instance()
{
    if (!m_instance) {
        m_instance = new StyleManager;
    }
    return m_instance;
}

void StyleManager::Free()
{
    delete m_instance;
}

D2D1_COLOR_F StyleManager::GetColor(Colors clr)
{
    return m_colors.at((size_t)clr);
}

StyleManager * StyleManager::m_instance = nullptr;

}
