#pragma once

#include "Sprite.h"

namespace ltk {

class Button;

class ScrollBar : public Sprite
{
public:
    enum Mode {
        Horizontal, Vertical
    };
    ScrollBar(Mode mode);
    ~ScrollBar();

    void Update();

    static const float SLIDER_MIN;

private:
    Mode m_mode;
    Button *m_slider = nullptr;
    float m_contentSize = 100.0f;
};

}