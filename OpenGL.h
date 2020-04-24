#pragma once

#include <Windows.h>
#include "GL/gl3w.h"
#include "GL/GL.h"
#include "GL/wgl.h"
#include <string>

class OpenGL
{
public:
    OpenGL();
    ~OpenGL();
    bool initExt(HWND tempHWnd);
    bool init(HWND parent);
    static void beginScene(float red, float green, float blue, float alpha);
    void endScene() const;
    static void resize(unsigned w, unsigned h);
    static std::wstring checkGLError();

private:
    bool loadExtensionList();

    HDC deviceContext;
    HGLRC renderingContext;
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
    const bool vsync = true;
    HWND hWnd;
};
