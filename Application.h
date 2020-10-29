#pragma once

#include "Input.h"
#include "Graphics.h"
#include <string>

class Application
{
public:
	Application();
	~Application();
	void run() const;
	LRESULT CALLBACK messageHandler(HWND, UINT, WPARAM, LPARAM) const;
	
private:
	bool frame() const;
	bool initWindow(OpenGL*, int&, int&);
	void closeWindow();
    static std::wstring strToWstr(std::string str);

	HINSTANCE instance;
	HWND wnd;
	static const int maxLoadString = 100;
	WCHAR title[maxLoadString];
	WCHAR windowClass[maxLoadString];
	OpenGL* openGLContext;
	Input* input;
	Graphics* graphics;
	bool isInit;
};

static Application* applicationHandle = nullptr;
