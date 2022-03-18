#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, std::wstring window_title, std::wstring window_class, int width, int height)
{
	if (!this->RenderWindow.Initialize(this, hInstance, window_title, window_class, width, height))
		return false;

	if (!gfx.Initialize(this->RenderWindow.GetHWND(), width, height))
		return false;

	return true;
}

bool Engine::ProcessMessages()
{
	return this->RenderWindow.ProcessMessages();
}

void Engine::update()
{
	while (!Keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = Keyboard.ReadChar();
	}
	
	while (!Keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = Keyboard.ReadKey();
		unsigned char Keycode = kbe.GetKeyCode();
	}

	while (!Mouse.EventBufferIsEmpty())
	{
		MouseEvent MouseEvent = Mouse.ReadEvent();
		//if (MouseEvent.GetType() == MouseEvent::EventType::RAW_MOVE)
		//{
		//	std::wstring outMsg = L"X:";
		//	outMsg += std::to_wstring(MouseEvent.GetPosX());
		//	outMsg += L", Y:";
		//	outMsg += std::to_wstring(MouseEvent.GetPosY());
		//	outMsg += L"\n";
		//	OutputDebugString(outMsg.c_str());
		//}
	}
}

void Engine::RenderFrame()
{
	gfx.RenderFrame();
}
