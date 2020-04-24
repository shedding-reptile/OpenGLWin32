#include "Application.h"
#include "resource.h"

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Application::Application() :
    hInstance(nullptr),
    hWnd(nullptr),
    title{},
    className{},
    context(nullptr),
    graph(nullptr)
{
    context = new OpenGL;

    if (!initWindow())
    {
        throw TEXT("Could not initialize the window.");
    }

    graph = new Graphics(context, hWnd);

    resizeViewport();
}

Application::~Application()
{
    delete graph;
    delete context;

    closeWindow();
}

bool Application::initWindow()
{
    applicationHandle = this;

    hInstance = GetModuleHandle(nullptr);

    LoadString(hInstance, IDS_APP_TITLE, title, maxLoadString);
    LoadString(hInstance, IDC_OPENGL2DWIN32, className, maxLoadString);

    WNDCLASSEX wcex;
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_OPENGL2DWIN32));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = className;
    wcex.cbSize = sizeof(WNDCLASSEX);

    RegisterClassEx(&wcex);

    hWnd = CreateWindow(className, title, WS_POPUP, 0, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);
    if (hWnd == nullptr)
    {
        return false;
    }

    ShowWindow(hWnd, SW_HIDE);

    if (!context->initExt(hWnd))
    {
        MessageBox(hWnd, L"Could not initialize the OpenGL extensions.", L"Error", MB_OK);
        return false;
    }

    DestroyWindow(hWnd);

    hWnd = CreateWindow(className, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {
        return false;
    }

    if (!context->init(hWnd))
    {
        MessageBox(hWnd, L"Could not initialize OpenGL, check if video card supports OpenGL 4.0.", L"Error", MB_OK);
        return false;
    }

    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);

    return true;
}

void Application::closeWindow()
{
    DestroyWindow(hWnd);
    hWnd = nullptr;

    UnregisterClass(className, hInstance);
    hInstance = nullptr;

    applicationHandle = nullptr;
}

void Application::run() const
{
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
    if (graph && graph->render())
    {
        return true;
    }

    return false;
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
        break;
    }

    return static_cast<INT_PTR>(FALSE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

    default:
        return applicationHandle->wndMsgHandler(hWnd, message, wParam, lParam);
    }
}

LRESULT CALLBACK Application::wndMsgHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) const
{
    switch (message)
    {
    case WM_SIZE:
        resizeViewport();

    default:
        if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
            return true;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

void Application::resizeViewport() const
{
    if (graph)
    {
        RECT rcClient;
        GetClientRect(hWnd, &rcClient);
        graph->resize(rcClient.right, rcClient.bottom);
    }
}
