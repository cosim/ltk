#pragma once

#include "LuaObject.h"

namespace ltk {

class StyleManager : public LuaObject
{
public:
    RTTI_DECLARATIONS(StyleManager, LuaObject);
    static StyleManager *Instance();
    static void Free();

    enum Colors {
        clrBackground1,
        clrBackground2,
        clrNormal,
        clrHover,
        clrText,
        clrHighlight,
        clrBorder,
        clrCaption,
    };
    D2D1_COLOR_F GetColor(Colors clr);

private:
    StyleManager();
    ~StyleManager();
    static StyleManager *m_instance;

    std::vector<D2D1_COLOR_F> m_colors;
};

}
