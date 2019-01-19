#pragma once

#include "BoxLayout.h"

namespace ltk {

class ImageSprite;
class Label;

class CheckBox : public BoxLayout
{
public:
    RTTI_DECLARATIONS(CheckBox, BoxLayout);
    CheckBox();
    ~CheckBox();

    void SetText(const wchar_t *t);

private:
    RefPtr<ImageSprite> m_box;
    RefPtr<Label> m_label;
};

} // namespace ltk
