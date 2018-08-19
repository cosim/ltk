#include "stdafx.h"
#include "Label.h"
#include "ltk.h"

namespace ltk {

Label::Label()
{
    m_textColor = D2D1::ColorF(D2D1::ColorF::White);
}

Label::~Label()
{
    if (m_brush) {
        m_brush->Release();
    }
    m_brush = INVALID_POINTER(ID2D1SolidColorBrush);

    if (m_textFormat) {
        m_textFormat->Release();
    }
    m_textFormat = INVALID_POINTER(IDWriteTextFormat);
}

bool Label::OnPaint(PaintEvent *ev)
{
    HRESULT hr = E_FAIL;
    RectF rc = this->GetClientRect();
    if (!m_textFormat) {
        hr = GetDWriteFactory()->CreateTextFormat(
            L"Î¢ÈíÑÅºÚ",
            NULL,
            m_fontWeight,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            m_fontSize,
            L"zh-cn",
            &m_textFormat
            );
        assert(SUCCEEDED(hr));
        LOG("recreate m_textFormat");
    }
    m_textFormat->SetTextAlignment(m_textAlign);
    m_textFormat->SetParagraphAlignment(m_paragraphAlign);
    m_brush->SetColor(m_textColor);
    ev->target->DrawText(m_text.c_str(), (UINT32)m_text.length(), m_textFormat, ltk::D2D1RectF(rc), m_brush);
    return true;
}

void Label::RecreateResouce(ID2D1RenderTarget *target)
{
    LOG(".");
    HRESULT hr = E_FAIL;
    SAFE_RELEASE(m_brush);
    hr = target->CreateSolidColorBrush(m_textColor, &m_brush);
    assert(SUCCEEDED(hr));
}

void Label::SetText(const wchar_t *t)
{
    m_text = t;
    this->Invalidate();
}

void Label::SetTextAlign(DWRITE_TEXT_ALIGNMENT a)
{
    m_textAlign = a;
    this->Invalidate();
}

void Label::SetParagraphAlign(DWRITE_PARAGRAPH_ALIGNMENT a)
{
    m_paragraphAlign = a;
    this->Invalidate();
}

void Label::SetTextColor(D2D1_COLOR_F clr)
{
    m_textColor = clr;
    this->Invalidate();
}

void Label::SetFontSize(float size)
{
    m_fontSize = size;
    SAFE_RELEASE(m_textFormat);
    this->Invalidate();
}

void Label::SetFontWeight(DWRITE_FONT_WEIGHT weight)
{
    m_fontWeight = weight;
    SAFE_RELEASE(m_textFormat);
    this->Invalidate();
}

void Label::SetFontName(const wchar_t *name)
{
    m_fontName = name;
    SAFE_RELEASE(m_textFormat);
    this->Invalidate();
}

#ifndef LTK_DISABLE_LUA

int Label::LuaConstructor(lua_State *L)
{
    auto text = LuaCheckWString(L, 2);
    auto thiz = new Label();
    thiz->SetText(text);
    thiz->PushToLua(L, "Label");
    thiz->Unref();
    return 1;
}

int Label::SetText(lua_State *L)
{
    Label *thiz = CheckLuaObject<Label>(L, 1);
    auto text = LuaCheckWString(L, 2);
    thiz->SetText(text);
    return 0;
}

int Label::SetTextAlign(lua_State *L)
{
    Label *thiz = CheckLuaObject<Label>(L, 1);
    const char *options1[] = {
        "leading", "trailing", "center", "justified", nullptr
    };
    thiz->SetTextAlign((DWRITE_TEXT_ALIGNMENT)luaL_checkoption(L, 2, nullptr, options1));
    return 0;
}

int Label::SetParagraphAlign(lua_State *L)
{
    Label *thiz = CheckLuaObject<Label>(L, 1);
    const char *options2[] = {
        "near", "far", "bottom", nullptr
    };
    thiz->SetParagraphAlign((DWRITE_PARAGRAPH_ALIGNMENT)luaL_checkoption(L, 2, nullptr, options2));
    return 0;
}

int Label::SetTextColor(lua_State *L)
{
    Label *thiz = CheckLuaObject<Label>(L, 1);
    auto clr = LuaCheckColor(L, 2);
    thiz->SetTextColor(clr);
    return 0;
}

int Label::SetFontSize(lua_State *L)
{
    Label *thiz = CheckLuaObject<Label>(L, 1);
    thiz->SetFontSize((float)luaL_checknumber(L, 2));
    return 0;
}

int Label::SetFontWeight(lua_State *L)
{
    Label *thiz = CheckLuaObject<Label>(L, 1);
    const char *options1[] = {
        "regular", "bold", nullptr
    };
    const DWRITE_FONT_WEIGHT enums[] = {
        DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_WEIGHT_BOLD
    };
    int id = luaL_checkoption(L, 2, nullptr, options1);
    thiz->SetFontWeight(enums[id]);
    return 0;
}

int Label::SetFontName(lua_State *L)
{
    Label *thiz = CheckLuaObject<Label>(L, 1);
    auto name = LuaCheckWString(L, 2);
    thiz->SetFontName(name);
    return 0;
}

#endif // LTK_DISABLE_LUA

} // namespace ltk
