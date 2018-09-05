/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/
#pragma once
#include "stdafx.h"
#include "ImeInput.h"
#include "LuaObject.h"
#include "Button.h"
#include "ShadowFrame.h"

namespace ltk {

class Sprite;
class Button;
class ResizeHelper;
class BoxLayout;
class Label;

class Window : public LuaObject
{
protected:
    virtual ~Window(void);

public:
    RTTI_DECLARATIONS(Window, LuaObject)

    Window(void);

    enum Mode
    {
        eOverlaped,
        eBorderless
    };

    void Create(Window *parent, RectF rc, Mode mode);

    void SetRect(RectF rc);

    RectF GetRect();

    SizeF GetClientSize();

    void SetTitle(const wchar_t *title);

	static void RegisterWndClass();

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

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

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);
    static int Create(lua_State *L);
    static int GetRect(lua_State *L);
    static int GetClientSize(lua_State *L);
    static int SetTitle(lua_State *L);
    static int GetRootSprite(lua_State *L);

    BEGIN_LUA_METHOD_MAP(Window)
        LUA_METHOD_ENTRY(Create)
        LUA_METHOD_ENTRY(GetRect)
        LUA_METHOD_ENTRY(GetClientSize)
        LUA_METHOD_ENTRY(SetTitle)
        LUA_METHOD_ENTRY(GetRootSprite)
    END_LUA_METHOD_MAP()
#endif // LTK_DISABLE_LUA

private:
	void HandleMouseMessage(UINT message, WPARAM wparam, LPARAM lparam);
    void HandleMouseLeave();
    void DrawNonClient();
    void RecreateResouce();
    void UpdateShadowFrame(bool bRedraw);

    LRESULT OnImeEvent(UINT message, WPARAM wparam, LPARAM lparam);
    enum { TIMER_ANIMATION = 100 };
    static const wchar_t * ClsName;

private:
	HWND m_hwnd;
    Mode m_mode = eOverlaped;

	ImeInput m_ime;
	RECT m_rectComposition;
	int m_caretHeight;

	BoxLayout *m_sprite; // owner
	Sprite *m_spFocus;
	Sprite *m_spCapture;
	Sprite *m_spHover;
	std::unordered_set<Sprite *> m_setTrackMouseLeave;
    std::unordered_set<Sprite *> m_setAnimation;

    ResizeHelper *m_resizable = nullptr; // owner

    BoxLayout *m_hboxCaption = nullptr;
    Button *m_btnClose = nullptr;
    Button *m_btnMaximize = nullptr;
    Button *m_btnMinimize = nullptr;
    Label *m_labelTitle = nullptr;

    ID2D1HwndRenderTarget *m_target = nullptr; // owner
    ID2D1SolidColorBrush *m_brush = nullptr; // owner
};

class ResizeHelper
{
public:
    ResizeHelper();
    void UpdateShadowFrame(bool bRedraw);
    LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, bool &bHandled);
    void Maximize();

private:
    POINT m_oldPoint;
    RECT m_oldRect;
    RECT m_normalRect;
    bool m_bMaximized = false;
    long m_minWidth = 400;
    long m_minHeight = 300;
    ShadowFrame m_shadowLeft;
    ShadowFrame m_shadowTop;
    ShadowFrame m_shadowRight;
    ShadowFrame m_shadowBottom;

    enum State {
        eNone,
        eMove,
        eLeftTop,
        eLeft,
        eLeftBottom,
        eRightTop,
        eRight,
        eRightBottom,
        eTop,
        eBottom
    };
    State StateFromPoint(POINT pt, const RECT &rc);
    void SetWindowRect(HWND hwnd, RECT &rc);
    State m_state = eNone;
    HWND m_hwnd = 0;
};

} // namespace ltk