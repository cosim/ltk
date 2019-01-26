size
/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/

#include "stdafx.h"
#include "atlstr.h"
#include "ltk.h"
#include "MainWindow.h"
#include "Common.h"
#include "Sprite.h"
#include "BoxLayout.h"
#include "ApiBinding.h"
#include "LuaObject.h"
#include "Label.h"
#include "Serializer.h"
#include "ShadowFrame.h"
#include "StyleManager.h"
#include "ImageSprite.h"
#include "ScrollBar.h"
#include "LuaVector.h"
#include "CheckBox.h"

static ID2D1Factory *g_d2d_factory = NULL;
static IWICImagingFactory  *g_wic_factory = NULL;
static IDWriteFactory *g_dw_factory = NULL;
static ULONG_PTR m_gdiplusToken;
static lua_State *g_luaState = NULL;

namespace ltk {
    ID2D1Factory *GetD2DFactory() { return g_d2d_factory; }
    IWICImagingFactory *GetWICFactory() { return g_wic_factory; }
    IDWriteFactory *GetDWriteFactory() { return g_dw_factory; }
    lua_State *GetGlobalLuaState() { return g_luaState; }

    // convert DIP to screen
    void MapCoordByDpi(float &x, float &y) 
    {
        static float dpi_x = 0.0f;
        static float dpi_y = 0.0f;
        if (dpi_x == 0.0f) {
            g_d2d_factory->GetDesktopDpi(&dpi_x, &dpi_y); // wtf? non-square pixel?
        }
        x = dpi_x * x / 96.0f;
        y = dpi_y * y / 96.0f;
    }

    // convert screen to DIP
    void UnmapCoordByDpi(float &x, float &y)
    {
        static float dpi_x = 0.0f;
        static float dpi_y = 0.0f;
        if (dpi_x == 0.0f) {
            g_d2d_factory->GetDesktopDpi(&dpi_x, &dpi_y); // wtf? non-square pixel?
        }
        x = x * 96.0f / dpi_x;
        y = y * 96.0f / dpi_y;
    }

    static void AdjustRect(D2D1_RECT_F &src, D2D1_RECT_F &dst, float scale)
    {
        //src.left = (float)(int)(src.left + 0.5f);
        //src.top = (float)(int)(src.top + 0.5f);
        //src.right = (float)(int)(src.right + 0.5f);
        //src.bottom = (float)(int)(src.bottom + 0.5f);

        //MapCoordByDpi(dst.left, dst.top);
        //MapCoordByDpi(dst.right, dst.bottom);
        //dst.left = (float)(int)(dst.left + 2.5f);
        //dst.top = (float)(int)(dst.top + 2.5f);
        //dst.right = (float)(int)(dst.right + 2.5f);
        //dst.bottom = (float)(int)(dst.bottom + 2.5f);
        //UnmapCoordByDpi(dst.left, dst.top);
        //UnmapCoordByDpi(dst.right, dst.bottom);
    }

    void DrawTextureNineInOne(ID2D1RenderTarget *target, ID2D1Bitmap *bitmap, 
        const RectF &atlas, const Margin& margin, RectF dst2, float opacity, float scale)
    {
        D2D1_BITMAP_INTERPOLATION_MODE interp_mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
        D2D1_RECT_F src;
        D2D1_RECT_F dst;
        Margin dMargin;
        dMargin.left = margin.left * scale;
        dMargin.top = margin.top * scale;
        dMargin.right = margin.right * scale;
        dMargin.bottom = margin.bottom * scale;

        // left top
        src.left = atlas.X;
        src.top = atlas.Y;
        src.right = atlas.X + margin.left;
        src.bottom = atlas.Y + margin.top;
        dst.left = dst2.X;
        dst.top = dst2.Y;
        dst.right = dst2.X + dMargin.left;
        dst.bottom = dst2.Y + dMargin.top;
        AdjustRect(src, dst, scale);
        target->DrawBitmap(bitmap, dst, opacity, interp_mode, src);

        // left center
        src.left = atlas.X;
        src.top = atlas.Y + margin.top;
        src.right = atlas.X + margin.left;
        src.bottom = atlas.Y + atlas.Height - margin.bottom;
        dst.left = dst2.X;
        dst.top = dst2.Y + dMargin.top ;
        dst.right = dst2.X + dMargin.left;
        dst.bottom = dst2.Y + dst2.Height - dMargin.bottom;
        AdjustRect(src, dst, scale);
        target->DrawBitmap(bitmap, dst, opacity, interp_mode, src);
    
        // left bottom
        src.left = atlas.X;
        src.top = atlas.Y + atlas.Height - margin.bottom;
        src.right = atlas.X + margin.left;
        src.bottom = atlas.Y + atlas.Height;
        dst.left = dst2.X;
        dst.top = dst2.Y + dst2.Height - dMargin.bottom;
        dst.right = dst2.X + dMargin.left;
        dst.bottom = dst2.Y + dst2.Height;
        AdjustRect(src, dst, scale);
        target->DrawBitmap(bitmap, dst, opacity, interp_mode, src);

        // center top
        src.left = atlas.X + margin.left;
        src.top = atlas.Y;
        src.right = atlas.X + atlas.Width - margin.right;
        src.bottom = atlas.Y + margin.top;
        dst.left = dst2.X + dMargin.left;
        dst.top = dst2.Y;
        dst.right = dst2.X + dst2.Width - dMargin.right;
        dst.bottom = dst2.Y + dMargin.top;
        AdjustRect(src, dst, scale);
        target->DrawBitmap(bitmap, dst, opacity, interp_mode, src);

        // center center
        src.left = atlas.X + margin.left;
        src.top = atlas.Y + margin.top;
        src.right = atlas.X + atlas.Width - margin.right;
        src.bottom = atlas.Y + atlas.Height - margin.bottom;
        dst.left = dst2.X + dMargin.left;
        dst.top = dst2.Y + dMargin.top;
        dst.right = dst2.X + dst2.Width - dMargin.right;
        dst.bottom = dst2.Y + dst2.Height - dMargin.bottom;
        AdjustRect(src, dst, scale);
        target->DrawBitmap(bitmap, dst, opacity, interp_mode, src);

        // center bottom
        src.left = atlas.X + margin.left;
        src.top = atlas.Y + atlas.Height - margin.bottom;
        src.right = atlas.X + atlas.Width - margin.right;
        src.bottom = atlas.Y + atlas.Height;
        dst.left = dst2.X + dMargin.left;
        dst.top = dst2.Y + dst2.Height - dMargin.bottom;
        dst.right = dst2.X + dst2.Width - dMargin.right;
        dst.bottom = dst2.Y + dst2.Height;
        AdjustRect(src, dst, scale);
        target->DrawBitmap(bitmap, dst, opacity, interp_mode, src);

        // right top
        src.left = atlas.X + atlas.Width - margin.right;
        src.top = atlas.Y;
        src.right = atlas.X + atlas.Width;
        src.bottom = atlas.Y + margin.top;
        dst.left = dst2.X + dst2.Width - dMargin.right;
        dst.top = dst2.Y;
        dst.right = dst2.X + dst2.Width;
        dst.bottom = dst2.Y + dMargin.top;
        AdjustRect(src, dst, scale);
        target->DrawBitmap(bitmap, dst, opacity, interp_mode, src);

        // right center
        src.left = atlas.X + atlas.Width - margin.right;
        src.top = atlas.Y + margin.top;
        src.right = atlas.X + atlas.Width;
        src.bottom = atlas.Y + atlas.Height - margin.bottom;
        dst.left = dst2.X + dst2.Width - dMargin.right;
        dst.top = dst2.Y + dMargin.top;
        dst.right = dst2.X + dst2.Width;
        dst.bottom = dst2.Y + dst2.Height - dMargin.bottom;
        AdjustRect(src, dst, scale);
        target->DrawBitmap(bitmap, dst, opacity, interp_mode, src);

        // right bottom
        src.left = atlas.X + atlas.Width - margin.right;
        src.top = atlas.Y + atlas.Height - margin.bottom;
        src.right = atlas.X + atlas.Width;
        src.bottom = atlas.Y + atlas.Height;
        dst.left = dst2.X + dst2.Width - dMargin.right;
        dst.top = dst2.Y + dst2.Height - dMargin.bottom;
        dst.right = dst2.X + dst2.Width;
        dst.bottom = dst2.Y + dst2.Height;
        AdjustRect(src, dst, scale);
        target->DrawBitmap(bitmap, dst, opacity, interp_mode, src);
    }

    void DrawRectSnapped(ID2D1RenderTarget *target, const RectF &rc, ID2D1Brush *brush)
    {
        D2D1_RECT_F rc2 = D2D1RectF(rc);
        MapCoordByDpi(rc2.left, rc2.top);
        MapCoordByDpi(rc2.right, rc2.bottom);
        rc2.left = Round45(rc2.left) + 0.5f;
        rc2.top = Round45(rc2.top) + 0.5f;
        rc2.right = Round45(rc2.right) + 0.5f;
        rc2.bottom = Round45(rc2.bottom) + 0.5f;
        UnmapCoordByDpi(rc2.left, rc2.top);
        UnmapCoordByDpi(rc2.right, rc2.bottom);

        static float dpi_x = 0.0f;
        static float dpi_y = 0.0f;
        if (dpi_x == 0.0f) {
            g_d2d_factory->GetDesktopDpi(&dpi_x, &dpi_y); // wtf? non-square pixel?
        }
        target->DrawRectangle(rc2, brush, 96.0f / dpi_x);
    }

    void TranslateTransform(ID2D1RenderTarget *target, float dx, float dy)
    {
        D2D1_MATRIX_3X2_F matrix;
        target->GetTransform(&matrix);
        matrix._31 += dx;
        matrix._32 += dy;
        target->SetTransform(&matrix);
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
            LTK_LOG("already initialized.");
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

        // InitInstance
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
    }

    void LtkUninitialize()
    {
        ShadowFrame::Free();
        StyleManager::Free();

        g_dw_factory->Release();
        g_wic_factory->Release();
        g_d2d_factory->Release();

        Gdiplus::GdiplusShutdown(m_gdiplusToken);

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
        font = new Gdiplus::Font(L"΢���ź�", 16);
    }
}
*/

static void size_test()
{
    std::map<int, void*> map1;
    LTK_LOG("map1: %d", sizeof(map1));
    Delegate<void(void)> dlgt1;
    LTK_LOG("dlgt1: %d", sizeof(dlgt1));
    std::function<void(void)> fn1;
    LTK_LOG("fn1: %d", sizeof(fn1));
    Delegate<void(int, float, std::string)> dlgt2;
    LTK_LOG("dlgt2: %d", sizeof(dlgt2));
    std::function<void(int, float, std::string)> fn2;
    LTK_LOG("fn2: %d", sizeof(fn2));
    LTK_LOG("Sprite: %d", sizeof(Sprite));
    LTK_LOG("LuaObject: %d", sizeof(LuaObject));
    LTK_LOG("RefCounted: %d",sizeof(RefCounted));
    std::forward_list<int> slist;
    LTK_LOG("forward_list: %d", sizeof(slist));
    SinglyLinkedList<int> slist2;

}

static void log_thread_test()
{
    auto tfunc = [](){
        for (int i = 0; i < 1000; i++) {
            LTK_LOG("log in thread test: [%d] %i", ::GetCurrentThreadId(), i);
        }
    };
    auto start_time = ::GetTickCount();
    std::thread tt1(tfunc);
    std::thread tt2(tfunc);
    std::thread tt3(tfunc);
    tt1.join();
    tt2.join();
    tt3.join();
    LTK_LOG("time: %d", ::GetTickCount() - start_time);
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

#define LTK_EXPORT_CURRENT_FUNCTION comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

int luaopen_ltk(lua_State *L)
{
    #pragma LTK_EXPORT_CURRENT_FUNCTION
    
    size_test();
    lua_newtable(L);
    lua_setglobal(L, "Ltk");

    g_luaState = L;
    LtkLogInit();
    // log_thread_test();
    LtkInitialize();
    ApiBindingInit(L);
    Window::RegisterWndClass();
    LuaRegisterClass<Window>(L, "Window");
    LuaRegisterClass<Sprite>(L, "Sprite");
    LuaRegisterClass<Button>(L, "Button");
    LuaRegisterClass<BoxLayout>(L, "BoxLayout");
    LuaRegisterClass<Label>(L, "Label");
    LuaRegisterClass<Serializer>(L, "Serializer");
    LuaRegisterClass<StyleManager>(L, "StyleManager");
    LuaRegisterClass<IconSprite>(L, "IconSprite");
    LuaRegisterClass<TextureSprite>(L, "TextureSprite");
    LuaRegisterClass<ImageSprite>(L, "ImageSprite");
    LuaRegisterClass<ScrollBar>(L, "ScrollBar");
    LuaRegisterClass<CheckBox>(L, "CheckBox");
    LuaRegisterClass<RadioGroup>(L, "RadioGroup");

    LuaRegisterClass<LuaVector<unsigned char>>(L, LuaVectorScriptName<unsigned char>());
    LuaRegisterClass<LuaVector<unsigned short>>(L, LuaVectorScriptName<unsigned short>());
    LuaRegisterClass<LuaVector<unsigned long>>(L, LuaVectorScriptName<unsigned long>());

    LuaRegisterClass<LuaVector<signed char>>(L, LuaVectorScriptName<signed char>());
    LuaRegisterClass<LuaVector<signed short>>(L, LuaVectorScriptName<signed short>());
    LuaRegisterClass<LuaVector<signed long>>(L, LuaVectorScriptName<signed long>());

    LuaRegisterClass<LuaVector<float>>(L, LuaVectorScriptName<float>());
    LuaRegisterClass<LuaVector<double>>(L, LuaVectorScriptName<double>());


    ShadowFrame::Init();

    return 0;
}

