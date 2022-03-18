#pragma once
#include "WindowContainer.h"

class Engine:WindowContainer
{
public:
	bool Initialize(HINSTANCE hInstance, std::wstring window_title, std::wstring window_class, int width, int height);
	bool ProcessMessages();

	//獲得來自鍵盤的事件
	void update();

	//渲染框架
	void RenderFrame();
};