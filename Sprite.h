/*
** Sprite.h The core element of LuaUI2
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/
#pragma once
#include "Common.h"
#include "Window.h"
#include "Event.h"

namespace ltk {

class Sprite
{
public:
    Sprite(void);
    virtual ~Sprite(void);

	Gdiplus::RectF GetRect();

	void SetRect( Gdiplus::RectF rect );

	Gdiplus::RectF GetAbsRect();

	void Invalidate();

	void SetHostWnd( Window *wnd ); // TODO ��������Ҫ�� ��������Ҳ������
	Window *GetHostWindow();

	void SetVisible( bool );

	bool GetVisible();

	void AddChild( Sprite *sp );

    void HandlePaint( ID2D1RenderTarget *target );

    void HandleMouseEvent(MouseEvent *ev);

	void HandleCapturedMouseEvent( MouseEvent *ev );

	void HandleKeyEvent( UINT message, DWORD keyCode, DWORD flag );

	void HandleImeInput( LPCTSTR text );

    void HandleRecreateResouce( ID2D1RenderTarget *target );

	void EnableFocus(bool enable);

	void SetCapture();

	void ReleaseCapture();

	void BringToFront();

	void SetClipChildren(bool bClip);

	bool GetClipChildren();

	void TrackMouseLeave();

	Sprite *DispatchMouseEvent(MouseEvent *ev);

	void RemoveChild(Sprite *sp);

	Sprite *GetAncestor();

	Sprite *GetParent();

	Sprite *GetNextSprite();

	Sprite *GetPrevSprite();

	Sprite *GetFirstChild();

	Sprite *GetLastChild();

    void ShowCaret();
    void SetCaretPos(Gdiplus::RectF rc);
    void HideCaret();

    void BeginAnimation();
    void EndAnimation();

    virtual bool OnEvent(Event *ev);

    virtual bool OnPaint            (PaintEvent *ev) { return false; }
    virtual bool OnLBtnDown         (MouseEvent *ev) { return false; }
    virtual bool OnLBtnUp           (MouseEvent *ev) { return false; }
    virtual bool OnLBtnDbClk        (MouseEvent *ev) { return false; }
    virtual bool OnRBtnDown         (MouseEvent *ev) { return false; }
    virtual bool OnRBtnUp           (MouseEvent *ev) { return false; }
    virtual bool OnMouseMove        (MouseEvent *ev) { return false; }
    virtual bool OnMouseEnter       (MouseEvent *ev) { return false; }
    virtual bool OnMouseLeave       (MouseEvent *ev) { return false; }
    virtual bool OnMouseWheel       (MouseEvent *ev) { return false; }
    virtual bool OnKeyDown          (KeyEvent *ev)   { return false; }
    virtual bool OnKeyUp            (KeyEvent *ev)   { return false; }
    virtual bool OnChar             (KeyEvent *ev)   { return false; }
    virtual bool OnSize             (SizeEvent *ev)  { return false; }
    virtual bool OnImeInput         (ImeEvent *ev)   { return false; }
    virtual bool OnSetFocus         (FocusEvent *ev) { return false; }
    virtual bool OnKillFocus        (FocusEvent *ev) { return false; }

    virtual void RecreateResouce(ID2D1RenderTarget *target){}

private:
	bool m_bVisible; // ���������Զ��Ż���1�ֽ�
	bool m_bMouseIn;
	bool m_enableFocus;
	bool m_bClipChildren;
    bool m_bShowCaret;

    Gdiplus::RectF m_rect;
    Window *m_hostWnd;

	Sprite *m_prevSibling; // �ؼ����ṹ Ҫ�ߴ���һ�� zorder���������ṹ�� ��������
	Sprite *m_nextSibling; // ƽ���ĵĹ�ϵ��˫����
	Sprite *m_firstChild;
	Sprite *m_lastChild;
	Sprite *m_parent;

	DISALLOW_COPY_AND_ASSIGN(Sprite);
};

} // namespace ltk