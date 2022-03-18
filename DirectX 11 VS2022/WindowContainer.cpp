#include "WindowContainer.h"

WindowContainer::WindowContainer()
{
	static bool raw_input_initialized = false;
	if (raw_input_initialized == false)
	{
		//Windows裝置識別:https://docs.microsoft.com/en-us/windows-hardware/drivers/hid/top-level-collections-opened-by-windows-for-system-use
		//結構說明:https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawinputdevice
		RAWINPUTDEVICE rid{};
		rid.usUsagePage = 0x01;	//通用桌面控制組建
		rid.usUsage = 0x02;	//滑鼠
		rid.dwFlags = 0;
		rid.hwndTarget = NULL;

		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
		{
			ErrorLogger::Log(GetLastError(), L"原始設備註冊失敗");
			exit(-1);
		}

		raw_input_initialized = true;
	}
}

LRESULT WindowContainer::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	//鍵盤輸入事件起始
	case WM_KEYDOWN:	//鍵盤按下
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
	case WM_KEYUP://鍵盤彈起
	{
		unsigned char keycode = static_cast<unsigned char>(wParam);
		Keyboard.OnKeyReleased(keycode);
		return 0;
	}
	case WM_CHAR://字元
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
	//鍵盤輸入事件結束
	
	//滑鼠事件起始
	case WM_MOUSEMOVE://滑鼠移動
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		Mouse.OnMouseMove(x, y);
		return 0;
	}
	case WM_LBUTTONDOWN://左鍵敲擊
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		Mouse.OnLeftPressed(x, y);
		return 0;
	}
	case WM_RBUTTONDOWN://右鍵敲擊
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		Mouse.OnRightPressed(x, y);
		return 0;
	}
	case WM_MBUTTONDOWN://滾輪敲擊
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		Mouse.OnMiddlePressed(x, y);
		return 0;
	}
	case WM_LBUTTONUP://左鍵彈起
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		Mouse.OnLeftReleased(x, y);
		return 0;
	}
	case WM_RBUTTONUP://右鍵彈起
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		Mouse.OnRightReleased(x, y);
		return 0;
	}
	case WM_MBUTTONUP://滾輪彈起
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		Mouse.OnMiddleReleased(x, y);
		return 0;
	}
	case WM_MOUSEWHEEL://滾輪事件
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)//滾輪上滑
		{
			Mouse.OnWheelUp(x, y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)//滾輪下滑
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
	//滑鼠事件結束
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}