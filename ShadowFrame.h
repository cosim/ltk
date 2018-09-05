#pragma once

namespace ltk {

class ShadowFrame
{
public:
    enum Mode {
        eLeft, eTop, eRight, eBottom
    };
    ShadowFrame(Mode m);
    ~ShadowFrame();

    static void Init();
    static void Free();
    void Create();
    void Destroy();
    void Update(HWND hParent, HDWP &hdwp, bool bRedraw);
    void OnDraw(const RECT &rc);
    void DrawShadow(Gdiplus::Graphics &g, Gdiplus::Rect rc);

    static void RegisterWndClass();
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static const UINT WM_RENDER_LATER;

private:
    HWND m_hwnd = 0;
    Mode m_mode = eLeft;
    long m_sizeLeft;
    long m_sizeTop;
    long m_sizeRight;
    long m_sizeBottom;
    static Gdiplus::Bitmap *m_bitmap;
};

}
