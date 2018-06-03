#include "stdafx.h"
#include "MainWindow.h"
#include "Button.h"
#include "Common.h"

namespace cs {

void TestClass1::OnBtn1Clicked(LPCWSTR text)
{
    LOGW(<< text << L" " << std::hex << m_data);
}


MainWindow::MainWindow()
{
    m_test = new TestClass1;

    const int btn_w = 40;
    const int padding = 20;

    Sprite *sp = new Sprite;

    for (int i = 0; i < 12; i ++)
    {
        auto btn = new Button;
        btn->SetText(L"X");
        btn->SetRect(Gdiplus::RectF(
            (float)padding + (btn_w + padding) * (i % 3), 
            (float)padding + (btn_w + padding) * (i / 3), (float)btn_w, (float)btn_w));
        sp->AddChild(btn);
    }
    this->AttachSprite(sp);
/*
    m_bg = new RectangleSprite;
    m_bg->SetColor(Gdiplus::Color(255, 255, 255));
    m_bg->SetBorderColor(Gdiplus::Color(127, 127, 127));
    this->AttachSprite(m_bg);
    auto btn = new Button;
    btn->SetText(L"Hello World!");
    btn->SetRect(Gdiplus::RectF(20, 20, 200, 40));
    m_testConn = btn->Clicked.Attach(std::bind(&TestClass1::OnBtn1Clicked, m_test, L"Hello World!!"));
    m_bg->AddChild(btn);
    m_testConn.Close();
    delete m_test;

    m_text = new TextView;
    m_text->SetRect(Gdiplus::RectF(20, 70, 500, 400));
    FILE *fp = _wfopen(L"res\\test.txt", L"rb");
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        std::string utf8;
        utf8.resize(size);
        fseek(fp, 0, SEEK_SET);
        auto cb_read = fread((void*)utf8.data(), 1, size, fp);
        assert((long)cb_read == size);
        CA2WEX<4000> utf16(utf8.data(), CP_ACP);
        m_text->SetText((LPCWSTR)utf16);
    }
    m_bg->AddChild(m_text);*/
}


MainWindow::~MainWindow()
{
    delete m_test;
}

bool MainWindow::OnSize(float cx, float cy, DWORD flag)
{
/*
    m_bg->SetRect(Gdiplus::RectF(1.0f, 1.0f, cx - 1.0f, cy - 1.0f));
    m_text->SetRect(Gdiplus::RectF(20, 70, cx - 40, cy - 90));
*/
    return true;
}

bool MainWindow::OnClose(bool &proceed)
{
    proceed = true;
    ::PostQuitMessage(0);
    return true;
}

void MainWindow::OnBtn1Clicked(LPCWSTR text)
{
    //::MessageBox(this->Handle(), text, NULL, 0);
}

} // namespace
