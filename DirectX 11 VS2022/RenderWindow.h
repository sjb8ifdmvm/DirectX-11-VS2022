#pragma once
#include "ErrorLogger.h"

class WindowContainer;

class RenderWindow
{
public:
	//�w�q���f��l��(���, ���D, ���O, XSize, YSize)
	bool Initialize(WindowContainer *pWindowContainer, HINSTANCE hInstance, std::wstring window_title, std::wstring window_class, int width, int height);
	
	//�������f�����AFALSE = ���f�w�����A TRUE = ���f���B��
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