
/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/

#include "stdafx.h"
#include "atlstr.h"
#include "ltk.h"
#include "MainWindow.h"
#include "Common.h"
#include "Sprite.h"
#include "ApiBinding.h"
#include "LuaObject.h"

static ID2D1Factory *g_d2d_factory = NULL;
static IWICImagingFactory  *g_wic_factory = NULL;
static IDWriteFactory *g_dw_factory = NULL;

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


	HRESULT LoadBitmapFromFile(ID2D1RenderTarget *target, LPCWSTR path, ID2D1Bitmap **bitmap)
	{
		IWICBitmapDecoder *decorder = NULL;
		IWICBitmapFrameDecode *frame = NULL;
		IWICFormatConverter *converter = NULL;
		assert(*bitmap == nullptr);

		HRESULT hr = g_wic_factory->CreateDecoderFromFilename(path, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decorder);
		if (SUCCEEDED(hr))
		{
			hr = decorder->GetFrame(0, &frame);
			assert(SUCCEEDED(hr));
			hr = g_wic_factory->CreateFormatConverter(&converter);
			assert(SUCCEEDED(hr));
			hr = converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
				NULL, 0.0, WICBitmapPaletteTypeMedianCut);
			assert(SUCCEEDED(hr));
			hr = target->CreateBitmapFromWicBitmap(converter, NULL, bitmap);
			assert(SUCCEEDED(hr));
			SAFE_RELEASE(converter);
			SAFE_RELEASE(frame);
			SAFE_RELEASE(decorder);
			return S_OK;
		}
		return E_FAIL;
	}

    void LtkInitialize()
    {
        ::CoInitialize(NULL);

        if (g_d2d_factory) {
            LOG("already initialized.");
            return;
        }

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
    }

    void LtkUninitialize()
    {
        g_dw_factory->Release();
        g_wic_factory->Release();
        g_d2d_factory->Release();

        ::CoUninitialize();
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

void size_test()
{
    std::map<int, void*> map1;
    LOG("map1: " << sizeof(map1));
    Delegate<void(void)> dlgt1;
    LOG("dlgt1: " << sizeof(dlgt1));
    std::function<void(void)> fn1;
    LOG("fn1: " << sizeof(fn1));
    Delegate<void(int, float, std::string)> dlgt2;
    LOG("dlgt2: " << sizeof(dlgt2));
    std::function<void(int, float, std::string)> fn2;
    LOG("fn2: " << sizeof(fn2));
    //LOG("Sprite: " << sizeof(Sprite));
}



int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
    )
{
    //ApiBindInit(ctx);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
    )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#define EXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

int luaopen_ltk(lua_State *L)
{
    #pragma EXPORT
    
    ApiBindingInit(L);
    Window::RegisterWndClass();
    LuaRegisterClass<Window>(L, "Window");

    return 0;
}

