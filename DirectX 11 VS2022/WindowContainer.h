#pragma once
#include "RenderWindow.h"
#include "keyboard\\KeyboardClass.h"
#include "Mouse\\MouseClass.h"
#include "Graphics\\Graphics.h"
#include <memory>
class WindowContainer
{
public:
	WindowContainer();
	LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	RenderWindow RenderWindow;
	KeyboardClass Keyboard;
	MouseClass Mouse;
	Graphics gfx;
private:

};