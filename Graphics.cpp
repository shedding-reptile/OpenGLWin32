#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "Graphics.h"
#include "resource.h"
#include "Application.h"

Graphics::Graphics(OpenGL* ctx, HWND parent) :
    hWnd(nullptr),
    program(nullptr),
    aspect(1.0f)
{
    context = ctx;
    hWnd = parent;

    RECT rect;
    if (GetWindowRect(hWnd, &rect))
    {
        const int width = rect.right - rect.left;
        const int height = rect.bottom - rect.top;

        aspect = static_cast<float>(width) / static_cast<float>(height);
    }

    program = new Shader(context, hWnd, IDR_SHADER_V, IDR_SHADER_F);

    float vertices[] =
    {
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };
    unsigned int indices[] =
    {
        0, 1, 3,
        1, 2, 3
    };

    context->glGenVertexArrays(1, &vao);
    context->glGenBuffers(1, &vbo);
    context->glGenBuffers(1, &ebo);

    context->glBindVertexArray(vao);

    context->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    context->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    context->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
    context->glEnableVertexAttribArray(0);
    
    context->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    context->glEnableVertexAttribArray(1);
    
    context->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    context->glEnableVertexAttribArray(2);
            
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const HBITMAP bmpFile = LoadBitmap(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDB_BITMAP1));
    const HDC dc = CreateCompatibleDC(GetDC(nullptr));
    SelectObject(dc, bmpFile);
    BITMAP bmp;
    GetObject(bmpFile, sizeof(BITMAP), &bmp);

    BITMAPINFOHEADER bmpInfo;
    bmpInfo.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.biWidth = bmp.bmWidth;
    bmpInfo.biHeight = bmp.bmHeight;
    bmpInfo.biPlanes = 1;
    bmpInfo.biBitCount = bmp.bmBitsPixel;
    bmpInfo.biCompression = BI_RGB;
    bmpInfo.biSizeImage = 0;
    bmpInfo.biXPelsPerMeter = 0;
    bmpInfo.biYPelsPerMeter = 0;
    bmpInfo.biClrUsed = 0;
    bmpInfo.biClrImportant = 0;

    const DWORD size = ((bmp.bmWidth * bmpInfo.biBitCount + 31) / 32) * 4 * bmp.bmHeight;
    const HANDLE dib = GlobalAlloc(GHND, size);
    if(dib)
    {
        char* data = static_cast<char*>(GlobalLock(dib));

        if(data)
        {
            GetDIBits(dc, bmpFile, 0, static_cast<UINT>(bmp.bmHeight), data, reinterpret_cast<BITMAPINFO*>(&bmpInfo), DIB_RGB_COLORS);
            const int width = bmp.bmWidth;
            const int height = bmp.bmHeight;

            if (data)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
                context->glGenerateMipmap(GL_TEXTURE_2D);
            }

            GlobalUnlock(data);
        }
    }
   
   GlobalFree(dib);
}

Graphics::~Graphics()
{
    delete program;
}

bool Graphics::render() const
{
    program->useProgram();

    context->beginScene(0.3f, 0.3f, 0.3f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, texture);
    
    context->glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
 
    context->endScene();

    return true;
}

void Graphics::resize(unsigned width, unsigned height) const
{
    if (context)
    {
        unsigned w;
        unsigned h;

        if (width / aspect <= height)
        {
            w = static_cast<unsigned>(height * aspect);
            h = height;
        }
        else
        {
            w = width;
            h = static_cast<unsigned>(width / aspect);
        }
        context->resize(w, h);
    }
}
