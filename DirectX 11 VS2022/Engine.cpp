#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, std::wstring window_title, std::wstring window_class, int width, int height)
{
	timer.Start();

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
	float dt = (float)timer.GetMilisecondsElapsed();
	timer.Restart();

	static float cameraSpeed = 0.004f;
	
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
		MouseEvent me = Mouse.ReadEvent();
		if (Mouse.IsRightDown())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				this->gfx.camera.AdjustRotation((float)me.GetPosY() * 0.01f, (float)me.GetPosX() * 0.01f, 0);
			}
			else if (me.GetType() == MouseEvent::EventType::WheelUp)
			{
				this->gfx.camera.AdjustPosition(this->gfx.camera.GetForwardVector() * cameraSpeed * 7.50f);
			}
			else if (me.GetType() == MouseEvent::EventType::WheelDown)
			{
				this->gfx.camera.AdjustPosition(this->gfx.camera.GetBackwardVector() * cameraSpeed * 7.50f);
			}
		}
	}

	this->gfx.gameObject.AdjustRotation(0.0f, 0.001f * dt / 1.0f, 0.0f);

 	if (Keyboard.KeyIsPressed(VK_OEM_PLUS))
	{
		cameraSpeed += 0.1f;
	}

	if (Keyboard.KeyIsPressed(VK_OEM_MINUS))
	{
		if (cameraSpeed - 0.1f > 0)
			cameraSpeed -= 0.1f;
	}

	if (Keyboard.KeyIsPressed('W'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetForwardVector() * cameraSpeed * dt);
	}
	if (Keyboard.KeyIsPressed('S'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetBackwardVector() * cameraSpeed * dt);
	}
	if (Keyboard.KeyIsPressed('A'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetLeftVector() * cameraSpeed * dt);
	}
	if (Keyboard.KeyIsPressed('D'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetRightVector() * cameraSpeed * dt);
	}
	if (Keyboard.KeyIsPressed(VK_SPACE))
	{
		this->gfx.camera.AdjustPosition(0.0f, cameraSpeed * dt, 0.0f);
	}
	if (Keyboard.KeyIsPressed('Z'))
	{
		this->gfx.camera.AdjustPosition(0.0f, -cameraSpeed * dt, 0.0f);
	}
}

void Engine::RenderFrame()
{
	this->gfx.RenderFrame();
}
