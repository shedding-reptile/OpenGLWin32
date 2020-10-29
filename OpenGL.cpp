#define _USE_MATH_DEFINES
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "OpenGL.h"

OpenGL::OpenGL(HWND parent) :
	wglChoosePixelFormatARB(nullptr),
	wglCreateContextAttribsARB(nullptr),
	wglSwapIntervalEXT(nullptr),
	projectionMatrix{},
	deviceContext(nullptr),
	renderingContext(nullptr)
{
	hWnd = parent;
	modelMatrix = glm::mat4(1.0f);
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

	// Set the field of view and screen aspect ratio.
	const float fieldOfView = static_cast<float>(M_PI) / 4.0f;
	const float screenAspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

	// Build the perspective projection matrix.
	projectionMatrix = glm::perspective(fieldOfView, screenAspect, screenNear, screenDepth);

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

glm::mat4 OpenGL::getModelMatrix()
{
	return modelMatrix;
}

glm::mat4 OpenGL::getProjectionMatrix()
{
	return projectionMatrix;
}

std::string OpenGL::getVideoCardInfo() const
{
	return videoCardDescription;
}
