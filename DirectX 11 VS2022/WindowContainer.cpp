#include "WindowContainer.h"

WindowContainer::WindowContainer()
{
	static bool raw_input_initialized = false;
	if (raw_input_initialized == false)
	{
		//Windows�˸m�ѧO:https://docs.microsoft.com/en-us/windows-hardware/drivers/hid/top-level-collections-opened-by-windows-for-system-use
		//���c����:https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawinputdevice
		RAWINPUTDEVICE rid{};
		rid.usUsagePage = 0x01;	//�q�ήୱ����ի�
		rid.usUsage = 0x02;	//�ƹ�
		rid.dwFlags = 0;
		rid.hwndTarget = NULL;

		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
		{
			ErrorLogger::Log(GetLastError(), L"��l�]�Ƶ��U����");
			exit(-1);
		}

		raw_input_initialized = true;
	}
}

LRESULT WindowContainer::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	//��L��J�ƥ�_�l
	case WM_KEYDOWN:	//��L���U
	{
		unsigned char keycode = static_cast<unsigned char>(wParam);
		if (Keyboard.IsKeyAutoRepeat())
			Keyboard.OnKeyPressed(keycode);
		else
		{
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed)
				Keyboard.OnKeyPressed(keycode);
		}
		return 0;
	}
	case WM_KEYUP://��L�u�_
	{
		unsigned char keycode = static_cast<unsigned char>(wParam);
		Keyboard.OnKeyReleased(keycode);
		return 0;
	}
	case WM_CHAR://�r��
	{
		unsigned char ch = static_cast<unsigned char>(wParam);
		if (Keyboard.IsCharsAutoRepeat())
			Keyboard.OnChar(ch);
		else
		{
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed)
				Keyboard.OnChar(ch);
		}
		return 0;
	}
	//��L��J�ƥ󵲧�
	
	//�ƹ��ƥ�_�l
	case WM_MOUSEMOVE://�ƹ�����
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		Mouse.OnMouseMove(x, y);
		return 0;
	}
	case WM_LBUTTONDOWN://����V��
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		Mouse.OnLeftPressed(x, y);
		return 0;
	}
	case WM_RBUTTONDOWN://�k��V��
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		Mouse.OnRightPressed(x, y);
		return 0;
	}
	case WM_MBUTTONDOWN://�u���V��
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		Mouse.OnMiddlePressed(x, y);
		return 0;
	}
	case WM_LBUTTONUP://����u�_
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		Mouse.OnLeftReleased(x, y);
		return 0;
	}
	case WM_RBUTTONUP://�k��u�_
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		Mouse.OnRightReleased(x, y);
		return 0;
	}
	case WM_MBUTTONUP://�u���u�_
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		Mouse.OnMiddleReleased(x, y);
		return 0;
	}
	case WM_MOUSEWHEEL://�u���ƥ�
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)//�u���W��
		{
			Mouse.OnWheelUp(x, y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)//�u���U��
		{
			Mouse.OnWheelDown(x, y);
		}
		return 0;
	}
	case WM_INPUT:
	{
		UINT dataSize = 0;

		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));
		
		if (dataSize > 0)
		{
			std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER) == dataSize))
			{
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
				if (raw->header.dwType == RIM_TYPEMOUSE)
				{
					Mouse.OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
				}
			}
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	//�ƹ��ƥ󵲧�
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}