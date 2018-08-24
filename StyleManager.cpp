#include "stdafx.h"
#include "StyleManager.h"

namespace ltk {

StyleManager::StyleManager()
{
    const D2D1_COLOR_F tbl[] = {
        { 0.9f, 0.9f, 0.9f, 1.0f },  // clrBackground1
        { 0.95f, 0.95f, 0.95f, 1.0f }, // clrBackground2
        { 0.8f, 0.8f, 0.85f, 1.0f }, // clrNormal
        { 0.7f, 0.7f, 1.0f, 1.0f }, // clrHover
        { 0.1f, 0.1f, 0.1f, 1.0f }, // clrText
        { 0.7f, 0.7f, 0.9f, 1.0f }, // clrHighlight
        { 0.7f, 0.7f, 0.9f, 1.0f }, // clrBorder
        { 0.95f, 0.95f, 0.95f, 1.0f }, // clrCaption
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
