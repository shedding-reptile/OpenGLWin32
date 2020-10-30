#pragma once

#include <windows.h>
#include "glad/glad.h"
#include "WGL/wgl.h"
#include <glm/glm.hpp>
#include <string>

class OpenGL
{
public:
	OpenGL(HWND parent);
	~OpenGL();
	bool initializeExtensions(HWND hwnd);
	bool initializeOpenGl(HWND hwnd, int screenWidth, int screenHeight, float screenDepth, float screenNear, bool vsync);
	static void beginScene();
	void endScene() const;
	glm::mat4 getModelMatrix();
	glm::mat4 getProjectionMatrix();
	std::string getVideoCardInfo() const;

private:
	bool loadExtensionList();

	HWND hWnd;
	HDC deviceContext;
	HGLRC renderingContext;
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
	glm::mat4 modelMatrix;
	glm::mat4 projectionMatrix;
	std::string videoCardDescription;
};
