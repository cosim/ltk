#pragma once

#include "Window.h"
#include "Delegate.h"

namespace ltk {


class TestClass1
{
public:
    void OnBtn1Clicked(LPCWSTR text);
private:
    int m_data = 0;
};

class MainWindow : public Window
{
public:
    MainWindow();
    virtual ~MainWindow();

    virtual bool OnSize(float cx, float cy, DWORD flag) override;
    virtual bool OnClose(bool &proceed) override;

    void OnBtn1Clicked(LPCWSTR text);

private:
    TestClass1 *m_test = nullptr;
    ConnectionTracker m_testConn;
};

} // namespace
