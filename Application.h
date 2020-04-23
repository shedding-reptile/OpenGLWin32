#pragma once

#include "OpenGL.h"
#include "Graphics.h"

class Application
{
public:
    Application();
    ~Application();
    void run();
    void handleKey(MSG& msg);
    LRESULT CALLBACK wndMsgHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    INT_PTR CALLBACK dlgMsgHandler(HWND hDlgWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    bool frame() const;
    bool initWindow();
    void closeWindow();
    void resizeViewport() const;

    static const int maxLoadString = 100;
    HINSTANCE hInstance;
    HWND hWnd;
    WCHAR title[maxLoadString];
    WCHAR className[maxLoadString];
    OpenGL* context;
    Graphics* graph;
};

static Application* applicationHandle = nullptr;
