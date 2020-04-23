#pragma once

#include "OpenGL.h"
#include <string>

class Shader
{
public:
    Shader(OpenGL* ctx, HWND hWnd, int vertexShader, int fragmentShader);
    ~Shader();
    void useProgram() const;

private:
    bool initializeShader(HWND hWnd, const char* vertexShaderBuffer, const char* fragmentShaderBuffer);
    void outputShaderError(HWND hWnd, unsigned shaderId) const;
    void outputLinkerError(HWND hWnd, unsigned programId) const;
    static void showMessage(HWND hWnd, std::string& log);

    unsigned vertShader;
    unsigned fragShader;
    unsigned program;
    OpenGL* context;
};
