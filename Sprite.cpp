/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/

#include "StdAfx.h"
#include "Window.h"
#include "Sprite.h"
#include "ltk.h"
#include <cmath>

namespace ltk {

Sprite::Sprite(void)
{
	m_rect.X = 0;
	m_rect.Y = 0;
	m_rect.Width = 10;
	m_rect.Height = 10;

    m_window = NULL;

	m_firstChild = NULL;
	m_lastChild = NULL;
	m_prevSibling = NULL;
	m_nextSibling = NULL;
	m_parent = NULL;

	m_bVisible = true;
	m_enableFocus = false;
	m_bMouseIn = false;
	m_bClipChildren = false;
    m_bShowCaret = false;
}

Sprite::~Sprite(void)
{
	Sprite *sp = m_firstChild;
	while(sp)
	{
		Sprite *tmp = sp->m_nextSibling;
        sp->m_parent = nullptr;
        sp->Release(); // FIXME 有没有想过这里其实如果数量太多 会不会爆栈呢? 事实上是一个递归调用.
		sp = tmp;
	}
    m_window = INVALID_POINTER(Window);
    m_firstChild = INVALID_POINTER(Sprite);
    m_lastChild = INVALID_POINTER(Sprite);
    m_prevSibling = INVALID_POINTER(Sprite);
    m_nextSibling = INVALID_POINTER(Sprite);
    m_parent = INVALID_POINTER(Sprite);

    delete m_name;
    m_name = INVALID_POINTER(const char);
}

RectF Sprite::GetRect()
{
	return m_rect;
}

void Sprite::SetDelegate(RTTI *dlgt)
{
    m_delegate = dlgt;
}

void Sprite::DelegateEvent(Event *ev)
{
    if (m_delegate) {
        m_delegate->OnEvent(ev);
    }
}

void Sprite::SetName(const char *name)
{
    delete m_name;
    m_name = strdup(name);
}

const char * Sprite::GetName()
{
    return m_name;
}

RectF Sprite::GetClientRect()
{
	RectF rc = this->GetRect();
	rc.X = 0.0f;
	rc.Y = 0.0f;
	return rc;
}

RectF Sprite::GetAbsRect()
{
	Sprite *sp = m_parent;
	RectF rcSelf = GetRect();
	RectF rcParent;
	while(sp)
	{
		rcParent = sp->GetRect();
		rcSelf.Offset(rcParent.X, rcParent.Y);
		sp = sp->m_parent;
	}
	return rcSelf;
}

void Sprite::SetRect( RectF rect )
{
	// 检查下宽高是否小于0 是则设为0 然后0宽或0高要在OnDraw这些里面特殊处理一下
	rect.Width = max(0.0f, rect.Width);
	rect.Height = max(0.0f, rect.Height);
	if (!m_rect.Equals(rect))
	{
		RectF rcOld = m_rect;
		// TODO OnMove
		m_rect = rect;
		this->Invalidate(); // 刷新整个窗口
		// 原先回掉的顺序错了 导致在OnSize里面GetRect和OnSize的参数不一样 诡异错误
		if (rect.Width != rcOld.Width || rect.Height != rcOld.Height)
		{
            SizeEvent ev;
            ev.id = eSizeChanged;
            ev.width = rect.Width;
            ev.height = rect.Height;
            this->OnEvent(&ev);
		}
	}
}

void Sprite::Invalidate()
{
	// 0指针访问 不挂是因为x64系统一个bug 记得打开调试中的Win32异常断点
	Window *wnd = GetWindow();
    if (wnd && ::IsWindow(wnd->Handle()))
	{
		BOOL ret = ::InvalidateRect(wnd->Handle(), NULL, FALSE);
        LTK_ASSERT(ret);
	}
}

void Sprite::SetWindow( Window *wnd )
{
    //LTK_LOG("SetWindow: %08x", wnd);
	m_window = wnd;
}

void Sprite::SetWindowRecursive(Window *wnd)
{
    auto sp = m_firstChild;
    while (sp) {
        sp->SetWindowRecursive(wnd);
        sp->m_window = wnd;
        sp = sp->m_nextSibling;
    }
}

void Sprite::HandlePaint( ID2D1RenderTarget *target )
{
	if (!m_bVisible)
	{
		return; // 子节点也不会被绘制
	}
	// 前序遍历 让父节点先绘制
	//RectF rc = GetRect();
	//if (10 == rc.Width && 10 == rc.Height)
	//{
	//	LOGW(<<L"Orignal Size 10 10"); // 检查下有没有多余的重绘
	//}
/*
	if (m_bClipChildren)
	{
		RectF rcClip = GetRect();
		rcClip.X = 0.0f;
		rcClip.Y = 0.0f;
		g.SetClip(rcClip);
	}*/
    PaintEvent ev;
    ev.id = ePaint;
    ev.target = target;
    OnEvent(&ev);

	Sprite *sp = m_firstChild;
	while(sp)
	{
		RectF rc2 = sp->GetRect();

		TranslateTransform(target, rc2.X, rc2.Y);
		sp->HandlePaint(target);
		TranslateTransform(target, -rc2.X, -rc2.Y);

		sp = sp->m_nextSibling;
	}
/*
	if (m_bClipChildren)
	{
		g.ResetClip();
	}*/
}

void Sprite::AddChild( Sprite *sp )
{
	if (!m_firstChild)
	{
		assert(NULL == m_lastChild);
		m_firstChild = sp;
		m_lastChild = sp;
	}
	else
	{
		Sprite *tmp = m_lastChild;
		m_lastChild = sp;
		sp->m_prevSibling = tmp;
		tmp->m_nextSibling = sp;
	}
	sp->SetWindow(m_window);
	sp->m_parent = this;
    sp->AddRef();
}

void Sprite::TranslateMouseEvent( MouseEvent *ev )
{
	switch(ev->message)
	{
	case WM_MOUSEMOVE:
        ev->id = eMouseMove;
        OnEvent(ev);
		if (!m_bMouseIn)
		{
			m_bMouseIn = true;
            ev->id = eMouseEnter;
            OnEvent(ev);
		}
		break;
	case WM_MOUSEWHEEL:
        ev->id = eMouseWheel;
        OnEvent(ev);
		break;
	case WM_LBUTTONDOWN:
        ev->id = eLBtnDown;
        OnEvent(ev);
		if (m_enableFocus)
		{
			GetWindow()->SetFocusSprite(this);
		}
		break;
	case WM_LBUTTONUP:
        ev->id = eLBtnUp;
        OnEvent(ev);
        break;
	case WM_MOUSELEAVE:
		m_bMouseIn = false;
        ev->id = eMouseLeave;
        OnEvent(ev);
        break;
	}		
}

void Sprite::HandleCapturedMouseEvent( MouseEvent *ev)
{
	switch(ev->message)
	{
	case WM_MOUSEMOVE:
        ev->id = eMouseMove;
        OnEvent(ev);
		break;
	case WM_LBUTTONDOWN:
        ev->id = eLBtnDown;
        OnEvent(ev);
		break;
	case WM_LBUTTONUP:
        ev->id = eLBtnUp;
        OnEvent(ev);
        break;
	}			
}

void Sprite::HandleKeyEvent( UINT message, DWORD keyCode, DWORD flag )
{
	KeyEvent ev;
    ev.keyCode = keyCode;
    ev.flag = flag;

	switch(message)
	{
	case WM_KEYDOWN:
        ev.id = eKeyDown;
        OnEvent(&ev);
		break;
	case WM_KEYUP:
        ev.id = eKeyUp;
        OnEvent(&ev);
		break;
	case WM_CHAR:
        ev.id = eCharInput;
        OnEvent(&ev);
		break;
	}
}

void Sprite::HandleImeInput(LPCTSTR text)
{
    ImeEvent ev;
    ev.id = eImeInput;
    ev.text = text;
    OnEvent(&ev);
}

void Sprite::EnableFocus( bool enable )
{
	m_enableFocus = enable;
}

// return weak ref
Window * Sprite::GetWindow()
{
	Sprite *sp = this;
	while (sp->m_parent)
	{
		sp = sp->m_parent;
	}
	return sp->m_window;
}

void Sprite::SetCapture()
{
    auto wnd = GetWindow();
    assert(wnd); // 这个在lua包装函数里检查 报成lua错误
	if (wnd)
	{
		wnd->SetCapture(this);
	}
}

void Sprite::ReleaseCapture()
{
	assert(GetWindow()); // 这个在lua包装函数里检查 报成lua错误
	if (GetWindow())
	{
		GetWindow()->ReleaseCapture();
	}
}

void Sprite::BringToFront()
{
	if (m_parent)
	{
		if (this == m_parent->m_lastChild)
		{
			return;
		}
		// 
		if (m_prevSibling)
		{
			m_prevSibling->m_nextSibling = m_nextSibling;
		}
		else
		{
			m_parent->m_firstChild = m_nextSibling;
		}
		m_nextSibling->m_prevSibling = m_prevSibling; // 没有next说明是最后一个 而上面会跳过

		m_prevSibling = m_parent->m_lastChild;
		m_nextSibling = NULL;
		m_parent->m_lastChild->m_nextSibling = this;
		m_parent->m_lastChild = this;

		Invalidate();
	}
}

void Sprite::SetVisible( bool v )
{
	if (m_bVisible != v)
	{
		Invalidate();
	}
	m_bVisible = v;
}

bool Sprite::GetVisible()
{
	return m_bVisible;
}

void Sprite::SetClipChildren( bool bClip )
{
	if (m_bClipChildren != bClip)
	{
		m_bClipChildren = bClip;
		Invalidate();
	}
}

bool Sprite::GetClipChildren()
{
	return m_bClipChildren;
}

void Sprite::DispatchMouseEvent(MouseEvent *ev)
{
    this->TranslateMouseEvent(ev);
    Sprite *sp = m_firstChild;
    while (sp)
    {
        RectF rc = sp->GetRect();
        if (rc.Contains(ev->x, ev->y)) {
            MouseEvent ev2 = *ev;
            ev2.x -= rc.X;
            ev2.y -= rc.Y;
            sp->DispatchMouseEvent(&ev2);
        }
        sp = sp->m_nextSibling;
    }
}

// http://blog.csdn.net/magic_feng/article/details/6618206
Sprite * Sprite::DispatchMouseEvent2(MouseEvent *event)
{
	std::stack<Sprite *> stack;
	std::stack<Sprite *> reverse;
	stack.push(this);
	Sprite *sp = NULL;
	
	while(!stack.empty())
	{
		sp = stack.top();
		stack.pop();
		reverse.push(sp); // visit in reversed order.
		
		Sprite *sp2 = sp->m_lastChild;
		while (sp2)
		{
			stack.push(sp2);
			sp2 = sp2->m_prevSibling;
		}
	}
	while (!reverse.empty())
	{
		sp = reverse.top();
		reverse.pop();

		RectF rc = sp->GetAbsRect();
		//LOG("HitTest:"<< rc.X << "," <<rc.Y << "," << rc.Width << "," << rc.Height);
		if (rc.Contains(event->x, event->y))
		{
			MouseEvent e2 = *event;
			e2.x -= rc.X;
			e2.y -= rc.Y;
            sp->TranslateMouseEvent(&e2);
		}
	}
	return NULL;
}

Sprite * Sprite::GetAncestor()
{
	Sprite *sp = this;
	while (sp->m_parent)
	{
		sp = sp->m_parent;
	}
	return sp;
}

Sprite * Sprite::GetParent()
{
	return m_parent;
}

void Sprite::TrackMouseLeave()
{
	Window *wnd = GetWindow();
	if (wnd)
	{
		wnd->TrackMouseLeave(this);
	}
}

void Sprite::RemoveChild( Sprite *sp )
{
	assert(sp != this);
	assert(sp->m_parent == this);
	// 线性查找一下 是不是真的是子项目
#ifndef NDEBUG
	Sprite *f = m_firstChild;
	bool bFound = false;
	while(f)
	{
		if (f == sp)
		{
			bFound = true;
			break;
		}
		f = f->m_nextSibling;
	}
    assert(bFound);
    if (!bFound)
    {
        return;
    }
#endif
	if (sp == this->m_lastChild)
	{
		this->m_lastChild = sp->m_prevSibling;
	}
	if (sp == this->m_firstChild)
	{
		this->m_firstChild = sp->m_nextSibling;
	}
	if (sp->m_prevSibling)
	{
		sp->m_prevSibling->m_nextSibling = sp->m_nextSibling;
	}
	if (sp->m_nextSibling)
	{
		sp->m_nextSibling->m_prevSibling = sp->m_prevSibling;
	}
    sp->Release();
}

Sprite * Sprite::GetNextSprite()
{
	return m_nextSibling;
}



Sprite * Sprite::GetPrevSprite()
{
	return m_prevSibling;
}

Sprite * Sprite::GetFirstChild()
{
	return m_firstChild;
}

Sprite * Sprite::GetLastChild()
{
	return m_lastChild;
}

void Sprite::ShowCaret()
{
    GetWindow()->ShowCaret();
    m_bShowCaret = true;
}

void Sprite::SetCaretPos(RectF rc)
{
    RectF arc = Sprite::GetAbsRect();
    GetWindow()->SetImePosition(rc.X + arc.X, rc.Y + arc.Y);
    HWND hwnd = GetWindow()->Handle();
    ::DestroyCaret(); // 这里销毁重新建立 才能改变高度
    ::CreateCaret(hwnd, NULL, (int)rc.Width, (int)rc.Height); // 可以加个参数制定虚线光标(HBITMAP)1
    BOOL ret = ::ShowCaret(hwnd); // TODO 这里太脏了 可能显示出来就隐藏不掉 应该一对一绑定
    assert(ret);
    GetWindow()->SetCaretHeight(rc.Height);
    ::SetCaretPos((int)(rc.X + arc.X), (int)(rc.Y + arc.Y));
}

void Sprite::HideCaret()
{
    GetWindow()->HideCaret();
    m_bShowCaret = false;
}

void Sprite::OnEvent(Event *ev)
{
    bool bHandled = false;
    UINT id = ev->id;

    switch (id)
    {
    case ePaint:
        bHandled = OnPaint(static_cast<PaintEvent*>(ev));
        break;
    case eMouseMove:
        bHandled = OnMouseMove(static_cast<MouseEvent*>(ev));
        break;
    case eSizeChanged:
        bHandled = OnSize(static_cast<SizeEvent*>(ev));
        break;
    case eMouseEnter:
        bHandled = OnMouseEnter(static_cast<MouseEvent*>(ev));
        break;
    case eMouseLeave:
        bHandled = OnMouseLeave(static_cast<MouseEvent*>(ev));
        break;
    case eLBtnDown:
        bHandled = OnLBtnDown(static_cast<MouseEvent*>(ev));
        break;
    case eLBtnUp:
        bHandled = OnLBtnUp(static_cast<MouseEvent*>(ev));
        break;
    case eRBtnDown:
        bHandled = OnRBtnDown(static_cast<MouseEvent*>(ev));
        break;
    case eRBtnUp:
        bHandled = OnRBtnUp(static_cast<MouseEvent*>(ev));
        break;
    case eMouseWheel:
        bHandled = OnMouseWheel(static_cast<MouseEvent*>(ev));
        break;
    case eKeyDown:
        bHandled = OnKeyDown(static_cast<KeyEvent*>(ev));
        break;
    case eKeyUp:
        bHandled = OnKeyUp(static_cast<KeyEvent*>(ev));
        break;
    case eCharInput:
        bHandled = OnChar(static_cast<KeyEvent*>(ev));
        break;
    case eImeInput:
        bHandled = OnImeInput(static_cast<ImeEvent*>(ev));
        break;
    case eSetFocus:
        bHandled = OnSetFocus(static_cast<FocusEvent*>(ev));
        break;
    case eKillFocus:
        bHandled = OnKillFocus(static_cast<FocusEvent*>(ev));
        break;
    default:
        bHandled = false;
    }
}

void Sprite::HandleRecreateResouce(ID2D1RenderTarget *target)
{
    auto sp = m_firstChild;
    while (sp)
    {
        sp->HandleRecreateResouce(target);
        sp = sp->m_nextSibling;
    }
    this->RecreateResouce(target);
}

void Sprite::BeginAnimation()
{
    //LTK_LOG("BeginAnimation");
    Window *wnd = GetWindow();
    wnd->BeginAnimation(this);
}

void Sprite::EndAnimation()
{
    //LTK_LOG("EndAnimation");
    Window *wnd = GetWindow();
    if (wnd) {
        wnd->EndAnimation(this);
    }
}

#ifndef LTK_DISABLE_LUA

int Sprite::LuaConstructor(lua_State *L)
{
    Sprite *sp = new Sprite();
    sp->PushToLua(L, "Sprite");
    sp->Release();
    return 1;
}

int Sprite::AddChild(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<Sprite>(L, 1);
    Sprite *sp = CheckLuaObject<Sprite>(L, 2);
    thiz->AddChild(sp);
    return 0;
}


int Sprite::SetRect(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<Sprite>(L, 1);
    RectF rc = LuaCheckRectF(L, 2);
    thiz->SetRect(rc);
    return 0;
}

int Sprite::GetRect(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<Sprite>(L, 1);
    RectF rc = thiz->GetRect();
    lua_pushnumber(L, rc.X);
    lua_pushnumber(L, rc.Y);
    lua_pushnumber(L, rc.Width);
    lua_pushnumber(L, rc.Height);
    return 4;
}

int Sprite::GetWindow(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<Sprite>(L, 1);
    Window *wnd = thiz->GetWindow();
    wnd->PushToLua(L, wnd->TypeNameInstance().c_str());
    // wnd->Release(); // weak -_-
    return 1;
}

int Sprite::GetName(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<Sprite>(L, 1);
    lua_pushstring(L, thiz->GetName());
    return 1;
}

int Sprite::SetName(lua_State *L)
{
    Sprite *thiz = CheckLuaObject<Sprite>(L, 1);
    auto name = luaL_checkstring(L, 2);
    thiz->SetName(name);
    return 0;
}

#endif // LTK_DISABLE_LUA

} // namespace ltk