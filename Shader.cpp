#include "Shader.h"

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
    context->glDetachShader(program, vertShader);
    context->glDetachShader(program, fragShader);
    context->glDeleteShader(vertShader);
    context->glDeleteShader(fragShader);
    context->glDeleteProgram(program);
}

void Shader::useProgram() const
{
    context->glUseProgram(program);
}

bool Shader::initializeShader(HWND hWnd, const char* vertexShaderBuffer, const char* fragmentShaderBuffer)
{
    int status;

    // Vertex shader
    vertShader = context->glCreateShader(GL_VERTEX_SHADER);
    context->glShaderSource(vertShader, 1, &vertexShaderBuffer, nullptr);    
    context->glCompileShader(vertShader);    
    context->glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);
    if (status != 1)
    {
        outputShaderError(hWnd, vertShader);
        return false;
    }

    // Fragment shader
    fragShader = context->glCreateShader(GL_FRAGMENT_SHADER);
    context->glShaderSource(fragShader, 1, &fragmentShaderBuffer, nullptr);
    context->glCompileShader(fragShader);
    context->glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
    if (status != 1)
    {
        outputShaderError(hWnd, fragShader);
        return false;
    }

    // Program
    program = context->glCreateProgram();
    context->glAttachShader(program, vertShader);
    context->glAttachShader(program, fragShader);
    context->glBindAttribLocation(program, 0, "vertex");
    context->glBindAttribLocation(program, 1, "normal");
    context->glLinkProgram(program);
    context->glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != 1)
    {
        outputLinkerError(hWnd, program);
        return false;
    }

    return true;
}

void Shader::outputShaderError(HWND hWnd, unsigned shaderId) const
{
    int logSize;
    context->glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);
    
    std::string log;
    log.resize(logSize);
    context->glGetShaderInfoLog(shaderId, logSize, nullptr, &log[0]);
    showMessage(hWnd, log);
}

void Shader::outputLinkerError(HWND hWnd, unsigned programId) const
{
    int logSize;
    context->glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logSize);

    std::string log;
    log.resize(logSize);
    context->glGetProgramInfoLog(programId, logSize, nullptr, &log[0]);
    showMessage(hWnd, log);
}

void Shader::showMessage(HWND hWnd, std::string& log)
{
    TCHAR* wLog = new TCHAR[log.size()];    
    std::mbstowcs(&wLog[0], log.c_str(), log.size());
    MessageBox(hWnd, wLog, L"Error", MB_OK);
    delete[] wLog;
}
