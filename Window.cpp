/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/

#include "StdAfx.h"
#include "Window.h"
#include "Common.h"
#include "Sprite.h"
#include "ltk.h"
#include "ApiBind.h"
#include "BoxLayout.h"
#include "Label.h"
#include "StyleManager.h"

namespace ltk {

const wchar_t * Window::ClsName = L"ltk_cls";
static const long SYSBTN_WIDTH = 35;
static const long SYSBTN_HEIGHT = 25;
static const long CAPTION_HEIGHT = 25;
static const long SYSICON_SIZE = 24;
static const long WINDOW_BORDER = 6;

Window::Window(void) : 
    m_shadowLeft(ShadowFrame::eLeft),
    m_shadowTop(ShadowFrame::eTop),
    m_shadowRight(ShadowFrame::eRight),
    m_shadowBottom(ShadowFrame::eBottom)
{
	m_hwnd = NULL;

	m_rectComposition.left = 0;
	m_rectComposition.top = 0;
	m_rectComposition.right = 5;
	m_rectComposition.bottom = 20;

    m_sprite = new BoxLayout(BoxLayout::Vertical);
    m_sprite->SetMargin(0.0f);
    m_sprite->SetWindow(this);
    
	m_spFocus = NULL;
	m_spCapture = NULL;
	m_spHover = NULL;

	m_caretHeight = 20;
}

Window::~Window(void)
{
    if (m_sprite) {
        m_sprite->Unref();
    }
    m_sprite = INVALID_POINTER(BoxLayout);
    if (m_hboxCaption) {
        m_hboxCaption->Unref();
    }
    m_hboxCaption = INVALID_POINTER(BoxLayout);

    m_spFocus = INVALID_POINTER(Sprite);
    m_spCapture = INVALID_POINTER(Sprite);
    m_spHover = INVALID_POINTER(Sprite);

    if (m_target) {
        m_target->Release();
    }
    m_target = INVALID_POINTER(ID2D1HwndRenderTarget);

    delete m_resizable;
    m_resizable = INVALID_POINTER(ResizeHelper);

    if (m_brush) {
        m_brush->Release();
    }
    m_brush = INVALID_POINTER(ID2D1SolidColorBrush);

    if (m_btnClose) {
        m_btnClose->Unref();
    }
    m_btnClose = INVALID_POINTER(Button);

    if (m_btnMinimize) {
        m_btnMinimize->Unref();
    }
    m_btnMinimize = INVALID_POINTER(Button);

    if (m_btnMaximize) {
        m_btnMaximize->Unref();
    }
    m_btnMaximize = INVALID_POINTER(Button);

    if (m_labelTitle) {
        m_labelTitle->Unref();
    }
    m_labelTitle = INVALID_POINTER(Label);
}

void Window::Create(Window *parent, RectF rc, Mode mode)
{
    m_mode = mode;
    HWND hParent = NULL;
    if (!parent)
    {
        hParent = ::GetDesktopWindow();
    }
    else
    {
        hParent = parent->m_hwnd;
    }
    DWORD style = WS_VISIBLE;
    
    switch (m_mode)
    {
    case ltk::Window::eOverlaped:
        style |= WS_OVERLAPPEDWINDOW;
        break;
    case ltk::Window::eBorderless:
        style |= WS_POPUP;
        m_resizable = new ResizeHelper;

        m_hboxCaption = new BoxLayout(BoxLayout::Horizontal);
        m_hboxCaption->SetMargin(0.0f);
        m_hboxCaption->SetSpacing(0.0f);
        m_hboxCaption->AddSpaceItem(7.0f, 0.0f);

        m_labelTitle = new Label;
        m_labelTitle->SetTextAlign(DWRITE_TEXT_ALIGNMENT_LEADING);
        m_hboxCaption->AddLayoutItem(m_labelTitle, 0.0f, 1.0f);

        m_btnMinimize = new Button;
        m_btnMinimize->SetText(L"_");
        m_btnMinimize->Clicked.Attach(std::bind(&Window::OnBtnMinimizeClicked, this));
        m_btnMinimize->SetNormalColor(StyleManager::Instance()->GetColor(StyleManager::clrCaption));
        m_hboxCaption->AddLayoutItem(m_btnMinimize, (float)SYSBTN_WIDTH);

        m_btnMaximize = new Button;
        m_btnMaximize->SetText(L"口");
        m_btnMaximize->Clicked.Attach(std::bind(&Window::OnBtnMaximizeClicked, this));
        m_btnMaximize->SetNormalColor(StyleManager::Instance()->GetColor(StyleManager::clrCaption));
        m_hboxCaption->AddLayoutItem(m_btnMaximize, (float)SYSBTN_WIDTH);

        m_btnClose = new Button();
        m_btnClose->SetText(L"X");
        m_btnClose->Clicked.Attach(std::bind(&Window::OnBtnCloseClicked, this));
        m_btnClose->SetNormalColor(StyleManager::Instance()->GetColor(StyleManager::clrCaption));
        m_btnClose->SetHoverColor(D2D1::ColorF(1.0f, 0.2f, 0.2f));
        m_hboxCaption->AddLayoutItem(m_btnClose, (float)SYSBTN_WIDTH);

        m_sprite->AddLayoutItem(m_hboxCaption, (float)CAPTION_HEIGHT);

        m_shadowLeft.Create();
        m_shadowTop.Create();
        m_shadowRight.Create();
        m_shadowBottom.Create();
        break;
    default:
        break;
    }
    style |=  WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

    ::CreateWindowEx(0, ClsName, L"", style,
        (int)rc.X, (int)rc.Y, (int)rc.Width, (int)rc.Height,
        hParent, NULL, HINST_THISCOMPONENT, this);
}

RectF Window::GetRect()
{
    RectF rf;
    RECT rc;
    ::GetWindowRect(m_hwnd, &rc);
    rf.X = (float)rc.left;
    rf.Y = (float)rc.top;
    rf.Width = (float)(rc.right - rc.left);
    rf.Height = (float)(rc.bottom - rc.top);
    return rf;
}

void Window::SetRect(RectF rc)
{
    ::MoveWindow(m_hwnd, (int)rc.X, (int)rc.Y, (int)rc.Width, (int)rc.Height, TRUE);
}

void Window::SetTitle(const wchar_t *title)
{
    ::SetWindowText(m_hwnd, title);
    if (m_labelTitle) {
        m_labelTitle->SetText(title);
    }
}

SizeF Window::GetClientSize()
{
    RECT rc;
    ::GetClientRect(m_hwnd, &rc);
    return SizeF((float)rc.right - rc.left, (float)rc.bottom - rc.top);
}

// TODO 记得窗口过程 里面一定要先push HWND message wparam lparam 到lua堆栈里
// 然后再 pcall 一个cfunction 再去解析参数 不然一旦lua报错 就找不到错误在哪 而且程序会挂掉

void Window::RegisterWndClass()
{
	WNDCLASS wc;
	wc.style         = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = HINST_THISCOMPONENT;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;// (HBRUSH)(COLOR_WINDOW + 1); // TODO 改成NULL 防止系统去擦除背景(闪烁) 双缓冲 还有clip children clip sibling
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = ClsName;

	ATOM a = RegisterClass(&wc);
    assert(a);
}

void Window::HandleMouseMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	MouseEvent event;
	event.message = message;
	event.flag = LOWORD(wparam);

	if (WM_MOUSEWHEEL == message)
	{
		POINT pt;
		pt.x = (short)LOWORD(lparam);
		pt.y = (short)HIWORD(lparam);
		::ScreenToClient(m_hwnd, &pt);
		event.x = (float)pt.x;
		event.y = (float)pt.y;
		event.delta = (float)(short)HIWORD(wparam) / 120.0F;
        //LOG("WM_MOUSEWHEEL: " << event.x << " " << event.y);
	}
	else
	{
		event.x = (float)(short)LOWORD(lparam); //先切2字节 然后转有符号 再转浮点
		event.y = (float)(short)HIWORD(lparam);
		event.delta = 0.0f;
	}
    if (WM_LBUTTONDBLCLK == message)
    {
        ::PostMessage(m_hwnd, WM_LBUTTONDOWN, wparam, lparam);
    }

	if (m_spCapture)
	{
		RectF rc = m_spCapture->GetAbsRect();
		event.x -= rc.X;
		event.y -= rc.Y;
		m_spCapture->HandleCapturedMouseEvent(&event);
	}
	else if (m_sprite)
	{
		//if (WM_LBUTTONDOWN == message)
		//{
			m_sprite->DispatchMouseEvent(&event);
			
            // TODO copy to vector first (pre-allocate)
			std::vector<Sprite *> defer_remove;
			for (std::unordered_set<Sprite *>::iterator iter = m_setTrackMouseLeave.begin();
				iter != m_setTrackMouseLeave.end(); ++iter)
			{
				Sprite *sp = *iter;
				RectF rc = sp->GetAbsRect();
				if (!rc.Contains(event.x, event.y))
				{
					MouseEvent e2 = event;
					e2.message = WM_MOUSELEAVE;
					sp->HandleMouseEvent(&e2);
					defer_remove.push_back(sp);
					// Fire the event and remove sp from the set;
				}
			}
			for (std::vector<Sprite *>::iterator iter = defer_remove.begin();
				iter != defer_remove.end(); ++iter)
			{
				Sprite *sp = *iter;
				m_setTrackMouseLeave.erase(sp);
			}
		//}
	}
}

void Window::HandleMouseLeave()
{
    for (auto iter = m_setTrackMouseLeave.begin(); iter != m_setTrackMouseLeave.end(); ++iter)
    {
        Sprite *sp = *iter;
        MouseEvent e2;
        e2.id = eMouseLeave;
        e2.delta = 0;
        e2.flag = 0;
        e2.x = 0;
        e2.y = 0;
        e2.message = WM_MOUSELEAVE;
        sp->HandleMouseEvent(&e2);
        // Fire the event and remove sp from the set;
    }
    m_setTrackMouseLeave.clear();
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	Window *thiz;
    lua_State *L = GetGlobalLuaState();

	if (WM_NCCREATE == message)
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lparam);
        thiz = reinterpret_cast<Window*>(lpcs->lpCreateParams);
		thiz->m_hwnd = hwnd;
		SetWindowLongPtr(hwnd, GWLP_USERDATA,
			reinterpret_cast<LPARAM>(thiz));
	}
	else if (WM_GETMINMAXINFO == message)
	{
        // TODO FIXME this cause nc hit test not working.
        return ::DefWindowProc(hwnd, message, wparam, lparam);
	} else {
        thiz = reinterpret_cast<Window *>
            (GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
	assert(NULL != thiz);

    bool bHandled = false;
    
    if (thiz->m_resizable) {
        LRESULT ret = thiz->m_resizable->HandleMessage(hwnd, message, wparam, lparam, bHandled);
        if (bHandled) {
            return ret;
        }
    }

	switch(message)
	{
	case WM_PAINT:
		thiz->OnPaint(hwnd);
		break;
	case WM_SYNCPAINT:
		// 上面有窗口拖动的时候 会有1像素宽度和1像素高度的区域 不停的发 而WM_PAINT又是低优先级的
		// 导致有的消息被丢弃了 所以才会有白条
		// 解决在win7 没开毛玻璃 设置窗口拖动时显示内容 窗口上面的窗口被拖动 画面会花
		::InvalidateRect(hwnd, NULL, TRUE);
		//LOG("WM_SYNCPAINT");
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
		thiz->HandleMouseMessage(message, wparam, lparam);
		break;
	case WM_ERASEBKGND:
		return TRUE;
    case WM_TIMER:
        if (wparam == TIMER_ANIMATION)
        {
            static int cnt = 0;
            cnt++;
            if (cnt % 100 == 0){
                LOG("TIMER_ANIMATION " << cnt);
            }
            ::InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
	case WM_SIZE:
        do
        {
            UINT cx = LOWORD(lparam);
            UINT cy = HIWORD(lparam);

            if (thiz->m_target)
            {
                thiz->m_target->Resize(D2D1::SizeU(cx, cy));
            }
            thiz->OnSize((float)cx, (float)cy, (DWORD)wparam);
            thiz->UpdateShadowFrame(true);
        } while (0);
		return 0;
    case WM_MOVE:
        thiz->UpdateShadowFrame(false);
        break;
    case WM_ACTIVATE:
        if (LOWORD(wparam)) {
            thiz->UpdateShadowFrame(false);
        }
        break;
        // TODO hide shadow when maximized or hide main window.
    case WM_MOUSELEAVE:
        thiz->HandleMouseLeave();
        break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		if (thiz->m_spFocus)
		{
			thiz->m_spFocus->HandleKeyEvent(message, (DWORD)wparam, (DWORD)lparam);
		}
		break;
	case WM_IME_SETCONTEXT:
	case WM_IME_STARTCOMPOSITION:
	case WM_IME_COMPOSITION:
	case WM_IME_ENDCOMPOSITION:
	case WM_INPUTLANGCHANGE:
		return thiz->OnImeEvent(message, wparam, lparam);
	case WM_SETFOCUS:
		LOG("WM_SETFOCUS");
		::CreateCaret(hwnd, NULL, 1, thiz->m_caretHeight);
		//::ShowCaret(hwnd);
		if (thiz->m_spFocus)
		{
            Event ev;
            ev.id = eSetFocus;
            thiz->m_spFocus->OnEvent(&ev);
		}
		return 0;
	case WM_KILLFOCUS:
		LOG("WM_KILLFOCUS");
		::DestroyCaret();
		if (thiz->m_spFocus)
		{
            Event ev;
            ev.id = eKillFocus;
            thiz->m_spFocus->OnEvent(&ev);
        }
		return 0;
	case WM_CREATE:
		//do 
		//{
		//	RECT rc;
		//	::GetClientRect(hwnd, &rc);
		//	::PostMessage(hwnd, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom)); // TODO 这里如果创建出来的是全屏的 第2个参数怎么办呢
		//} while (0);
		return 0;
	case WM_CLOSE:
        do
        {
            bool proceed = false;
            thiz->OnClose(proceed);
            if (proceed)
            {
                ::DestroyWindow(hwnd);
            }
        } while (0);
        return 0;
	case WM_DESTROY:
        thiz->OnDestroy();
        thiz->CallEventHandler(L, "OnDestroy", 0, 0);
		return 0;
	}
    return ::DefWindowProc(hwnd, message, wparam, lparam);
}

void Window::DrawNonClient()
{
    SizeF size = this->GetClientSize();
    if (m_mode == Window::eBorderless) {
        m_brush->SetColor(StyleManager::Instance()->GetColor(StyleManager::clrCaption));
        m_target->FillRectangle(D2D1::RectF(0.0f, 0.0f, size.Width, (float)CAPTION_HEIGHT), m_brush);
    }
    m_brush->SetColor(StyleManager::Instance()->GetColor(StyleManager::clrBorder));
    DrawRectSnapped(m_target, RectF(0.0f, 0.0f, size.Width - 1.0f, size.Height - 1.0f), m_brush);
    // debug
    //m_target->DrawRectangle(D2D1::RectF(20, 400, 60, 440), m_brush);
}

void Window::RecreateResouce()
{
    HRESULT hr = E_FAIL;
    SAFE_RELEASE(m_brush);
    hr = m_target->CreateSolidColorBrush(D2D1::ColorF(0.5f, 0.5f, 0.5f), &m_brush);
    assert(SUCCEEDED(hr));
}

void Window::OnPaint(HWND hwnd )
{
	PAINTSTRUCT ps;
	HDC hdc = ::BeginPaint(hwnd, &ps);
    HRESULT hr = E_FAIL;

    if (!m_target)
    {
        RECT rc;
        ::GetClientRect(hwnd, &rc);

        hr = GetD2DFactory()->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(
            hwnd, D2D1::SizeU(rc.right, rc.bottom)), &m_target);
        assert(SUCCEEDED(hr));

        if (m_sprite)
        {
            m_sprite->HandleRecreateResouce(m_target);
        }
        this->RecreateResouce();
    }

    m_target->BeginDraw();
    m_target->SetTransform(D2D1::Matrix3x2F::Identity());
    //TranslateTransform(m_target, 0.5f, 0.5f);
    m_target->Clear(StyleManager::Instance()->GetColor(StyleManager::clrBackground1));
    this->DrawNonClient();

    if (m_sprite)
    {
        RectF rc = m_sprite->GetRect();
        TranslateTransform(m_target, rc.X, rc.Y);
        m_sprite->HandlePaint(m_target);
        TranslateTransform(m_target, -rc.X, -rc.Y);
    }

    hr = m_target->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        SAFE_RELEASE(m_target);
    }
    ::EndPaint(hwnd, &ps);
}

bool Window::OnSize(float cx, float cy, DWORD flag)
{
    //if (m_mode == eBorderless) {
    //    m_btnClose->SetRect(RectF((float)(cx - SYSBTN_WIDTH - 2), (float)(1.0f - CAPTION_HEIGHT),
    //        (float)SYSBTN_WIDTH, (float)(CAPTION_HEIGHT - 1)));
    //}
    m_sprite->SetRect(RectF(1.0f, 1.0f, (float)(cx - 2.0f), (float)(cy - 1.0f)));
    return false;
}

HWND Window::Handle()
{
	return m_hwnd;
}

LRESULT Window::OnImeEvent( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    // TODO 加个开关 能够关闭输入法 当焦点不在输入框上的时候就关闭
	switch(uMsg)
	{
	case WM_IME_SETCONTEXT:
		do 
		{
			LOG(<<"WM_IME_SETCONTEXT");
			BOOL handled = FALSE;
			m_ime.CreateImeWindow(m_hwnd);
			m_ime.CleanupComposition(m_hwnd);
			m_ime.SetImeWindowStyle(m_hwnd, uMsg, wParam, lParam, &handled);
		} while (0);
		return 0;
	case WM_IME_STARTCOMPOSITION:
		LOG(<<"WM_IME_STARTCOMPOSITION");
		m_ime.CreateImeWindow(m_hwnd);
		m_ime.ResetComposition(m_hwnd);
		return 0;
	case WM_IME_COMPOSITION:
		do
		{
			LOG(<<"WM_IME_COMPOSITION");
			ImeComposition comp;
			m_ime.UpdateImeWindow(m_hwnd);
			m_ime.GetResult(m_hwnd, lParam, &comp);
			if (GCS_RESULTSTR == comp.string_type)
			{
				wstring tmp = comp.ime_string; // 这里结尾居然多出个0 可能是改了谷歌的代码导致的问题
				tmp.resize(wcslen(tmp.c_str()));
				LOGW(<<tmp);
				OnImeInput(tmp.c_str());
				//m_text += tmp;
				//::InvalidateRect(GetHWND(), NULL, TRUE);
			}
			m_ime.ResetComposition(m_hwnd);
			m_ime.EnableIME(m_hwnd, m_rectComposition, false); // 输入窗口跟随
		}while(0);
		return 0;
	case WM_IME_ENDCOMPOSITION:
		LOG(<<"WM_IME_ENDCOMPOSITION");
		m_ime.ResetComposition(m_hwnd);
		m_ime.DestroyImeWindow(m_hwnd);
		//::ShowCaret(m_hwnd);
		::DefWindowProc(m_hwnd, uMsg, wParam, lParam);
		return 0;
	case WM_INPUTLANGCHANGE:
		LOG(<<"WM_INPUTLANGCHANGE");
		m_ime.SetInputLanguage();
		::DefWindowProc(m_hwnd, uMsg, wParam, lParam);
		return 0;
	case WM_KEYDOWN:
		return 0;
	}
	return 0;
}

void Window::SetImePosition( float x, float y )
{
	m_rectComposition.left = (int)x;
	m_rectComposition.right = (int)x + 5;
	m_rectComposition.top = (int)y;
	m_rectComposition.bottom = (int)y + 20;
}

void Window::SetFocusSprite( Sprite *sp )
{
	if (m_spFocus == sp)
	{
		return;
	}
	if (m_spFocus)
	{
		Event ev;
        ev.id = eKillFocus;
        m_spFocus->OnEvent(&ev);
	}
	m_spFocus = sp;
	if (m_spFocus)
	{
		Event ev;
        ev.id = eSetFocus;
        m_spFocus->OnEvent(&ev);
	}
}

Sprite *Window::GetFocusSprite()
{
	return m_spFocus;
}

void Window::OnImeInput( PCTSTR text )
{
	if (m_spFocus)
	{
		// 其他的Sprite也有可能去接受ime消息。比如再来一个RichEdit
		m_spFocus->HandleImeInput(text);
	}
}

void Window::SetCaretHeight( float h)
{
	m_caretHeight = (int)h;
}

void Window::SetCapture( Sprite *sp )
{
	assert(sp->GetWindow() == this);
	m_spCapture = sp;
	::SetCapture(m_hwnd);
}

void Window::ReleaseCapture()
{
	m_spCapture = NULL;
	::ReleaseCapture();
}

void Window::ShowCaret()
{
	LOG(" ");
	::ShowCaret(m_hwnd);
}

void Window::HideCaret()
{
	LOG(" ");
	::HideCaret(m_hwnd);
}

void Window::TrackMouseLeave( Sprite *sp )
{
	if (sp->GetWindow() == this)
	{
		m_setTrackMouseLeave.insert(sp);
	}
	// Track for the HWND
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_hwnd;
    tme.dwFlags = TME_LEAVE;
    tme.dwHoverTime = HOVER_DEFAULT;
    ::TrackMouseEvent(&tme);
}

void Window::BeginAnimation(Sprite *sp)
{
    if (m_setAnimation.size() == 0)
    {
        ::SetTimer(m_hwnd, TIMER_ANIMATION, 0, NULL);
    }
    m_setAnimation.insert(sp);
}

void Window::EndAnimation(Sprite *sp)
{
    if (m_setAnimation.size() == 0)
    {
        return;
    }
    auto iter = m_setAnimation.find(sp);
    if (iter != m_setAnimation.end())
    {
        m_setAnimation.erase(iter);
        if (m_setAnimation.size() == 0)
        {
            ::KillTimer(m_hwnd, TIMER_ANIMATION);
        }
    }
}

void Window::OnBtnCloseClicked()
{
    //::CloseWindow(m_hwnd); // MSDN: Minimizes (but does not destroy) the specified window. WTF??
    ::SendMessage(m_hwnd, WM_CLOSE, 0, 0);
}

void Window::OnBtnMinimizeClicked()
{
    ::ShowWindow(m_hwnd, SW_MINIMIZE);
}

void Window::OnBtnMaximizeClicked()
{
    if (m_resizable) {
        m_resizable->Maximize();
    }
}

void Window::UpdateShadowFrame(bool bRedraw)
{
    if (m_mode == Mode::eBorderless) {
        m_shadowLeft.Update(m_hwnd, bRedraw);
        m_shadowTop.Update(m_hwnd, bRedraw);
        m_shadowRight.Update(m_hwnd, bRedraw);
        m_shadowBottom.Update(m_hwnd, bRedraw);
    }
}

#ifndef LTK_DISABLE_LUA

class DtorTest
{
public:
    ~DtorTest() { LOG("..."); }
};

int Window::LuaConstructor(lua_State *L)
{
    Window *wnd = new Window();
    wnd->PushToLua(L, "Window");
    wnd->Unref();
    return 1;
}

int Window::Create(lua_State *L)
{
    DtorTest test;
    auto thiz = CheckLuaObject<Window>(L, 1);
    auto rc = LuaCheckRectF(L, 2);
    auto mode = luaL_checkstring(L, 3);
    auto m = eOverlaped;

    if (strcmp(mode, "borderless") == 0) {
        m = eBorderless;
    }
    else if (strcmp(mode, "overlapped") == 0){
        m = eOverlaped;
    }
    else {
        luaL_error(L, "InvalideArgs #3 mode: borderless|overlapped");
    }

    thiz->Create(nullptr, rc, m);
    return 0;
}

int Window::GetRect(lua_State *L)
{
    auto thiz = CheckLuaObject<Window>(L, 1);
    auto rc = thiz->GetRect();
    LuaPushRectF(L, rc);
    return 1;
}

int Window::GetClientSize(lua_State *L)
{
    auto thiz = CheckLuaObject<Window>(L, 1);
    SizeF size = thiz->GetClientSize();
    lua_pushnumber(L, size.Width);
    lua_pushnumber(L, size.Height);
    return 2;
}

int Window::SetTitle(lua_State *L)
{
    auto thiz = CheckLuaObject<Window>(L, 1);
    auto title = LuaCheckWString(L, 2);
    thiz->SetTitle(title);
    return 0;
}

int Window::GetRootSprite(lua_State *L)
{
    auto thiz = CheckLuaObject<Window>(L, 1);
    thiz->m_sprite->PushToLua(L, "BoxLayout");
    return 1;
}

#endif

void ResizeHelper::SetWindowRect(HWND hwnd, RECT &rc)
{
    if (rc.right - rc.left < m_minWidth) {
        switch (m_state) {
        case eLeftTop:
        case eLeft:
        case eLeftBottom:
            rc.left = rc.right - m_minWidth;
            break;
        case eRightTop:
        case eRight:
        case eRightBottom:
            rc.right = rc.left + m_minWidth;
            break;
        }
    }
    if (rc.bottom - rc.top < m_minHeight) {
        switch (m_state) {
        case eLeftTop:
        case eTop:
        case eRightTop:
            rc.top = rc.bottom - m_minHeight;
            break;
        case eLeftBottom:
        case eBottom:
        case eRightBottom:
            rc.bottom = rc.top + m_minHeight;
            break;
        }
    }
    ::MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
}

LRESULT ResizeHelper::HandleMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, bool &bHandled)
{
    // all coords here are in screen space.
    RECT rc;
    POINT pt;
    BOOL ret;
    State st;
    WINDOWPLACEMENT wp = { 0 };
    wp.length = sizeof(wp);
    ::GetWindowPlacement(hwnd, &wp);
    if ((wp.showCmd == SW_MAXIMIZE || m_bMaximized) && message != WM_LBUTTONDBLCLK) {
        return 0;
    }

    switch (message) {
    case WM_CREATE:
        m_hwnd = hwnd;
        bHandled = false;
        break;
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
        ret = ::GetCursorPos(&pt);
        assert(ret);
        ::GetWindowRect(hwnd, &rc);

        switch(message) {
        case WM_LBUTTONDOWN:
            m_oldPoint = pt;
            m_oldRect = rc;

            m_state = StateFromPoint(pt, rc);
            if (m_state != eNone) {
                bHandled = true;
                ::SetCapture(hwnd);
            }
            break;
        case WM_MOUSEMOVE:
            if (m_state == eNone) {
                st = StateFromPoint(pt, rc);
            }
            else {
                st = m_state;
            }
            switch (st) {
            case eLeftTop:
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENWSE)));
                break;
            case eLeft:
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
                break;
            case eLeftBottom:
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENESW)));
                break;
            case eRightTop:
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENESW)));
                break;
            case eRight:
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
                break;
            case eRightBottom:
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENWSE)));
                break;
            case eTop:
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
                break;
            case eBottom:
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
                break;
            }
            if (st != eNone && st != eMove) {
                bHandled = true;
            }
            switch (m_state) {
            case eMove:
                ::SetWindowPos(hwnd, NULL, 
                    pt.x - m_oldPoint.x + m_oldRect.left,
                    pt.y - m_oldPoint.y + m_oldRect.top,
                    0, 0, SWP_NOSIZE);
                bHandled = true;
                return 0;
            case eLeftTop:
                rc.left = pt.x - m_oldPoint.x + m_oldRect.left;
                rc.top = pt.y - m_oldPoint.y + m_oldRect.top;
                break;
            case eLeft:
                rc.left = pt.x - m_oldPoint.x + m_oldRect.left;
                break;
            case eLeftBottom:
                rc.left = pt.x - m_oldPoint.x + m_oldRect.left;
                rc.bottom = pt.y - m_oldPoint.y + m_oldRect.bottom;
                break;

            case eRightTop:
                rc.right = pt.x - m_oldPoint.x + m_oldRect.right;
                rc.top = pt.y - m_oldPoint.y + m_oldRect.top;
                break;
            case eRight:
                rc.right = pt.x - m_oldPoint.x + m_oldRect.right;
                break;
            case eRightBottom:
                rc.right = pt.x - m_oldPoint.x + m_oldRect.right;
                rc.bottom = pt.y - m_oldPoint.y + m_oldRect.bottom;
                break;
            case eTop:
                rc.top = pt.y - m_oldPoint.y + m_oldRect.top;
                break;
            case eBottom:
                rc.bottom = pt.y - m_oldPoint.y + m_oldRect.bottom;
                break;
            }
            if (m_state != eNone) {
                SetWindowRect(hwnd, rc);
                bHandled = true;
                return 0;
            }
            break;
        case WM_LBUTTONUP:
            if (m_state != eNone) {
                ::ReleaseCapture();
                bHandled = true;
                m_state = eNone;
            }
            break;
        case WM_LBUTTONDBLCLK:
            if (pt.x < rc.right - SYSBTN_WIDTH * 3 - 5 && pt.y < rc.top + CAPTION_HEIGHT) {
                if (wp.showCmd == SW_MAXIMIZE) {
                    wp.showCmd = SW_SHOWNORMAL;
                    ::SetWindowPlacement(hwnd, &wp);
                }
                else if (m_bMaximized) {
                    SetWindowRect(hwnd, m_normalRect);
                    m_bMaximized = false;
                }
                else {
                    ::GetWindowRect(hwnd, &m_normalRect);
                    HMONITOR mon = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
                    MONITORINFO info = { 0 };
                    info.cbSize = sizeof(info);
                    ::GetMonitorInfoW(mon, &info);
                    SetWindowRect(hwnd, info.rcWork);
                    m_bMaximized = true;
                }
            }
            break;
        }
        break;
    }
    return 0;
}

ResizeHelper::State ResizeHelper::StateFromPoint(POINT pt, const RECT &rc)
{
    if (pt.x < rc.left + WINDOW_BORDER) {
        if (pt.y < rc.top + WINDOW_BORDER) {
            return eLeftTop;
        } 
        else if (pt.y > rc.bottom - WINDOW_BORDER) {
            return eLeftBottom;
        }
        else {
            return eLeft;
        }
    }
    else if (pt.x > rc.right - WINDOW_BORDER) {
        if (pt.y < rc.top + WINDOW_BORDER) {
            return eRightTop;
        } 
        else if (pt.y > rc.bottom - WINDOW_BORDER) {
            return eRightBottom;
        } 
        else {
            return eRight;
        }
    }
    else if (pt.y < rc.top + WINDOW_BORDER) {
        return eTop;
    }
    else if (pt.y > rc.bottom - WINDOW_BORDER) {
        return eBottom;
    }
    else if (pt.x < rc.right - SYSBTN_WIDTH * 3 - 5 && pt.y < rc.top + CAPTION_HEIGHT) {
        return eMove;
    }
    return eNone;
}

void ResizeHelper::Maximize()
{
    if (m_bMaximized) {
        SetWindowRect(m_hwnd, m_normalRect);
        m_bMaximized = false;
    }
    else {
        POINT pt;
        ::GetCursorPos(&pt);
        LTK_ASSERT(::IsWindow(m_hwnd));
        ::GetWindowRect(m_hwnd, &m_normalRect);
        HMONITOR mon = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        MONITORINFO info = { 0 };
        info.cbSize = sizeof(info);
        ::GetMonitorInfoW(mon, &info);
        SetWindowRect(m_hwnd, info.rcWork);
        m_bMaximized = true;
    }
}

} // namespace ltk