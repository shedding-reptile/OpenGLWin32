#include "framework.h"
#include "OpenGLWin32.h"
#include "Application.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	// Create the system object.
	Application* app = new Application;
	if (!app)
	{
		return 0;
	}

	// Initialize and run the system object.
	if (app->init())
	{
		app->run();
	}

	// Shutdown and release the system object.
	app->close();
	delete app;
	app = nullptr;

	return 0;
}
