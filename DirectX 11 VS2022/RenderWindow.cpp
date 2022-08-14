#include "WindowContainer.h"


LRESULT CALLBACK HandleMessageSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HandleMsgRedirect(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

RenderWindow::RenderWindow() :
	handle(NULL),
	hInstance(NULL),
	window_title(L""),
	//window_title_wide(L""),
	window_class(L""),
	//window_class_wide(L""),
	width(0),
	height(0)
{
}

RenderWindow::~RenderWindow()
{
	if (this->handle != NULL)
	{
		UnregisterClass(this->window_class.c_str(), this->hInstance);
		DestroyWindow(this->handle);
	}
}

bool RenderWindow::Initialize(WindowContainer *pWindowContainer, HINSTANCE hInstance, std::wstring window_title, std::wstring window_class, int width, int height)
{
	this->hInstance = hInstance;
	this->width = width;
	this->height = height;
	this->window_title = window_title;
	//this->window_title_wide = StringHelper::StringToWide(this->window_title);
	this->window_class = window_class;
	//this->window_class_wide = StringHelper::StringToWide(this->window_class);

	this->RegisterWindowClass();

	UINT centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - this->width / 2;
	UINT centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - this->height / 2;
	RECT wr{};//視窗起始點定義
	wr.left = centerScreenX;//PosX
	wr.top = centerScreenY;//PosY
	wr.right = wr.left + this->width;//XSize
	wr.bottom = wr.top + this->height;//YSize
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	this->handle = CreateWindowEx(0,//擴展Windows樣式, 0=使用預設
		this->window_class.c_str(),//視窗類別
		this->window_title.c_str(),//視窗標題
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, //窗口樣式
		wr.left, wr.top, //PosX, PosY
		wr.right - wr.left, wr.bottom - wr.top, //XSize, YSize
		NULL, NULL, //父窗口,選單或子窗口
		this->hInstance, pWindowContainer);//欲使用的窗口模塊，窗口附加參數

	if (this->handle == NULL)
	{
		ErrorLogger::Log(GetLastError(), L"CreateWindowEx() 失敗: " + this->window_title);
		return false;
	}

	//窗口至頂，設為主要窗口
	ShowWindow(this->handle, SW_SHOW);
	SetForegroundWindow(this->handle);
	SetFocus(this->handle);

	return true;
}


void RenderWindow::RegisterWindowClass()
{
	WNDCLASSEX wc{};//創建窗口需要賦予類別
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;//窗口風格配置，使用OR連接多個屬性
	wc.lpfnWndProc = HandleMessageSetup;//窗口處理函數的指針
	wc.cbClsExtra = 0;//指定緊跟在窗口類結構後的附加字節數
	wc.cbWndExtra = 0;//指定緊跟在窗口實例的附加字節數，若一個應用程序在資源鐘用CLASS偽指令置物側一個對話框類別，則需要將數值設為DLGWINDOWEXTRA
	wc.hInstance = this->hInstance;//窗口的實例句柄
	wc.hIcon = NULL;//圖標的句柄
	wc.hIconSm = NULL;//窗口關聯之小圖示，NULL=自動轉換hIcon圖標至合適大小
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);//滑鼠指標
	wc.hbrBackground = NULL;//窗口背景樣式句柄
	wc.lpszMenuName = NULL;//指向窗口選單的指針
	wc.lpszClassName = this->window_class.c_str();//指向類別名稱的指針
	wc.cbSize = sizeof(WNDCLASSEX);//WNDCLASSEX的大小，使用sizeof(WNDCLASSEX)獲得精確數值
	RegisterClassEx(&wc);
}

bool RenderWindow::ProcessMessages()
{
	//定義並初始化消息接收結構
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));


	//PeekMessage解說:https://baike.baidu.hk/item/PeekMessage/6373956
	//PeekMessage解說:https://docs.microsoft.com/zh-tw/windows/win32/api/winuser/nf-winuser-peekmessagea
	while (PeekMessage(&msg,//消息儲存位置
		this->handle,//檢查消息的窗口 
		0, //消息過濾起始區段
		0, //消息過濾終止區段
		PM_REMOVE))//消息在PeekMessage處理完後從隊列刪除
	{
		TranslateMessage(&msg);//轉換消息為虛擬碼
		DispatchMessage(&msg);//發送消息至窗口
	}

	//檢查窗口是否關閉
	if (msg.message == WM_NULL)
	{
		if (!IsWindow(this->handle))
		{
			this->handle = NULL;
			UnregisterClass(this->window_class.c_str(), this->hInstance);
			return false;
		}
	}

	return true;
}

HWND RenderWindow::GetHWND() const
{
	return this->handle;
}

LRESULT CALLBACK HandleMessageSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		WindowContainer* pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
		if (pWindow == nullptr)
		{
			ErrorLogger::Log(L"嚴重錯誤:在 WM_NCCREATE 期間指向窗口容器的指針為空");
			exit(-1);
		}

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
		return pWindow->WindowProc(hWnd, uMsg, wParam, lParam);
		//return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

}

LRESULT CALLBACK HandleMsgRedirect(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		//其他訊息
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
	default:
	{
		//回復指針至窗口類別
		WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		//轉送消息至窗口句柄
		return pWindow->WindowProc(hWnd, uMsg, wParam, lParam);
	}
	}
}