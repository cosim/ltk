/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/

#include "StdAfx.h"
#include "Window.h"
#include "Common.h"
#include "Sprite.h"
#include "ltk.h"
#include "ApiBind.h"

namespace ltk {

const wchar_t * Window::ClsName = L"ltk_cls";
static const float SYSBTN_WIDTH = 40;
static const float SYSBTN_HEIGHT = 30;
static const float CAPTION_HEIGHT = 25;
static const float SYSICON_SIZE = 24;
static const float WINDOW_BORDER = 4;

Window::Window(void)
{
	m_hwnd = NULL;

	m_rectComposition.left = 0;
	m_rectComposition.top = 0;
	m_rectComposition.right = 5;
	m_rectComposition.bottom = 20;

    m_sprite = new Sprite();
    m_sprite->SetHostWnd(this);

    m_btnClose = new Button();
    m_btnClose->SetText(L"X");
    m_btnClose->Clicked.Attach(std::bind(&Window::OnBtnCloseClicked, this));
    m_sprite->AddChild(m_btnClose);
    
	m_spFocus = NULL;
	m_spCapture = NULL;
	m_spHover = NULL;

	m_caretHeight = 20;

    m_resizable = new ResizeHelper;
}

Window::~Window(void)
{
    if (m_sprite) {
        m_sprite->Unref();
    }
    m_sprite = INVALID_POINTER(Sprite);
    m_spFocus = INVALID_POINTER(Sprite);
    m_spCapture = INVALID_POINTER(Sprite);
    m_spHover = INVALID_POINTER(Sprite);

    if (m_target) {
        m_target->Release();
    }
    m_target = INVALID_POINTER(ID2D1HwndRenderTarget);

    delete m_resizable;
    m_resizable = INVALID_POINTER(ResizeHelper);
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
        style |= WS_OVERLAPPED;
        break;
    case ltk::Window::eBorderless:
        style |= WS_POPUP;
        break;
    default:
        break;
    }
    style |=  WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

    ::CreateWindowEx(0, ClsName, L"", style,
        (int)rc.X, (int)rc.Y, (int)rc.Width, (int)rc.Height,
        hParent, NULL, HINST_THISCOMPONENT, this);
}

void Window::SetRect(RectF rc)
{
    ::MoveWindow(m_hwnd, (int)rc.X, (int)rc.Y, (int)rc.Width, (int)rc.Height, TRUE);
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
    if (WM_LBUTTONDOWN == message)
    {
        //LOG("WM_LBUTTONDOWN: " << event.x << " " << event.y);
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

LRESULT Window::HandleNcHitTest(float x, float y)
{
    if (x < 0 || y < 0) {
        return HTNOWHERE;
    }
    RECT rc;
    ::GetClientRect(m_hwnd, &rc);
    float cx = (float)(rc.right - rc.left);
    float cy = (float)(rc.bottom - rc.top);

    if (x < WINDOW_BORDER) {
        if (y < WINDOW_BORDER) {
            return HTTOPLEFT;
        }
    }

    if (x > SYSICON_SIZE && x < cx - SYSBTN_WIDTH && y < SYSBTN_HEIGHT) {
        return HTCAPTION;
    }

    return HTCLIENT;
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
		return 0; // return nil
	} else {
        thiz = reinterpret_cast<Window *>
            (GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
	assert(NULL != thiz);

    bool bHandled = false;

    LRESULT ret = thiz->m_resizable->HandleMessage(hwnd, message, wparam, lparam, bHandled);
    if (bHandled) {
        return ret;
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
        } while (0);
		return 0;
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
		do 
		{
			RECT rc;
			::GetClientRect(hwnd, &rc);
			::PostMessage(hwnd, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom)); // TODO 这里如果创建出来的是全屏的 第2个参数怎么办呢
		} while (0);
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

void Window::OnPaint(HWND hwnd )
{

	DWORD startTime = ::GetTickCount();
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
    HRESULT hr = E_FAIL;

	//LOG("PAINTSTRUCT:" << ps.fErase << "," << ps.fIncUpdate << "," << ps.fRestore);

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
    }

    m_target->BeginDraw();
    m_target->SetTransform(D2D1::Matrix3x2F::Identity());
    TranslateTransform(m_target, 0.5f, 0.5f);
    m_target->Clear(D2D1::ColorF(D2D1::ColorF(0.1f, 0.1f, 0.2f)));

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
    EndPaint(hwnd, &ps);
}

HWND Window::Handle()
{
	return m_hwnd;
}

void Window::AttachSprite( Sprite *sp )
{
	m_sprite->Unref();
    sp->Ref();
	sp->SetHostWnd(this);
    m_sprite = sp;
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
	assert(sp->GetHostWindow() == this);
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
	if (sp->GetHostWindow() == this)
	{
		m_setTrackMouseLeave.insert(sp);
	}
	// TODO Track for the HWND
}

void Window::BeginAnimation(Sprite *sp)
{
    if (m_setAnimation.size() == 0)
    {
        ::SetTimer(m_hwnd, TIMER_ANIMATION, 33, NULL);
        LOG("SetTimer");
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
            LOG("KillTimer");
        }
    }
}

void Window::OnBtnCloseClicked()
{
    //::CloseWindow(m_hwnd); // MSDN: Minimizes (but does not destroy) the specified window. WTF??
    ::SendMessage(m_hwnd, WM_CLOSE, 0, 0);
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

int Window::AttachSprite(lua_State *L)
{

    return 0;
}

bool Window::OnSize(float cx, float cy, DWORD flag)
{
    m_btnClose->SetRect(RectF(cx - SYSBTN_WIDTH - 1, 0, SYSBTN_WIDTH, SYSBTN_HEIGHT));
    return false;
}

#endif

static void SetWindowRect(HWND hwnd, RECT &rc)
{
    ::MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
}

LRESULT ResizeHelper::HandleMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, bool &bHandled)
{
    float x, y, cx, cy;
    RECT rc;

    switch (message) {
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        x = (float)(short)LOWORD(lparam);
        y = (float)(short)HIWORD(lparam);
        ::GetClientRect(hwnd, &rc);
        cx = (float)(rc.right - rc.left);
        cy = (float)(rc.bottom - rc.top);

        switch(message) {
        case WM_LBUTTONDOWN:
            m_originX = x;
            m_originY = y;
            ::SetCapture(hwnd);

            if (x < WINDOW_BORDER) {
                if (y < WINDOW_BORDER) {
                    m_state = eLeftTop;
                }
                else {
                    m_state = eLeft;
                }
            }
            else if (x < cx - SYSBTN_WIDTH - 5 && y < CAPTION_HEIGHT) {
                m_state = eMove;
            }
            break;
        case WM_MOUSEMOVE:
            if (x < WINDOW_BORDER) {
                if (y < WINDOW_BORDER) {
                    ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENWSE)));
                    bHandled = true;
                }
                else {
                    auto cursor = ::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE));
                    ::SetCursor(cursor);
                    bHandled = true;
                }
            }
            ::GetWindowRect(hwnd, &rc);
            switch (m_state) {
            case eMove:
                ::SetWindowPos(hwnd, NULL, 
                    (int)(rc.left + x - m_originX), 
                    (int)(rc.top + y - m_originY),
                    0, 0, SWP_NOSIZE);
                bHandled = true;
                return 0;
            case eLeft:
                rc.left += (LONG)(x - m_originX);
                SetWindowRect(hwnd, rc);
                return 0;
            case eLeftTop:
                rc.left += (LONG)(x - m_originX);
                rc.top += (LONG)(y - m_originY);
                SetWindowRect(hwnd, rc);
                return 0;
            }
            break;
        case WM_LBUTTONUP:
            ::ReleaseCapture();
            m_state = eNone;
            break;
        }
        break;
    }
    return 0;
}

} // namespace ltk