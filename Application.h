#pragma once

#include "OpenGL.h"
#include "Input.h"
#include "Graphics.h"

class Application
{
public:
	Application();
	Application(const Application&);
	~Application() = default;
	bool init();
	void close();
	void run() const;
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM) const;

private:
	bool frame() const;
	bool initWindow(OpenGL*, int&, int&);
	void closeWindow();

	HINSTANCE hInstance;
	HWND hWnd;
	static const int maxLoadString = 100;
	HINSTANCE hInst;
	WCHAR szTitle[maxLoadString];
	WCHAR szWindowClass[maxLoadString];
	OpenGL* m_OpenGL;
	Input* m_Input;
	Graphics* m_Graphics;
};

static Application* applicationHandle = nullptr;
