#define _USE_MATH_DEFINES
#include <cmath>
#include "OpenGL.h"

OpenGL::OpenGL(HWND parent) :
	wglChoosePixelFormatARB(nullptr),
	wglCreateContextAttribsARB(nullptr),
	wglSwapIntervalEXT(nullptr),
	modelMatrix{},
	projectionMatrix{}
{
	deviceContext = nullptr;
	renderingContext = nullptr;
	hWnd = parent;
}

OpenGL::~OpenGL()
{
	// Release the rendering context.
	if (renderingContext)
	{
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(renderingContext);
		renderingContext = nullptr;
	}

	// Release the device context.
	if (deviceContext)
	{
		ReleaseDC(hWnd, deviceContext);
		deviceContext = nullptr;
	}
}

bool OpenGL::initializeExtensions(HWND hwnd)
{
	PIXELFORMATDESCRIPTOR pixelFormat = {};

	// Get the device context for this window.
	HDC deviceContext = GetDC(hwnd);
	if (!deviceContext)
	{
		return false;
	}

	// Set a temporary default pixel format.
	if (SetPixelFormat(deviceContext, 1, &pixelFormat) != 1)
	{
		return false;
	}

	// Create a temporary rendering context.
	HGLRC renderContext = wglCreateContext(deviceContext);
	if (!renderContext)
	{
		return false;
	}

	// Set the temporary rendering context as the current rendering context for this window.
	if (wglMakeCurrent(deviceContext, renderContext) != 1)
	{
		return false;
	}

	// Initialize the OpenGL extensions needed for this application.  Note that a temporary rendering context was needed to do so.
	if (!loadExtensionList())
	{
		return false;
	}

	// Release the temporary rendering context now that the extensions have been loaded.
	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(renderContext);
	renderContext = nullptr;

	// Release the device context for this window.
	ReleaseDC(hwnd, deviceContext);
	deviceContext = nullptr;

	return true;
}

bool OpenGL::initializeOpenGl(HWND hwnd, int screenWidth, int screenHeight, float screenDepth, float screenNear, bool vsync)
{
	int attributeListInt[19];
	int pixelFormat[1];
	unsigned int formatCount;
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor = {};
	int attributeList[5];

	// Get the device context for this window.
	deviceContext = GetDC(hwnd);
	if (!deviceContext)
	{
		return false;
	}

	// Support for OpenGL rendering.
	attributeListInt[0] = WGL_SUPPORT_OPENGL_ARB;
	attributeListInt[1] = TRUE;

	// Support for rendering to a window.
	attributeListInt[2] = WGL_DRAW_TO_WINDOW_ARB;
	attributeListInt[3] = TRUE;

	// Support for hardware acceleration.
	attributeListInt[4] = WGL_ACCELERATION_ARB;
	attributeListInt[5] = WGL_FULL_ACCELERATION_ARB;

	// Support for 24bit color.
	attributeListInt[6] = WGL_COLOR_BITS_ARB;
	attributeListInt[7] = 24;

	// Support for 24 bit depth buffer.
	attributeListInt[8] = WGL_DEPTH_BITS_ARB;
	attributeListInt[9] = 24;

	// Support for double buffer.
	attributeListInt[10] = WGL_DOUBLE_BUFFER_ARB;
	attributeListInt[11] = TRUE;

	// Support for swapping front and back buffer.
	attributeListInt[12] = WGL_SWAP_METHOD_ARB;
	attributeListInt[13] = WGL_SWAP_EXCHANGE_ARB;

	// Support for the RGBA pixel type.
	attributeListInt[14] = WGL_PIXEL_TYPE_ARB;
	attributeListInt[15] = WGL_TYPE_RGBA_ARB;

	// Support for a 8 bit stencil buffer.
	attributeListInt[16] = WGL_STENCIL_BITS_ARB;
	attributeListInt[17] = 8;

	// Null terminate the attribute list.
	attributeListInt[18] = 0;

	// Query for a pixel format that fits the attributes we want.
	if (wglChoosePixelFormatARB(deviceContext, attributeListInt, nullptr, 1, pixelFormat, &formatCount) != 1)
	{
		return false;
	}

	// If the video card/display can handle our desired pixel format then we set it as the current one.
	if (SetPixelFormat(deviceContext, pixelFormat[0], &pixelFormatDescriptor) != 1)
	{
		return false;
	}

	// Set the 4.0 version of OpenGL in the attribute list.
	attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
	attributeList[1] = 4;
	attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
	attributeList[3] = 0;

	// Null terminate the attribute list.
	attributeList[4] = 0;

	// Create a OpenGL 4.0 rendering context.
	renderingContext = wglCreateContextAttribsARB(deviceContext, nullptr, attributeList);
	if (renderingContext == nullptr)
	{
		return false;
	}

	// Set the rendering context to active.
	if (wglMakeCurrent(deviceContext, renderingContext) != 1)
	{
		return false;
	}

	if (!gladLoadGL())
	{
		return false;
	}

	// Set the depth buffer to be entirely cleared to 1.0 values.
	glClearDepth(1.0f);

	// Enable depth testing.
	glEnable(GL_DEPTH_TEST);

	// Set the polygon winding to front facing for the left handed system.
	glFrontFace(GL_CW);

	// Enable back face culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Initialize the world/model matrix to the identity matrix.
	buildIdentityMatrix(modelMatrix);

	// Set the field of view and screen aspect ratio.
	const float fieldOfView = static_cast<float>(M_PI) / 4.0f;
	const float screenAspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

	// Build the perspective projection matrix.
	buildPerspectiveFovLhMatrix(projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	// Get the name of the video card.
	const char* vendorString = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	const char* rendererString = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

	// Store the video card name in a class member variable so it can be retrieved later.
	videoCardDescription = std::string(vendorString) + " - " + std::string(rendererString);

	// Turn on or off the vertical sync depending on the input bool value.
	if (vsync)
	{
		if (wglSwapIntervalEXT(1) != 1)
		{
			return false;
		}
	}
	else
	{
		if (wglSwapIntervalEXT(0) != 1)
		{
			return false;
		}
	}

	return true;
}

void OpenGL::beginScene()
{
	// Set the color to clear the screen to.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Clear the screen and depth buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void OpenGL::endScene() const
{
	// Present the back buffer to the screen since rendering is complete.
	SwapBuffers(deviceContext);

}

bool OpenGL::loadExtensionList()
{
	// Load the OpenGL extensions that this application will be using.
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

	return true;
}

void OpenGL::getWorldMatrix(float* matrix)
{
	matrix[0] = modelMatrix[0];
	matrix[1] = modelMatrix[1];
	matrix[2] = modelMatrix[2];
	matrix[3] = modelMatrix[3];

	matrix[4] = modelMatrix[4];
	matrix[5] = modelMatrix[5];
	matrix[6] = modelMatrix[6];
	matrix[7] = modelMatrix[7];

	matrix[8] = modelMatrix[8];
	matrix[9] = modelMatrix[9];
	matrix[10] = modelMatrix[10];
	matrix[11] = modelMatrix[11];

	matrix[12] = modelMatrix[12];
	matrix[13] = modelMatrix[13];
	matrix[14] = modelMatrix[14];
	matrix[15] = modelMatrix[15];
}

void OpenGL::getProjectionMatrix(float* matrix)
{
	matrix[0] = projectionMatrix[0];
	matrix[1] = projectionMatrix[1];
	matrix[2] = projectionMatrix[2];
	matrix[3] = projectionMatrix[3];

	matrix[4] = projectionMatrix[4];
	matrix[5] = projectionMatrix[5];
	matrix[6] = projectionMatrix[6];
	matrix[7] = projectionMatrix[7];

	matrix[8] = projectionMatrix[8];
	matrix[9] = projectionMatrix[9];
	matrix[10] = projectionMatrix[10];
	matrix[11] = projectionMatrix[11];

	matrix[12] = projectionMatrix[12];
	matrix[13] = projectionMatrix[13];
	matrix[14] = projectionMatrix[14];
	matrix[15] = projectionMatrix[15];
}

std::string OpenGL::getVideoCardInfo() const
{
	return videoCardDescription;
}

void OpenGL::buildIdentityMatrix(float* matrix)
{
	matrix[0] = 1.0f;
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 1.0f;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = 1.0f;
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;

}

void OpenGL::buildPerspectiveFovLhMatrix(float* matrix, float fieldOfView, float screenAspect, float screenNear, float screenDepth) const
{
	matrix[0] = 1.0f / (screenAspect * tan(fieldOfView * 0.5f));
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 1.0f / tan(fieldOfView * 0.5f);
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = screenDepth / (screenDepth - screenNear);
	matrix[11] = 1.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = (-screenNear * screenDepth) / (screenDepth - screenNear);
	matrix[15] = 0.0f;

}

void OpenGL::matrixRotationY(float* matrix, float angle)
{
	matrix[0] = cosf(angle);
	matrix[1] = 0.0f;
	matrix[2] = -sinf(angle);
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 1.0f;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = sinf(angle);
	matrix[9] = 0.0f;
	matrix[10] = cosf(angle);
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;

}

void OpenGL::matrixTranslation(float* matrix, float x, float y, float z)
{
	matrix[0] = 1.0f;
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 1.0f;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = 1.0f;
	matrix[11] = 0.0f;

	matrix[12] = x;
	matrix[13] = y;
	matrix[14] = z;
	matrix[15] = 1.0f;
}

void OpenGL::matrixMultiply(float* result, float* matrix1, float* matrix2)
{
	result[0] = (matrix1[0] * matrix2[0]) + (matrix1[1] * matrix2[4]) + (matrix1[2] * matrix2[8]) + (matrix1[3] * matrix2[12]);
	result[1] = (matrix1[0] * matrix2[1]) + (matrix1[1] * matrix2[5]) + (matrix1[2] * matrix2[9]) + (matrix1[3] * matrix2[13]);
	result[2] = (matrix1[0] * matrix2[2]) + (matrix1[1] * matrix2[6]) + (matrix1[2] * matrix2[10]) + (matrix1[3] * matrix2[14]);
	result[3] = (matrix1[0] * matrix2[3]) + (matrix1[1] * matrix2[7]) + (matrix1[2] * matrix2[11]) + (matrix1[3] * matrix2[15]);

	result[4] = (matrix1[4] * matrix2[0]) + (matrix1[5] * matrix2[4]) + (matrix1[6] * matrix2[8]) + (matrix1[7] * matrix2[12]);
	result[5] = (matrix1[4] * matrix2[1]) + (matrix1[5] * matrix2[5]) + (matrix1[6] * matrix2[9]) + (matrix1[7] * matrix2[13]);
	result[6] = (matrix1[4] * matrix2[2]) + (matrix1[5] * matrix2[6]) + (matrix1[6] * matrix2[10]) + (matrix1[7] * matrix2[14]);
	result[7] = (matrix1[4] * matrix2[3]) + (matrix1[5] * matrix2[7]) + (matrix1[6] * matrix2[11]) + (matrix1[7] * matrix2[15]);

	result[8] = (matrix1[8] * matrix2[0]) + (matrix1[9] * matrix2[4]) + (matrix1[10] * matrix2[8]) + (matrix1[11] * matrix2[12]);
	result[9] = (matrix1[8] * matrix2[1]) + (matrix1[9] * matrix2[5]) + (matrix1[10] * matrix2[9]) + (matrix1[11] * matrix2[13]);
	result[10] = (matrix1[8] * matrix2[2]) + (matrix1[9] * matrix2[6]) + (matrix1[10] * matrix2[10]) + (matrix1[11] * matrix2[14]);
	result[11] = (matrix1[8] * matrix2[3]) + (matrix1[9] * matrix2[7]) + (matrix1[10] * matrix2[11]) + (matrix1[11] * matrix2[15]);

	result[12] = (matrix1[12] * matrix2[0]) + (matrix1[13] * matrix2[4]) + (matrix1[14] * matrix2[8]) + (matrix1[15] * matrix2[12]);
	result[13] = (matrix1[12] * matrix2[1]) + (matrix1[13] * matrix2[5]) + (matrix1[14] * matrix2[9]) + (matrix1[15] * matrix2[13]);
	result[14] = (matrix1[12] * matrix2[2]) + (matrix1[13] * matrix2[6]) + (matrix1[14] * matrix2[10]) + (matrix1[15] * matrix2[14]);
	result[15] = (matrix1[12] * matrix2[3]) + (matrix1[13] * matrix2[7]) + (matrix1[14] * matrix2[11]) + (matrix1[15] * matrix2[15]);
}

