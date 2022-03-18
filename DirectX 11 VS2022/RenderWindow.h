#pragma once
#include "ErrorLogger.h"

class WindowContainer;

class RenderWindow
{
public:
	//定義窗口初始化(實例, 標題, 類別, XSize, YSize)
	bool Initialize(WindowContainer *pWindowContainer, HINSTANCE hInstance, std::wstring window_title, std::wstring window_class, int width, int height);
	
	//接收窗口消息，FALSE = 窗口已關閉， TRUE = 窗口仍運行
	bool ProcessMessages();

	HWND GetHWND() const;

	RenderWindow();
	~RenderWindow();
private:
	void RegisterWindowClass();
	HWND handle;
	HINSTANCE hInstance;
	std::wstring window_title;
	std::wstring window_class;
	int width;
	int height;
};