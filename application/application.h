#pragma once
#include "../global/base.h"
#include <Windows.h>

#define app Application::getInstance()

class Application {

public:

	static Application* getInstance();
	Application();
	~Application();

	bool initApplication(HINSTANCE hInstance, const uint32_t& width = 800, const uint32_t& height = 600);

	void handleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	bool peekMessage();

	// Copy canvasDC to main HDC
	void show();

	uint32_t getWidth() const { return mWidth; }
	uint32_t getHeight() const { return mHeight; }
	void* getCanvas() const { return mCanvasBuffer; }

private:
	BOOL createWindow(HINSTANCE hInstance);
	ATOM registerWindowClass(HINSTANCE hInstance);

private:
	static Application* mInstance;

	bool		mAlive{ true };
	HINSTANCE	mWindowInst;
	WCHAR		mWindowClassName[100] = L"AppWindow";
	HWND		mHwnd;

	int			mWidth = 800;
	int			mHeight = 600;
	
	HDC			mhDC;						// Current window main HDC
	HDC			mCanvasDC;					// second HDC for drawing
	HBITMAP     mhBmp;						// bitmap
	void*		mCanvasBuffer{ nullptr };	// bitmap pointer
};