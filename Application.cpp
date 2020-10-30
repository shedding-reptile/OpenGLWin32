#include "Application.h"
#include "resource.h"
#include "shobjidl_core.h"
#include "atlstr.h"
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
	graphics(nullptr),
	isInit(false),
	isClosing(false)
{
	openGLContext = new OpenGL(wnd);

	int screenWidth = 0;
	int screenHeight = 0;
	if (!initWindow(openGLContext, screenWidth, screenHeight))
	{
		MessageBox(wnd, L"Could not initialize the window.", L"Error", MB_OK);
		return;
	}

	try
	{
		graphics = new Graphics(openGLContext);
		isInit = true;
	}
	catch (const std::exception& e)
	{
		const std::wstring error = strToWstr(e.what());
		MessageBox(wnd, error.c_str(), L"Error", MB_OK);
	}
}

Application::~Application()
{
	if (graphics)
	{
		delete graphics;
	}

	if (openGLContext)
	{
		delete openGLContext;
	}

	closeWindow();
}

void Application::run() const
{
	if (!isInit)
	{
		return;
	}

	MSG msg = {};

	bool done = false;
	while (!done)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			if (!frame())
			{
				done = true;
			}
		}
	}
}

bool Application::frame() const
{
	if (isClosing)
	{
		return false;
	}

	if (input->isKeyDown(VK_UP))
	{
		graphics->move(Direction::Up);
	}

	if (input->isKeyDown(VK_DOWN))
	{
		graphics->move(Direction::Down);
	}

	if (input->isKeyDown(VK_LEFT))
	{
		graphics->move(Direction::Left);
	}

	if (input->isKeyDown(VK_RIGHT))
	{
		graphics->move(Direction::Right);
	}

	if (input->isKeyDown(VK_SPACE))
	{
		graphics->createDynamic();
		input->keyUp(VK_SPACE);
	}

	// Do the frame processing for the graphics object.
	if (!graphics->render())
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK Application::messageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		const int wmId = LOWORD(wParam);
		
		switch (wmId)
		{
		case ID_FILE_LOADMODEL:
		{
			std::string file;
			const std::wstring ext = L"*.STL";
			const std::wstring type = L"STL files";
			HRESULT hr = openFile(type, ext, file);
			if (SUCCEEDED(hr))
			{
				if (!graphics->load(file))
				{
					MessageBox(wnd, L"Could not load file.", L"Error", MB_OK);
				}
			}			
		}			
			break;
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
	
	case WM_KEYDOWN:
		switch (wParam)
		{

		case VK_ESCAPE:
			isClosing = true;
			break;

		case VK_UP:
			graphics->move(Direction::Up);
			break;

		case VK_DOWN:
			graphics->move(Direction::Down);
			break;

		case VK_LEFT:
			graphics->move(Direction::Left);
			break;

		case VK_RIGHT:
			graphics->move(Direction::Right);
			break;

		default:
			break;

		}
		return 0;

	default:
		;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

bool Application::initWindow(OpenGL* OpenGL, int& screenWidth, int& screenHeight)
{
	applicationHandle = this;

	instance = GetModuleHandle(nullptr);

	LoadStringW(instance, IDS_APP_TITLE, title, maxLoadString);
	LoadStringW(instance, IDC_OPENGLWIN32, windowClass, maxLoadString);

	WNDCLASSEX wcex;
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_OPENGLWIN32));
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_OPENGLWIN32);
	wcex.lpszClassName = windowClass;
	wcex.cbSize = sizeof(WNDCLASSEX);

	RegisterClassExW(&wcex);

	wnd = CreateWindowW(windowClass, title, WS_POPUP, 0, 0, 640, 480, nullptr, nullptr, instance, nullptr);
	if (wnd == nullptr)
	{
		return false;
	}

	ShowWindow(wnd, SW_HIDE);

	if (!OpenGL->initializeExtensions(wnd))
	{
		MessageBox(wnd, L"Could not initialize the OpenGL extensions.", L"Error", MB_OK);
		return false;
	}

	DestroyWindow(wnd);
	wnd = nullptr;

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	wnd = CreateWindowW(windowClass, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, instance, nullptr);
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

	if (!openGLContext->initializeOpenGl(wnd, screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, VSYNC_ENABLED))
	{
		MessageBox(wnd, L"Could not initialize OpenGL, check if video card supports OpenGL 4.0.", L"Error", MB_OK);
		return false;
	}

	ShowWindow(wnd, SW_SHOW);
	SetForegroundWindow(wnd);
	SetFocus(wnd);

	return true;
}

void Application::closeWindow()
{
	DestroyWindow(wnd);
	wnd = nullptr;

	UnregisterClass(windowClass, instance);
	instance = nullptr;

	applicationHandle = nullptr;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	default:
		return applicationHandle->messageHandler(hWnd, message, wParam, lParam);
	}
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return static_cast<INT_PTR>(TRUE);

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return static_cast<INT_PTR>(TRUE);
		}
		break;
	default:
		;
	}
	return static_cast<INT_PTR>(FALSE);
}

std::wstring Application::strToWstr(const std::string str)
{
	const char* mbstr = str.c_str();
	std::mbstate_t state = std::mbstate_t();
	const std::size_t len = 1 + std::mbsrtowcs(nullptr, &mbstr, 0, &state);
	std::vector<wchar_t> wstr(len);
	std::mbsrtowcs(&wstr[0], &mbstr, wstr.size(), &state);

	return std::wstring(&wstr[0]);
}

HRESULT Application::openFile(const std::wstring& fileTypeName, const std::wstring& fileTypeExt, std::string& filePath)
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* fileOpenDlg;

		hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&fileOpenDlg));
		if (FAILED(hr))
		{
			return hr;
		}

		COMDLG_FILTERSPEC filter[] =
		{
			{ fileTypeName.c_str(), fileTypeExt.c_str() },
			{ L"All", L"*.*" },
		};
		hr = fileOpenDlg->SetFileTypes(sizeof(filter) / sizeof(COMDLG_FILTERSPEC), filter);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = fileOpenDlg->Show(nullptr);

		if (SUCCEEDED(hr))
		{
			IShellItem* item;
			hr = fileOpenDlg->GetResult(&item);
			if (SUCCEEDED(hr))
			{
				PWSTR file;
				hr = item->GetDisplayName(SIGDN_FILESYSPATH, &file);

				if (SUCCEEDED(hr))
				{
					filePath = std::string(CW2A(file));
					CoTaskMemFree(file);
				}
				item->Release();
			}
		}
		fileOpenDlg->Release();

		CoUninitialize();
	}

	return hr;
}
