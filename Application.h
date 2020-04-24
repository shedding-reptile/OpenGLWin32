#pragma once

#include "Graphics.h"

class Application
{
public:
    Application();
    ~Application();
    void run() const;
    LRESULT CALLBACK wndMsgHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) const;
    
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
