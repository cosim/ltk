#pragma once

#include "LuaObject.h"

namespace ltk {

struct NinePatchInfo {
    RectF atlas;
    float left;
    float top;
    float right;
    float bottom;
    float textLeft;
    float textTop;
    float textRight;
    float textBottom;
};

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
        clrLast
    };
    D2D1_COLOR_F GetColor(Colors clr);

    static D2D1_COLOR_F ColorFromString(const char *psz);

    static int LuaConstructor(lua_State *L);
    static int SetColorScheme(lua_State *L);

    BEGIN_LUA_METHOD_MAP(StyleManager)
        LUA_METHOD_ENTRY(SetColorScheme)
    END_LUA_METHOD_MAP()

private:
    StyleManager();
    ~StyleManager();
    static StyleManager *m_instance;

    std::vector<D2D1_COLOR_F> m_colors;
};

}
