
/*
** Copyright (C) 2015-2016 Chen Shi. See Copyright Notice in ltk.h
*/

#include "stdafx.h"
#include "atlstr.h"
#include "ltk.h"
#include "MainWindow.h"
#include "Common.h"
#include "Delegate.h"
#include "Sprite.h"
#include "duktape.h"
#include "ApiBind.h"
#include "duk_trans_socket.h"


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
    LOG("Sprite: " << sizeof(Sprite));
}

duk_context *g_duk_ctx = nullptr;

bool DukDoFile(duk_context *ctx, const wchar_t *path)
{
    DukStackChecker chk(ctx);
    bool ret = false;
    CStringA pathA;
    CStringA error;
    char *buf = nullptr;
    FILE *fp = _wfopen(path, L"rb");
    if (!fp) {
        goto leave;
    }
    if (fseek(fp, 0, SEEK_END) != 0) goto leave;
    long size = ftell(fp);
    if (fseek(fp, 0, SEEK_SET) != 0) goto leave;
    buf = new char[size];
    fread(buf, 1, size, fp);

    pathA = Utf16ToUtf8(path, -1);
    duk_push_string(ctx, pathA);
    if (duk_pcompile_lstring_filename(ctx, 0, buf, size) != 0) {
        error.Format("[%s] %s\n", pathA, duk_safe_to_string(ctx, -1));
        DukLog(error);
        duk_pop(ctx);
        goto leave;
    }
    DukPCall(ctx, 0);
    duk_pop(ctx);
    ret = true;

leave:
    delete[] buf;
    if (fp) fclose(fp);
    return ret;
}

static duk_ret_t native_print(duk_context *ctx) {
    duk_push_string(ctx, "\r\n");
    duk_concat(ctx, duk_get_top(ctx));
    DukLog(duk_safe_to_string(ctx, -1));
    return 0;
}

static void fatal_handler(void *udata, const char *msg)
{
    CStringA log;
    log.Format("<FATAL> %s\r\n", msg);
    DukLog(log);
    // TODO flush log to disk
    ::TerminateProcess(::GetCurrentProcess(), 0);
}

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
    )
{
    //test_enum_cast();
    size_test();
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

    duk_context *ctx = duk_create_heap(NULL, NULL, NULL, NULL, fatal_handler);
    g_duk_ctx = ctx;

    duk_trans_socket_init();
    duk_debugger_attach(ctx,
        duk_trans_socket_read_cb,
        duk_trans_socket_write_cb,
        duk_trans_socket_peek_cb,
        duk_trans_socket_read_flush_cb,
        duk_trans_socket_write_flush_cb,
        nullptr, nullptr, nullptr);
    duk_trans_socket_waitconn();

    duk_push_c_function(ctx, native_print, DUK_VARARGS);
    duk_put_global_string(ctx, "print");

    duk_push_array(ctx);
    duk_put_global_string(ctx, "__ltk_callbacks");

    ApiBindInit(ctx);
    Window::RegisterWndClass();
    DukRegisterClass<Window>(ctx, "Window");

    DukDoFile(ctx, L"script\\main.js");

    //duk_eval_string(ctx, "var wnd = new Window(); wnd.Create(); wnd.Show();");
    //auto wnd = new MainWindow;
    //wnd->Create(nullptr, Gdiplus::RectF(0, 0, 1000, 700), WS_OVERLAPPEDWINDOW, 0);
    //::ShowWindow(wnd->Handle(), SW_SHOW);

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

    g_dw_factory->Release();
    g_wic_factory->Release();
    g_d2d_factory->Release();

    duk_trans_socket_finish();
    duk_destroy_heap(ctx);

    ::CoUninitialize();

    return 0;
}