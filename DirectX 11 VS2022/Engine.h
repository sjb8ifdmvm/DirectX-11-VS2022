#pragma once
#include "WindowContainer.h"
#include "Timer.h"

class Engine:WindowContainer
{
public:
	bool Initialize(HINSTANCE hInstance, std::wstring window_title, std::wstring window_class, int width, int height);
	bool ProcessMessages();

	//��o�Ӧ���L���ƥ�
	void update();

	//��V�ج[
	void RenderFrame();
	
private:
	Timer timer; //FPS�p�ƾ�
};