#pragma once

#include "OpenGL.h"
#include "Shader.h"

class Graphics
{
public:
    Graphics(OpenGL* ctx, HWND parent);
    ~Graphics();
    bool render() const;
    void resize(unsigned width, unsigned height) const;

private:

    HWND hWnd;
    OpenGL* context;
    Shader* program;
    float aspect;
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    unsigned int texture;
};
