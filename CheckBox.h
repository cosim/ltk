#pragma once

#include "BoxLayout.h"

namespace ltk {

class IconSprite;
class Label;

class CheckBox : public BoxLayout
{
public:
    RTTI_DECLARATIONS(CheckBox, BoxLayout);
    CheckBox();
    ~CheckBox();

    void SetText(const wchar_t *t);

private:
    RefPtr<IconSprite> m_box;
    RefPtr<Label> m_label;
};

} // namespace ltk
