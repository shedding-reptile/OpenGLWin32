#pragma once

#include "Graphics.h"
#include <string>
#include <atomic>

class Application
{
public:
	Application();
	~Application();
	void run() const;
	LRESULT CALLBACK messageHandler(HWND, UINT, WPARAM, LPARAM);
	
private:
	bool frame() const;
	bool initWindow(OpenGL*, int&, int&);
	void closeWindow();
    
	HINSTANCE instance;
	HWND wnd;
	static const int maxLoadString = 100;
	WCHAR title[maxLoadString];
	WCHAR windowClass[maxLoadString];
	OpenGL* openGLContext;
	Graphics* graphics;
	bool isInit;
	const bool VSYNC_ENABLED = true;
	const float SCREEN_DEPTH = 1000.0f;
	const float SCREEN_NEAR = 0.1f;
	std::atomic<bool> isClosing;
};

HRESULT openFile(const std::wstring& fileTypeName, const std::wstring& fileTypeExt, std::string& filePath);
static std::wstring strToWstr(std::string str);
static Application* applicationHandle = nullptr;
