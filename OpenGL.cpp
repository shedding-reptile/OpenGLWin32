#include "OpenGL.h"

OpenGL::OpenGL() :
    deviceContext(nullptr),
    renderingContext(nullptr),
    wglChoosePixelFormatARB(nullptr),
    wglCreateContextAttribsARB(nullptr),
    wglSwapIntervalEXT(nullptr),
    hWnd(nullptr)
{
}

OpenGL::~OpenGL()
{
    if (renderingContext)
    {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(renderingContext);
    }

    if (deviceContext)
    {
        ReleaseDC(hWnd, deviceContext);
    }
}

bool OpenGL::initExt(HWND tempHWnd)
{
    // Set the pixel format.
    PIXELFORMATDESCRIPTOR pixelFormat = {};

    const HDC deviceContext = GetDC(tempHWnd);
    if (!deviceContext)
    {
        return false;
    }

    int error = SetPixelFormat(deviceContext, 1, &pixelFormat);
    if (error != 1)
    {
        return false;
    }

    // Create and set the rendering context.
    const HGLRC renderContext = wglCreateContext(deviceContext);
    if (!renderContext)
    {
        return false;
    }

    error = wglMakeCurrent(deviceContext, renderContext);
    if (error != 1)
    {
        return false;
    }

    // Load OpenGL extensions.
    if (!loadExtensionList())
    {
        return false;
    }

    // Cleanup
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(renderContext);
    ReleaseDC(tempHWnd, deviceContext);

    return true;
}

bool OpenGL::init(HWND parent)
{
    hWnd = parent;

    int attributeListInt[] =
    {
        // Support for OpenGL rendering.
        WGL_SUPPORT_OPENGL_ARB,
        TRUE,

        // Support for rendering to a window.
        WGL_DRAW_TO_WINDOW_ARB,
        TRUE,

        // Support for hardware acceleration.
        WGL_ACCELERATION_ARB,
        WGL_FULL_ACCELERATION_ARB,

        // Support for 24bit color.
        WGL_COLOR_BITS_ARB,
        24,

        // Support for 24 bit depth buffer.
        WGL_DEPTH_BITS_ARB,
        24,

        // Support for double buffer.
        WGL_DOUBLE_BUFFER_ARB,
        TRUE,

        // Support for swapping front and back buffer.
        WGL_SWAP_METHOD_ARB,
        WGL_SWAP_EXCHANGE_ARB,

        // Support for the RGBA pixel type.
        WGL_PIXEL_TYPE_ARB,
        WGL_TYPE_RGBA_ARB,

        // Support for a 8 bit stencil buffer.
        WGL_STENCIL_BITS_ARB,
        8,

        // Null terminate the attribute list.
        0
    };

    // Get the device context for the parent window.
    deviceContext = GetDC(hWnd);
    if (!deviceContext)
    {
        return false;
    }

    // Set the pixel format.
    int pixelFormat;
    unsigned formatCount;

    if (wglChoosePixelFormatARB(deviceContext, attributeListInt, nullptr, 1, &pixelFormat, &formatCount) != 1)
    {
        return false;
    }

    PIXELFORMATDESCRIPTOR pixelFormatDescriptor = {};
    if (SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDescriptor) != 1)
    {
        return false;
    }

    // Create and set the rendering context.
    int attributeList[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB,
        4,
        WGL_CONTEXT_MINOR_VERSION_ARB,
        0,
        0
    };

    renderingContext = wglCreateContextAttribsARB(deviceContext, nullptr, attributeList);
    if (renderingContext == nullptr)
    {
        return false;
    }

    if (wglMakeCurrent(deviceContext, renderingContext) != 1)
    {
        return false;
    }



    // Set VSync
    if (wglSwapIntervalEXT(vsync ? 1 : 0) != 1)
    {
        return false;
    }

    return true;
}

void OpenGL::beginScene(float red, float green, float blue, float alpha)
{
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL::endScene() const
{
    SwapBuffers(deviceContext);
}

void OpenGL::resize(unsigned w, unsigned h)
{
    glViewport(0, 0, w, h);
}

std::wstring OpenGL::checkGLError()
{
    GLenum err(glGetError());
    std::wstring error;

    while (err != GL_NO_ERROR)
    {
        switch (err)
        {
        case GL_INVALID_ENUM:
            error = L"INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = L"INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = L"INVALID_OPERATION";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = L"INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error = L"OUT_OF_MEMORY";
            break;
        default:;
        }

        err = glGetError();
    }

    return error;
}

bool OpenGL::loadExtensionList()
{
    wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
    if (!wglChoosePixelFormatARB)
    {
        return false;
    }

    wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
    if (!wglCreateContextAttribsARB)
    {
        return false;
    }

    wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
    if (!wglSwapIntervalEXT)
    {
        return false;
    }

    if (gl3wInit())
    {
        return false;
    }

    if (!gl3wIsSupported(4, 0))
    {
        return false;
    }

    return true;
}
