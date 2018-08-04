/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/

#include "StdAfx.h"
#include "Window.h"
#include "Common.h"
#include "Sprite.h"
#include "ltk.h"
#include "ApiBind.h"

extern HINSTANCE g_hInstance;

namespace ltk {

const wchar_t * Window::ClsName = L"ltk_cls";

Window::Window(void)
{
	m_style = WS_OVERLAPPEDWINDOW|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	m_exStyle = 0;

	m_rect.X = 0;
	m_rect.Y = 0;
	m_rect.Width = 800;
	m_rect.Height = 600;

	m_hwnd = NULL;

	m_rectComposition.left = 0;
	m_rectComposition.top = 0;
	m_rectComposition.right = 5;
	m_rectComposition.bottom = 20;

    m_sprite = NULL;
	m_spFocus = NULL;
	m_spCapture = NULL;
	m_spHover = NULL;

	m_caretHeight = 20;
}

Window::~Window(void)
{
    m_sprite->Unref();
    m_sprite = INVALID_POINTER(Sprite);
    m_spFocus = INVALID_POINTER(Sprite);
    m_spCapture = INVALID_POINTER(Sprite);
    m_spHover = INVALID_POINTER(Sprite);
    m_target->Release();
    m_target = INVALID_POINTER(ID2D1HwndRenderTarget);
}

void Window::Create(Window *parent, Gdiplus::RectF rc, DWORD style, DWORD exStyle)
{
    HWND hParent = NULL;
    if (!parent)
    {
        hParent = ::GetDesktopWindow();
    }
    else
    {
        hParent = parent->m_hwnd;
    }

    ::CreateWindowEx(exStyle, ClsName, L"", style,
        (int)rc.X, (int)rc.Y, (int)rc.Width, (int)rc.Height,
        hParent, NULL, g_hInstance, this);
}

void Window::SetRect(Gdiplus::RectF rc)
{
    ::MoveWindow(m_hwnd, (int)rc.X, (int)rc.Y, (int)rc.Width, (int)rc.Height, TRUE);
}

// TODO �ǵô��ڹ��� ����һ��Ҫ��push HWND message wparam lparam ��lua��ջ��
// Ȼ���� pcall һ��cfunction ��ȥ�������� ��Ȼһ��lua���� ���Ҳ����������� ���ҳ����ҵ�

void Window::RegisterWndClass()
{
	WNDCLASS wc;
	wc.style         = 0; // CS_DBLCLKS;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = g_hInstance;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;// (HBRUSH)(COLOR_WINDOW + 1); // TODO �ĳ�NULL ��ֹϵͳȥ��������(��˸) ˫���� ����clip children clip sibling
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
		event.x = (float)(short)LOWORD(lparam); //����2�ֽ� Ȼ��ת�з��� ��ת����
		event.y = (float)(short)HIWORD(lparam);
		event.delta = 0.0f;
	}
    if (WM_LBUTTONDOWN == message)
    {
        //LOG("WM_LBUTTONDOWN: " << event.x << " " << event.y);
    }

	if (m_spCapture)
	{
		Gdiplus::RectF rc = m_spCapture->GetAbsRect();
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
				Gdiplus::RectF rc = sp->GetAbsRect();
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

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	Window *thiz;
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

	switch(message)
	{
	case WM_PAINT:
		thiz->OnPaint(hwnd);
		break;
	case WM_SYNCPAINT:
		// �����д����϶���ʱ�� ����1���ؿ�Ⱥ�1���ظ߶ȵ����� ��ͣ�ķ� ��WM_PAINT���ǵ����ȼ���
		// �����е���Ϣ�������� ���ԲŻ��а���
		// �����win7 û��ë���� ���ô����϶�ʱ��ʾ���� ��������Ĵ��ڱ��϶� ����Ứ
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
            thiz->OnSize((float)cx, (float)cy, wparam);
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
			::PostMessage(hwnd, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom)); // TODO �������������������ȫ���� ��2��������ô����
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
    m_target->Clear(D2D1::ColorF(D2D1::ColorF::White));

    if (m_sprite)
    {
        Gdiplus::RectF rc = m_sprite->GetRect();
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
/*
	int x = ps.rcPaint.left;
	int y = ps.rcPaint.top;
	int width = ps.rcPaint.right - ps.rcPaint.left;
	int height = ps.rcPaint.bottom - ps.rcPaint.top;
	Gdiplus::RectF rcDirty((float)x, (float) y, (float)width, (float)height);

	// http://www.codeproject.com/Tips/66909/Rendering-fast-with-GDI-What-to-do-and-what-not-to
	Gdiplus::Graphics gScreen(hwnd, FALSE);
	SetGdipMode(gScreen);
	Gdiplus::Bitmap bmp(width, height, PixelFormat32bppPARGB);
	Gdiplus::Graphics gDoubleBuffer(&bmp);
	SetGdipMode(gDoubleBuffer);
	//Gdiplus::Graphics gDoubleBuffer(dcMem);
    gDoubleBuffer.TranslateTransform(0.5f, 0.5f);
	gDoubleBuffer.TranslateTransform(-(float)x, -(float)y);
	if (m_sprite)
	{
		Gdiplus::RectF rc = m_sprite->GetRect();
		gDoubleBuffer.TranslateTransform(rc.X, rc.Y);
		m_sprite->HandlePaint(gDoubleBuffer, rcDirty);
		gDoubleBuffer.TranslateTransform(-rc.X, -rc.Y);
	}
	do 
	{
		// ��ʾFPS
		/ *
		gDoubleBuffer.TranslateTransform(rcDirty.X, rcDirty.Y);
		CStringW speed;
		speed.Format(L"%dms", ::GetTickCount() - startTime);
		Gdiplus::Font font(L"΢���ź�", 12.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		Gdiplus::StringFormat format;
		format.SetFormatFlags(Gdiplus::StringFormatFlagsMeasureTrailingSpaces | Gdiplus::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlagsBypassGDI);
		Gdiplus::SolidBrush brush(Gdiplus::Color(255,0,255));
		Gdiplus::PointF pt(0.0f, 0.0f);
		gDoubleBuffer.DrawString(speed, speed.GetLength(), &font, pt, &format, &brush);
		* /
	} while (0);
	do 
	{
		// ���Ե�ʱ����ʾ�����
        / *
		gDoubleBuffer.ResetTransform();
        Gdiplus::Pen pen(Gdiplus::Color(rand() % 256, rand() % 256, rand() % 256), 1.0f);
		gDoubleBuffer.DrawRectangle(&pen, 1, 1, width - 1, height - 1);
        * /
        / *
		const int size = 10;
		gDoubleBuffer.DrawLine(&pen, width / 2 - size, height / 2, width / 2 + size, height / 2);
		gDoubleBuffer.DrawLine(&pen, width / 2, height / 2 - size, width / 2, height / 2 + size);
		* /
	} while (0);

	gScreen.DrawImage(&bmp,x, y);
*/
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
    // TODO �Ӹ����� �ܹ��ر����뷨 �����㲻��������ϵ�ʱ��͹ر�
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
				wstring tmp = comp.ime_string; // �����β��Ȼ�����0 �����Ǹ��˹ȸ�Ĵ��뵼�µ�����
				tmp.resize(wcslen(tmp.c_str()));
				LOGW(<<tmp);
				OnImeInput(tmp.c_str());
				//m_text += tmp;
				//::InvalidateRect(GetHWND(), NULL, TRUE);
			}
			m_ime.ResetComposition(m_hwnd);
			m_ime.EnableIME(m_hwnd, m_rectComposition, false); // ���봰�ڸ���
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
		// ������SpriteҲ�п���ȥ����ime��Ϣ����������һ��RichEdit
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

#ifndef LTK_DISABLE_DUKTAPE

duk_ret_t Window::DukInit(duk_context *ctx)
{
    DukStackChecker check(ctx);
    duk_push_c_function(ctx, DukConstructor, 0); // ctor
    duk_push_object(ctx); // ctor proto
    RegisterMethods(ctx);
    duk_put_prop_string(ctx, -2, "prototype"); // ctor
    duk_put_global_string(ctx, "Window"); // empty
    return 0;
}

duk_ret_t Window::DukConstructor(duk_context *ctx)
{
    if (!duk_is_constructor_call(ctx)) {
        return DUK_RET_TYPE_ERROR;
    }
    duk_push_this(ctx);
    Window *wnd = new Window();
    duk_push_pointer(ctx, wnd);
    duk_put_prop_string(ctx, -2, DukThisSymbol);
    return 0;
}

duk_ret_t Window::Create(duk_context *ctx)
{
    auto thiz = DukCheckThis<Window>(ctx); if (!thiz) return DUK_RET_TYPE_ERROR;
    Gdiplus::RectF rc(0, 0, 100, 100);
    DWORD style = WS_OVERLAPPEDWINDOW;
    if (duk_is_object(ctx, 0))
    {
        duk_get_prop_string(ctx, 0, "rect");
        if (duk_is_object(ctx, -1))
        {
            if (!DukGetRect(ctx, -1, rc)) {
                rc = Gdiplus::RectF(0, 0, 100, 100);
            }
        }
        duk_pop(ctx); // rect

        duk_get_prop_string(ctx, 0, "style");
        if (duk_is_number(ctx, -1))
        {
            style = (DWORD) duk_get_number(ctx, -1);
        }
        duk_pop(ctx); // style
    }
    thiz->Create(nullptr, rc, style, 0);
    return 0;
}

duk_ret_t Window::Show(duk_context *ctx)
{
    auto thiz = DukCheckThis<Window>(ctx); if (!thiz) return DUK_RET_TYPE_ERROR;
    int cmd = SW_SHOW;
    if (duk_is_number(ctx, 0))
    {
        cmd = (int)duk_get_number(ctx, 0);
    }
    ::ShowWindow(thiz->Handle(), cmd);
    return 0;
}

#endif // LTK_DISABLE_DUKTAPE

} // namespace ltk