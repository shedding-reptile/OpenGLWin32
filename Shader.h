#pragma once

#include "OpenGL.h"

class Shader
{
public:
	Shader();
	Shader(const Shader&);
	~Shader();
	bool initialize(OpenGL*, HWND);
	void shutdown(OpenGL*) const;
	void setShader(OpenGL*) const;
	bool setShaderParameters(OpenGL*, float*, float*, float*, int, float*, float*, float*) const;

private:
	bool initializeShader(const char*, const char*, OpenGL*, HWND);
	static void outputShaderErrorMessage(OpenGL*, HWND, unsigned int);
	static void outputLinkerErrorMessage(OpenGL*, HWND, unsigned int);
	void shutdownShader(OpenGL*) const;

	unsigned int m_vertexShader;
	unsigned int m_fragmentShader;
	unsigned int m_shaderProgram;
};
