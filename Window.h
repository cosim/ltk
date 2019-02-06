/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/
#pragma once
#include "stdafx.h"
#include "ImeInput.h"
#include "LuaObject.h"
#include "ShadowFrame.h"

namespace ltk {

class Sprite;
class Button;
class BoxLayout;
class Label;

class Window : public LuaObject
{
protected:
    virtual ~Window(void);

public:
    RTTI_DECLARATIONS(Window, LuaObject)

    Window(void);

    virtual void OnEvent(Event *ev) override;

    void Create(Window *parent, RectF rc);

    void SetRect(RectF rc);

    RectF GetRect();

    SizeF GetClientSize();

    void SetTitle(const wchar_t *title);

	static void RegisterWndClass();

    LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static LRESULT CALLBACK WndProcStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void OnPaint( HWND hwnd );

	HWND Handle();

	void OnImeInput(LPCTSTR text);

	void SetImePosition( float x, float y );

	void SetFocusSprite(Sprite *sp);

	Sprite *GetFocusSprite();

	void SetCaretHeight(float);

	void ShowCaret();

	void HideCaret();

	void SetCapture(Sprite *sp);

	void ReleaseCapture();

	void TrackMouseLeave(Sprite *sp);

    void BeginAnimation(Sprite *sp);
    void EndAnimation(Sprite *sp);

    virtual bool OnSize(float cx, float cy, DWORD flag);
    virtual bool OnClose(bool &proceed) { proceed = true; return true; }
    virtual bool OnDestroy() { return false; }

    void OnBtnCloseClicked();
    void OnBtnMinimizeClicked();
    void OnBtnMaximizeClicked();

    ID2D1Bitmap *GetAtlasBitmap();
    void SetBackground(const TextureInfo &info);

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);
    static int Create(lua_State *L);
    static int GetRect(lua_State *L);
    static int GetClientSize(lua_State *L);
    static int SetTitle(lua_State *L);
    static int GetRootSprite(lua_State *L);
    static int SetBackground(lua_State *L);

    BEGIN_LUA_METHOD_MAP(Window)
        LUA_METHOD_ENTRY(Create)
        LUA_METHOD_ENTRY(GetRect)
        LUA_METHOD_ENTRY(GetClientSize)
        LUA_METHOD_ENTRY(SetTitle)
        LUA_METHOD_ENTRY(GetRootSprite)
        LUA_METHOD_ENTRY(SetBackground)
    END_LUA_METHOD_MAP()
#endif // LTK_DISABLE_LUA

private:
	void HandleMouseMessage(UINT message, WPARAM wparam, LPARAM lparam);
    void HandleMouseLeave();
    void DrawNonClient();
    void RecreateResouce();
    void UpdateShadowFrame(bool bRedraw);

    LRESULT OnImeEvent(UINT message, WPARAM wparam, LPARAM lparam);
    LRESULT HandleNcHitTest(const POINT &pt);
    enum { TIMER_ANIMATION = 100 };
    static const wchar_t * ClsName;

private:
	HWND m_hwnd = NULL;

	ImeInput m_ime;
	RECT m_rectComposition;
	int m_caretHeight;

	BoxLayout *m_sprite = nullptr; // owner
    Sprite *m_spFocus = nullptr;
    Sprite *m_spCapture = nullptr;
    Sprite *m_spHover = nullptr;
	std::unordered_set<Sprite *> m_setTrackMouseLeave;
    std::unordered_set<Sprite *> m_setAnimation;

    BoxLayout *m_hboxCaption = nullptr;
    Button *m_btnClose = nullptr;
    Button *m_btnMaximize = nullptr;
    Button *m_btnMinimize = nullptr;
    Label *m_labelTitle = nullptr;

    ID2D1HwndRenderTarget *m_target = nullptr; // owner
    ID2D1SolidColorBrush *m_brush = nullptr; // owner

    ShadowFrame m_shadowLeft;
    ShadowFrame m_shadowTop;
    ShadowFrame m_shadowRight;
    ShadowFrame m_shadowBottom;

    ID2D1Bitmap *m_atlas = nullptr;
    TextureInfo m_background;
};

} // namespace ltk