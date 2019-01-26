#pragma once

#include "BoxLayout.h"

namespace ltk {

class IconSprite;
class Label;
class RadioGroup;

class CheckBox : public BoxLayout
{
public:
    RTTI_DECLARATIONS(CheckBox, BoxLayout);
    CheckBox();
    ~CheckBox();

    enum State{
        Checked,
        Unchecked,
        Indeterminate
    };

    void SetText(const wchar_t *t);
    void SetCheckState(State s);
    bool IsChecked();
    void SetRadioGroup(RadioGroup *group);

    virtual bool OnLBtnDown(MouseEvent *ev);
    virtual bool OnMouseEnter(MouseEvent *ev);
    virtual bool OnMouseLeave(MouseEvent *ev);

    static int LuaConstructor(lua_State *L);
    static int SetText(lua_State *L);
    static int IsChecked(lua_State *L);
    static int SetRadioGroup(lua_State *L);

    BEGIN_LUA_METHOD_MAP(CheckBox)
        LUA_CHAIN_METHOD_MAP(BoxLayout)
        LUA_METHOD_ENTRY(SetText)
        LUA_METHOD_ENTRY(IsChecked)
        LUA_METHOD_ENTRY(SetRadioGroup)
    END_LUA_METHOD_MAP()

private:
    void UpdateIcon();

private:
    IconSprite *m_box = nullptr;
    Label *m_label = nullptr;
    State m_state = Unchecked;
    RadioGroup *m_radioGroup = nullptr;
};

class RadioGroup : public LuaObject
{
public:
    RTTI_DECLARATIONS(RadioGroup, LuaObject);
    RadioGroup(){}
    ~RadioGroup(){}

    void Attach(CheckBox *);
    void Dettach(CheckBox *);
    void SetCheck(CheckBox *);

    static int LuaConstructor(lua_State *L);

    BEGIN_LUA_METHOD_MAP(RadioGroup)
    END_LUA_METHOD_MAP()

private:
    std::unordered_set<CheckBox *> m_setCheck;
};

} // namespace ltk
