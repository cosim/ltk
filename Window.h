/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/
#pragma once
#include "stdafx.h"
#include "ImeInput.h"

namespace cs {

class Sprite;

class Window
{
public:
    Window(void);
    virtual ~Window(void);

    void SetRect(Gdiplus::RectF rc);

    void Create(Window *parent, Gdiplus::RectF rc, DWORD style, DWORD exStyle);

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

    virtual bool OnSize(float cx, float cy, DWORD flag) { return false; }
    virtual bool OnClose(bool &proceed) { proceed = true; return true; }
    virtual bool OnDestroy() { return false; }

private:
	void HandleMouseMessage(UINT message, WPARAM wparam, LPARAM lparam);
	LRESULT OnImeEvent(UINT message, WPARAM wparam, LPARAM lparam);
    enum { TIMER_ANIMATION = 100 };
private:
	DWORD m_style;
	DWORD m_exStyle;
	Gdiplus::RectF m_rect;
	HWND m_hwnd;
	ImeInput m_ime;
	RECT m_rectComposition;
	int m_caretHeight;
	unique_ptr<Sprite> m_sprite;
	Sprite *m_spFocus;
	Sprite *m_spCapture;
	Sprite *m_spHover;
	std::unordered_set<Sprite *> m_setTrackMouseLeave;
    std::unordered_set<Sprite *> m_setAnimation;
    ID2D1HwndRenderTarget *m_target = nullptr;
};

} // namespace cs