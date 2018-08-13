/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/
#pragma once
#include "stdafx.h"
#include "ImeInput.h"
#include "LuaObject.h"
#include "Button.h"

namespace ltk {

class Sprite;
class Button;
class ResizeHelper;

class Window : public LuaObject
{
protected:
    virtual ~Window(void);

public:
    RTTI_DECLARATIONS(Window, LuaObject)

    Window(void);

    void SetRect(RectF rc);

    enum Mode
    {
        eOverlaped,
        eBorderless
    };

    void Create(Window *parent, RectF rc, Mode mode);

	static void RegisterWndClass();

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void OnPaint( HWND hwnd );

	HWND Handle();

	void AttachSprite(Sprite *sp);

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

#ifndef LTK_DISABLE_LUA
    static int LuaConstructor(lua_State *L);
    static int Create(lua_State *L);
    static int AttachSprite(lua_State *L);

    BEGIN_LUA_METHOD_MAP(Window)
        LUA_METHOD_ENTRY(Create)
        LUA_METHOD_ENTRY(AttachSprite)
    END_LUA_METHOD_MAP()
#endif

private:
	void HandleMouseMessage(UINT message, WPARAM wparam, LPARAM lparam);
    void HandleMouseLeave();
    LRESULT OnImeEvent(UINT message, WPARAM wparam, LPARAM lparam);
    enum { TIMER_ANIMATION = 100 };
    static const wchar_t * ClsName;

private:
	HWND m_hwnd;
    Mode m_mode = eOverlaped;
	ImeInput m_ime;
	RECT m_rectComposition;
	int m_caretHeight;
	Sprite *m_sprite; // owner
	Sprite *m_spFocus;
	Sprite *m_spCapture;
	Sprite *m_spHover;
	std::unordered_set<Sprite *> m_setTrackMouseLeave;
    std::unordered_set<Sprite *> m_setAnimation;
    ID2D1HwndRenderTarget *m_target = nullptr;
    Button *m_btnClose = nullptr;
    ResizeHelper *m_resizable = nullptr;
};

class ResizeHelper
{
public:
    LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, bool &bHandled);

private:
    POINT m_oldPoint;
    RECT m_oldRect;
    RECT m_normalRect;
    bool m_bMaximized = false;

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
    State m_state = eNone;
};

} // namespace ltk