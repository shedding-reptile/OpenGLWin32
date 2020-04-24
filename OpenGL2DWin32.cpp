#include "framework.h"
#include "OpenGL2DWin32.h"
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

    try
    {
        const Application app;
        app.run();
    }
    catch (const TCHAR* e)
    {
    	MessageBox(nullptr, e, L"Error", MB_OK);
    }
	
	return 0;
}
