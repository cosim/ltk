
/*
** LuaUI2.cpp : Defines the exported functions for the DLL application.
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in LuaUI2.h
*/

#include "stdafx.h"
#include "atlstr.h"
#include "LuaUI2.h"
#include "MainWindow.h"
#include "Common.h"


static ID2D1Factory *g_d2d_factory = NULL;
static IWICImagingFactory  *g_wic_factory = NULL;
static IDWriteFactory *g_dw_factory = NULL;

HINSTANCE g_hInstance;

namespace ltk {
    ID2D1Factory *GetD2DFactory() { return g_d2d_factory; }
    IWICImagingFactory *GetWICFactory() { return g_wic_factory; }
    IDWriteFactory *GetDWriteFactory() { return g_dw_factory; }

    void TranslateTransform(ID2D1RenderTarget *target, float dx, float dy)
    {
        D2D1_MATRIX_3X2_F matrix;
        target->GetTransform(&matrix);
        matrix._31 += dx;
        matrix._32 += dy;
        target->SetTransform(&matrix);
    }

    D2D1_RECT_F GdipRectF2D2D1RectF(const Gdiplus::RectF &rc)
    {
        D2D1_RECT_F rc2;
        rc2.left = rc.X;
        rc2.top = rc.Y;
        rc2.right = rc.X + rc.Width;
        rc2.bottom = rc.Y + rc.Height;
        return rc2;
    }

}

using namespace ltk;

/*
void test_gdip_font()
{
    Gdiplus::Font *font = nullptr;
    for (size_t i = 0; i < 1000000; i++)
    {
        font = new Gdiplus::Font(L"Î¢ÈíÑÅºÚ", 16);
    }
}
*/

struct EnumBase
{
    EnumBase(UINT id_) : id(id_) {}
    UINT id;
};

enum {
    eStringMessage,
    eIntMessage
};

struct StringMessage : EnumBase
{
    static const UINT tag = eStringMessage;
    StringMessage(const std::string& t) : EnumBase(tag), text(t) {}
    std::string text;
};

struct IntMessage : EnumBase
{
    static const UINT tag = eIntMessage;
    IntMessage(int i) : EnumBase(tag), data(i) {}
    int data;
};

template<typename T>
T *enum_cast(EnumBase *base)
{
    assert(base->id == T::tag);
    return static_cast<T*>(base);
}

void test_enum_cast()
{
    //StringMessage msg("hello enum");
    IntMessage msg(123);
    EnumBase *base = &msg;
    StringMessage *wwn = enum_cast<StringMessage>(base);
}

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
    )
{
    //test_enum_cast();
    g_hInstance = hInstance;

    ::CoInitialize(NULL);

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d_factory);
    assert(SUCCEEDED(hr));

    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IWICImagingFactory,
        (LPVOID*)&g_wic_factory
        );
    assert(SUCCEEDED(hr));

    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&g_dw_factory)
        );
    assert(SUCCEEDED(hr));

    Window::RegisterWndClass();
    auto wnd = new MainWindow;
    wnd->Create(nullptr, Gdiplus::RectF(0, 0, 1000, 700), WS_OVERLAPPEDWINDOW, 0);
    ::ShowWindow(wnd->Handle(), SW_SHOW);

    //test_gdip_font();
    MSG msg;
    BOOL bRet;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            // handle the error and possibly exit
            LOG(<< "ERROR");
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    LOG(<< "WM_QUIT");

    delete wnd;
    g_dw_factory->Release();
    g_wic_factory->Release();
    g_d2d_factory->Release();

    ::CoUninitialize();

    return 0;
}