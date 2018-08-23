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
    void Update(HWND hParent, bool bRedraw);
    void OnDraw(const RECT &rc);
    void DrawShadow(Gdiplus::Graphics &g, Gdiplus::Rect rc);

    static void RegisterWndClass();
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
    HWND m_hwnd = 0;
    Mode m_mode = eLeft;
    long m_sizeLeft = 16;
    long m_sizeTop = 15;
    long m_sizeRight = 15;
    long m_sizeBottom = 19;
    static Gdiplus::Bitmap *m_bitmap;
};

}
