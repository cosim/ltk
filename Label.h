#pragma once
#include "Sprite.h"

namespace ltk {

class Label : public Sprite
{
public:
    RTTI_DECLARATIONS(Label, Sprite)
    Label();
    virtual ~Label();
    virtual bool OnPaint(PaintEvent *ev);
    virtual void RecreateResouce(ID2D1RenderTarget *target);

    void SetText(const wchar_t *t);
    void SetTextAlign(DWRITE_TEXT_ALIGNMENT);
    void SetParagraphAlign(DWRITE_PARAGRAPH_ALIGNMENT);
    void SetTextColor(D2D1_COLOR_F);

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);
    static int SetText(lua_State *L);
    static int SetTextAlign(lua_State *L);
    static int SetParagraphAlign(lua_State *L);
    static int SetTextColor(lua_State *L);
    static int SetFontSize(lua_State *L);

    BEGIN_LUA_METHOD_MAP(Label)
        LUA_CHAIN_METHOD_MAP(Sprite)
        LUA_METHOD_ENTRY(SetText)
        LUA_METHOD_ENTRY(SetTextAlign)
        LUA_METHOD_ENTRY(SetParagraphAlign)
        LUA_METHOD_ENTRY(SetTextColor)
        LUA_METHOD_ENTRY(SetFontSize)
    END_LUA_METHOD_MAP()
#endif // LTK_DISABLE_LUA

private:
    ID2D1SolidColorBrush *m_brush = nullptr; // owner
    IDWriteTextFormat *m_textFormat = nullptr; // owner
    std::wstring m_text;
    DWRITE_TEXT_ALIGNMENT m_textAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
    DWRITE_PARAGRAPH_ALIGNMENT m_paragraphAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
    D2D1_COLOR_F m_textColor;
    float m_fontSize = 12.0f;
};

} // namespace ltk
