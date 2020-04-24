#include "Shader.h"

#include <GL/glcorearb.h>

Shader::Shader(OpenGL* ctx, HWND hWnd, int vertexShader, int fragmentShader) :
    vertShader(0),
    fragShader(0),
    program(0)
{
    context = ctx;
    HRSRC hRes = FindResource(nullptr, MAKEINTRESOURCE(vertexShader), L"SHADER");
    LPVOID vs = nullptr;
    if (hRes)
    {
        const HGLOBAL hData = LoadResource(nullptr, hRes);
        if (hData)
        {
            vs = LockResource(hData);
        }
    }

    LPVOID fs = nullptr;
    hRes = FindResource(nullptr, MAKEINTRESOURCE(fragmentShader), L"SHADER");
    if (hRes)
    {
        const HGLOBAL hData = LoadResource(nullptr, hRes);
        if (hData)
        {
            fs = LockResource(hData);
        }
    }

    if (!vs || !fs)
    {
        throw TEXT("Cannot load shaders");
    }

    if (!initializeShader(hWnd, static_cast<char*>(vs), static_cast<char*>(fs)))
    {
        throw TEXT("Cannot initialize shaders");
    }
}

Shader::~Shader()
{
    glDetachShader(program, vertShader);
    glDetachShader(program, fragShader);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    glDeleteProgram(program);
}

void Shader::useProgram() const
{
    glUseProgram(program);
}

bool Shader::initializeShader(HWND hWnd, const char* vertexShaderBuffer, const char* fragmentShaderBuffer)
{
    int status;

    // Vertex shader
    vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertexShaderBuffer, nullptr);    
    glCompileShader(vertShader);    
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);
    if (status != 1)
    {
        outputShaderError(hWnd, vertShader);
        return false;
    }

    // Fragment shader
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragmentShaderBuffer, nullptr);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
    if (status != 1)
    {
        outputShaderError(hWnd, fragShader);
        return false;
    }

    // Program
    program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glBindAttribLocation(program, 0, "vertex");
    glBindAttribLocation(program, 1, "normal");
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != 1)
    {
        outputLinkerError(hWnd, program);
        return false;
    }

    return true;
}

void Shader::outputShaderError(HWND hWnd, unsigned shaderId)
{
    int logSize;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);
    
    std::string log;
    log.resize(logSize);
    glGetShaderInfoLog(shaderId, logSize, nullptr, &log[0]);
    showMessage(hWnd, log);
}

void Shader::outputLinkerError(HWND hWnd, unsigned programId)
{
    int logSize;
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logSize);

    std::string log;
    log.resize(logSize);
    glGetProgramInfoLog(programId, logSize, nullptr, &log[0]);
    showMessage(hWnd, log);
}

void Shader::showMessage(HWND hWnd, std::string& log)
{
    TCHAR* wLog = new TCHAR[log.size()];    
    std::mbstowcs(&wLog[0], log.c_str(), log.size());
    MessageBox(hWnd, wLog, L"Error", MB_OK);
    delete[] wLog;
}
