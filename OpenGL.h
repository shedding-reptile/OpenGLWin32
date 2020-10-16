#pragma once

#include <windows.h>
#include "glad/glad.h"
#include "WGL/wgl.h"
#include <string>

class OpenGL
{
public:
	OpenGL(HWND parent);
	~OpenGL();
	bool initializeExtensions(HWND);
	bool initializeOpenGl(HWND, int, int, float, float, bool);
	static void beginScene();
	void endScene() const;
	void getWorldMatrix(float*);
	void getProjectionMatrix(float*);
	std::string getVideoCardInfo();
	static void buildIdentityMatrix(float*);
	void buildPerspectiveFovLhMatrix(float*, float, float, float, float) const;
	static void matrixRotationY(float*, float);
	static void matrixTranslation(float*, float, float, float);
	static void matrixMultiply(float*, float*, float*);

private:
	bool loadExtensionList();

	HWND hWnd;
	HDC deviceContext;
	HGLRC renderingContext;
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
	float modelMatrix[16];
	float projectionMatrix[16];
	std::string videoCardDescription;
};
