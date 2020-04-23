#include "OpenGL.h"

OpenGL::OpenGL() :
    glAttachShader(nullptr),
    glBindBuffer(nullptr),
    glBindVertexArray(nullptr),
    glBufferData(nullptr),
    glBufferSubData(nullptr),
    glCompileShader(nullptr),
    glCreateProgram(nullptr),
    glCreateShader(nullptr),
    glDeleteBuffers(nullptr),
    glDeleteProgram(nullptr),
    glDeleteShader(nullptr),
    glDeleteVertexArrays(nullptr),
    glDetachShader(nullptr),
    glEnableVertexAttribArray(nullptr),
    glGenBuffers(nullptr),
    glGenVertexArrays(nullptr),
    glGetAttribLocation(nullptr),
    glGetProgramInfoLog(nullptr),
    glGetProgramiv(nullptr),
    glGetShaderInfoLog(nullptr),
    glGetShaderiv(nullptr),
    glLinkProgram(nullptr),
    glShaderSource(nullptr),
    glUseProgram(nullptr),
    glVertexAttribPointer(nullptr),
    glBindAttribLocation(nullptr),
    glGetUniformLocation(nullptr),
    glGetUniformfv(nullptr),
    glUniformMatrix4fv(nullptr),
    glActiveTexture(nullptr),
    glUniform1i(nullptr),
    glGenerateMipmap(nullptr),
    glDisableVertexAttribArray(nullptr),
    glUniform1f(nullptr),
    glUniform3fv(nullptr),
    glUniform4fv(nullptr),
    glDrawArraysInstanced(nullptr),
    glDrawElementsInstanced(nullptr),
    glVertexAttribDivisor(nullptr),
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

    glAttachShader = reinterpret_cast<PFNGLATTACHSHADERPROC>(wglGetProcAddress("glAttachShader"));
    if (!glAttachShader)
    {
        return false;
    }

    glBindBuffer = reinterpret_cast<PFNGLBINDBUFFERPROC>(wglGetProcAddress("glBindBuffer"));
    if (!glBindBuffer)
    {
        return false;
    }

    glBindVertexArray = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(wglGetProcAddress("glBindVertexArray"));
    if (!glBindVertexArray)
    {
        return false;
    }

    glBufferData = reinterpret_cast<PFNGLBUFFERDATAPROC>(wglGetProcAddress("glBufferData"));
    if (!glBufferData)
    {
        return false;
    }

    glBufferSubData = reinterpret_cast<PFNGLBUFFERSUBDATAPROC>(wglGetProcAddress("glBufferSubData"));
    if (!glBufferSubData)
    {
        return false;
    }

    glCompileShader = reinterpret_cast<PFNGLCOMPILESHADERPROC>(wglGetProcAddress("glCompileShader"));
    if (!glCompileShader)
    {
        return false;
    }

    glCreateProgram = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(wglGetProcAddress("glCreateProgram"));
    if (!glCreateProgram)
    {
        return false;
    }

    glCreateShader = reinterpret_cast<PFNGLCREATESHADERPROC>(wglGetProcAddress("glCreateShader"));
    if (!glCreateShader)
    {
        return false;
    }

    glDeleteBuffers = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(wglGetProcAddress("glDeleteBuffers"));
    if (!glDeleteBuffers)
    {
        return false;
    }

    glDeleteProgram = reinterpret_cast<PFNGLDELETEPROGRAMPROC>(wglGetProcAddress("glDeleteProgram"));
    if (!glDeleteProgram)
    {
        return false;
    }

    glDeleteShader = reinterpret_cast<PFNGLDELETESHADERPROC>(wglGetProcAddress("glDeleteShader"));
    if (!glDeleteShader)
    {
        return false;
    }

    glDeleteVertexArrays = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(wglGetProcAddress("glDeleteVertexArrays"));
    if (!glDeleteVertexArrays)
    {
        return false;
    }

    glDetachShader = reinterpret_cast<PFNGLDETACHSHADERPROC>(wglGetProcAddress("glDetachShader"));
    if (!glDetachShader)
    {
        return false;
    }

    glEnableVertexAttribArray = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(wglGetProcAddress("glEnableVertexAttribArray"));
    if (!glEnableVertexAttribArray)
    {
        return false;
    }

    glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>(wglGetProcAddress("glGenBuffers"));
    if (!glGenBuffers)
    {
        return false;
    }

    glGenVertexArrays = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(wglGetProcAddress("glGenVertexArrays"));
    if (!glGenVertexArrays)
    {
        return false;
    }

    glGetAttribLocation = reinterpret_cast<PFNGLGETATTRIBLOCATIONPROC>(wglGetProcAddress("glGetAttribLocation"));
    if (!glGetAttribLocation)
    {
        return false;
    }

    glGetProgramInfoLog = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(wglGetProcAddress("glGetProgramInfoLog"));
    if (!glGetProgramInfoLog)
    {
        return false;
    }

    glGetProgramiv = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(wglGetProcAddress("glGetProgramiv"));
    if (!glGetProgramiv)
    {
        return false;
    }

    glGetShaderInfoLog = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(wglGetProcAddress("glGetShaderInfoLog"));
    if (!glGetShaderInfoLog)
    {
        return false;
    }

    glGetShaderiv = reinterpret_cast<PFNGLGETSHADERIVPROC>(wglGetProcAddress("glGetShaderiv"));
    if (!glGetShaderiv)
    {
        return false;
    }

    glLinkProgram = reinterpret_cast<PFNGLLINKPROGRAMPROC>(wglGetProcAddress("glLinkProgram"));
    if (!glLinkProgram)
    {
        return false;
    }

    glShaderSource = reinterpret_cast<PFNGLSHADERSOURCEPROC>(wglGetProcAddress("glShaderSource"));
    if (!glShaderSource)
    {
        return false;
    }

    glUseProgram = reinterpret_cast<PFNGLUSEPROGRAMPROC>(wglGetProcAddress("glUseProgram"));
    if (!glUseProgram)
    {
        return false;
    }

    glVertexAttribPointer = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(wglGetProcAddress("glVertexAttribPointer"));
    if (!glVertexAttribPointer)
    {
        return false;
    }

    glBindAttribLocation = reinterpret_cast<PFNGLBINDATTRIBLOCATIONPROC>(wglGetProcAddress("glBindAttribLocation"));
    if (!glBindAttribLocation)
    {
        return false;
    }

    glGetUniformLocation = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(wglGetProcAddress("glGetUniformLocation"));
    if (!glGetUniformLocation)
    {
        return false;
    }

    glGetUniformfv = reinterpret_cast<PFNGLGETUNIFORMFVPROC>(wglGetProcAddress("glGetUniformfv"));
    if (!glGetUniformfv)
    {
        return false;
    }

    glUniformMatrix4fv = reinterpret_cast<PFNGLUNIFORMMATRIX4FVPROC>(wglGetProcAddress("glUniformMatrix4fv"));
    if (!glUniformMatrix4fv)
    {
        return false;
    }

    glActiveTexture = reinterpret_cast<PFNGLACTIVETEXTUREPROC>(wglGetProcAddress("glActiveTexture"));
    if (!glActiveTexture)
    {
        return false;
    }

    glUniform1i = reinterpret_cast<PFNGLUNIFORM1IPROC>(wglGetProcAddress("glUniform1i"));
    if (!glUniform1i)
    {
        return false;
    }

    glGenerateMipmap = reinterpret_cast<PFNGLGENERATEMIPMAPPROC>(wglGetProcAddress("glGenerateMipmap"));
    if (!glGenerateMipmap)
    {
        return false;
    }

    glDisableVertexAttribArray = reinterpret_cast<PFNGLDISABLEVERTEXATTRIBARRAYPROC>(wglGetProcAddress("glDisableVertexAttribArray"));
    if (!glDisableVertexAttribArray)
    {
        return false;
    }

    glUniform3fv = reinterpret_cast<PFNGLUNIFORM3FVPROC>(wglGetProcAddress("glUniform3fv"));
    if (!glUniform3fv)
    {
        return false;
    }

    glUniform4fv = reinterpret_cast<PFNGLUNIFORM4FVPROC>(wglGetProcAddress("glUniform4fv"));
    if (!glUniform4fv)
    {
        return false;
    }


    glUniform1f = reinterpret_cast<PFNGLUNIFORM1FPROC>(wglGetProcAddress("glUniform1f"));
    if (!glUniform1f)
    {
        return false;
    }

    glDrawArraysInstanced = reinterpret_cast<PFNGLDRAWARRAYSINSTANCEDPROC>(wglGetProcAddress("glDrawArraysInstanced"));
    if (!glDrawArraysInstanced)
    {
        return false;
    }

    glDrawElementsInstanced = reinterpret_cast<PFNGLDRAWELEMENTSINSTANCEDPROC>(wglGetProcAddress("glDrawElementsInstanced"));
    if (!glDrawElementsInstanced)
    {
        return false;
    }

    glVertexAttribDivisor = reinterpret_cast<PFNGLVERTEXATTRIBDIVISORPROC>(wglGetProcAddress("glVertexAttribDivisor"));
    if (!glVertexAttribDivisor)
    {
        return false;
    }

    return true;
}
