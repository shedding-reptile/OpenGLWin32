#include "Application.h"
#include "resource.h"
#include <exception>
#include <cwchar>
#include <vector>

INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

Application::Application() :
	instance(nullptr),
	wnd(nullptr),
	title{},
	windowClass{},
	openGLContext(nullptr),
	input(nullptr),
	graphics(nullptr)
{
}

Application::Application(const Application& other) :
	instance(nullptr),
	wnd(nullptr),
	title{},
	windowClass{},
	openGLContext(nullptr),
	input(nullptr),
	graphics(nullptr)
{
}

bool Application::init()
{
	// Create the OpenGL object.
	openGLContext = new OpenGL(wnd);
	if (!openGLContext)
	{
		return false;
	}

	// Create the window the application will be using and also initialize OpenGL.
	int screenWidth = 0;
	int screenHeight = 0;
	if (!initWindow(openGLContext, screenWidth, screenHeight))
	{
		MessageBox(wnd, L"Could not initialize the window.", L"Error", MB_OK);
		return false;
	}

	// Create the input object.  This object will be used to handle reading the input from the user.
	input = new Input;
	if (!input)
	{
		return false;
	}

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	try
	{
		graphics = new Graphics(openGLContext);
	}
	catch (const std::exception& e)
	{
		std::wstring error = strToWstr(e.what());
		MessageBox(wnd, error.c_str(), L"Error", MB_OK);
	}
	
	if (!graphics)
	{
		return false;
	}

	// Initialize the graphics object.
	 return true;
}

void Application::close()
{
	// Release the graphics object.
	if (graphics)
	{
		delete graphics;
		graphics = nullptr;
	}

	// Release the input object.
	if (input)
	{
		delete input;
		input = nullptr;
	}

	// Release the OpenGL object.
	if (openGLContext)
	{
		delete openGLContext;
		openGLContext = nullptr;
	}

	// Shutdown the window.
	closeWindow();
}

void Application::run() const
{
	MSG msg = {};

	// Loop until there is a quit message from the window or the user.
	bool done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			if (!frame())
			{
				done = true;
			}
		}

	}

}

bool Application::frame() const
{
	// Check if the user pressed escape and wants to exit the application.
	if (input->isKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Do the frame processing for the graphics object.
	bool result = graphics->render();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK Application::messageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) const
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(instance, MAKEINTRESOURCE(IDD_ABOUTBOX), wnd, About);
			break;
		case IDM_EXIT:
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc(wnd, message, wParam, lParam);
		}
	}
	break;
	// Check if a key has been pressed on the keyboard.
	case WM_KEYDOWN:
		// If a key is pressed send it to the input object so it can record that state.
		input->keyDown((unsigned int)wParam);
		return 0;

		// Check if a key has been released on the keyboard.
	case WM_KEYUP:
		// If a key is released then send it to the input object so it can unset the state for that key.
		input->keyUp((unsigned int)wParam);
		return 0;
	default:
		;
	}

	// Any other messages send to the default message handler as our application won't make use of them.
	return DefWindowProc(hwnd, message, wParam, lParam);
}

bool Application::initWindow(OpenGL* OpenGL, int& screenWidth, int& screenHeight)
{
	// Get an external pointer to this object.	
	applicationHandle = this;

	// Get the instance of this application.
	instance = GetModuleHandle(nullptr);

	LoadStringW(instance, IDS_APP_TITLE, title, maxLoadString);
	LoadStringW(instance, IDC_OPENGLWIN32, windowClass, maxLoadString);

	// Setup the windows class with default settings.
	WNDCLASSEX wcex;
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_OPENGLWIN32));
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_OPENGLWIN32);
	wcex.lpszClassName = windowClass;
	wcex.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassExW(&wcex);

	// Create a temporary window for the OpenGL extension setup.
	wnd = CreateWindowW(windowClass, title, WS_POPUP,
		0, 0, 640, 480, nullptr, nullptr, instance, nullptr);
	if (wnd == nullptr)
	{
		return false;
	}

	// Don't show the window.
	ShowWindow(wnd, SW_HIDE);

	// Initialize a temporary OpenGL window and load the OpenGL extensions.
	bool result = OpenGL->initializeExtensions(wnd);
	if (!result)
	{
		MessageBox(wnd, L"Could not initialize the OpenGL extensions.", L"Error", MB_OK);
		return false;
	}

	// Release the temporary window now that the extensions have been initialized.
	DestroyWindow(wnd);
	wnd = nullptr;

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Create the window with the screen settings and get the handle to it.
	wnd = CreateWindowW(windowClass, title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, instance, nullptr);
	if (wnd == nullptr)
	{
		return false;
	}

	RECT rect;
	if (GetWindowRect(wnd, &rect))
	{
		screenWidth = rect.right - rect.left;
		screenHeight = rect.bottom - rect.top;
	}

	// Initialize OpenGL now that the window has been created.
	result = openGLContext->initializeOpenGl(wnd, screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, VSYNC_ENABLED);
	if (!result)
	{
		MessageBox(wnd, L"Could not initialize OpenGL, check if video card supports OpenGL 4.0.", L"Error", MB_OK);
		return false;
	}

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(wnd, SW_SHOW);
	SetForegroundWindow(wnd);
	SetFocus(wnd);

	return true;
}

void Application::closeWindow()
{
	// Remove the window.
	DestroyWindow(wnd);
	wnd = nullptr;

	// Remove the application instance.
	UnregisterClass(windowClass, instance);
	instance = nullptr;

	// Release the pointer to this class.
	applicationHandle = nullptr;

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
		return applicationHandle->messageHandler(hWnd, message, wParam, lParam);
	}
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	default: 
		;
	}
	return (INT_PTR)FALSE;
}

std::wstring Application::strToWstr(const std::string str)
{
	const char* mbstr = str.c_str();
	std::mbstate_t state = std::mbstate_t();
	std::size_t len = 1 + std::mbsrtowcs(NULL, &mbstr, 0, &state);
	std::vector<wchar_t> wstr(len);
	std::mbsrtowcs(&wstr[0], &mbstr, wstr.size(), &state);
	return std::wstring(&wstr[0]);
}
