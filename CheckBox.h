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
    bool IsChecked();

    virtual bool OnLBtnDown(MouseEvent *ev);
    virtual bool OnMouseEnter(MouseEvent *ev);
    virtual bool OnMouseLeave(MouseEvent *ev);

    static int LuaConstructor(lua_State *L);
    static int SetText(lua_State *L);
    static int IsChecked(lua_State *L);

    BEGIN_LUA_METHOD_MAP(CheckBox)
        LUA_CHAIN_METHOD_MAP(BoxLayout)
        LUA_METHOD_ENTRY(SetText)
        LUA_METHOD_ENTRY(IsChecked)
    END_LUA_METHOD_MAP()

private:
    void UpdateIcon();

private:
    RefPtr<IconSprite> m_box;
    RefPtr<Label> m_label;
    bool m_checked = false;
};

} // namespace ltk
