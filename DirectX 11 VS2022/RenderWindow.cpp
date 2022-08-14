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
	RECT wr{};//�����_�l�I�w�q
	wr.left = centerScreenX;//PosX
	wr.top = centerScreenY;//PosY
	wr.right = wr.left + this->width;//XSize
	wr.bottom = wr.top + this->height;//YSize
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	this->handle = CreateWindowEx(0,//�X�iWindows�˦�, 0=�ϥιw�]
		this->window_class.c_str(),//�������O
		this->window_title.c_str(),//�������D
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, //���f�˦�
		wr.left, wr.top, //PosX, PosY
		wr.right - wr.left, wr.bottom - wr.top, //XSize, YSize
		NULL, NULL, //�����f,���Τl���f
		this->hInstance, pWindowContainer);//���ϥΪ����f�Ҷ��A���f���[�Ѽ�

	if (this->handle == NULL)
	{
		ErrorLogger::Log(GetLastError(), L"CreateWindowEx() ����: " + this->window_title);
		return false;
	}

	//���f�ܳ��A�]���D�n���f
	ShowWindow(this->handle, SW_SHOW);
	SetForegroundWindow(this->handle);
	SetFocus(this->handle);

	return true;
}


void RenderWindow::RegisterWindowClass()
{
	WNDCLASSEX wc{};//�Ыص��f�ݭn�ᤩ���O
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;//���f����t�m�A�ϥ�OR�s���h���ݩ�
	wc.lpfnWndProc = HandleMessageSetup;//���f�B�z��ƪ����w
	wc.cbClsExtra = 0;//���w���b���f�����c�᪺���[�r�`��
	wc.cbWndExtra = 0;//���w���b���f��Ҫ����[�r�`�ơA�Y�@�����ε{�Ǧb�귽����CLASS�����O�m�����@�ӹ�ܮ����O�A�h�ݭn�N�ƭȳ]��DLGWINDOWEXTRA
	wc.hInstance = this->hInstance;//���f����ҥy�`
	wc.hIcon = NULL;//�ϼЪ��y�`
	wc.hIconSm = NULL;//���f���p���p�ϥܡANULL=�۰��ഫhIcon�ϼЦܦX�A�j�p
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);//�ƹ�����
	wc.hbrBackground = NULL;//���f�I���˦��y�`
	wc.lpszMenuName = NULL;//���V���f��檺���w
	wc.lpszClassName = this->window_class.c_str();//���V���O�W�٪����w
	wc.cbSize = sizeof(WNDCLASSEX);//WNDCLASSEX���j�p�A�ϥ�sizeof(WNDCLASSEX)��o��T�ƭ�
	RegisterClassEx(&wc);
}

bool RenderWindow::ProcessMessages()
{
	//�w�q�ê�l�Ʈ����������c
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));


	//PeekMessage�ѻ�:https://baike.baidu.hk/item/PeekMessage/6373956
	//PeekMessage�ѻ�:https://docs.microsoft.com/zh-tw/windows/win32/api/winuser/nf-winuser-peekmessagea
	while (PeekMessage(&msg,//�����x�s��m
		this->handle,//�ˬd���������f 
		0, //�����L�o�_�l�Ϭq
		0, //�����L�o�פ�Ϭq
		PM_REMOVE))//�����bPeekMessage�B�z����q���C�R��
	{
		TranslateMessage(&msg);//�ഫ�����������X
		DispatchMessage(&msg);//�o�e�����ܵ��f
	}

	//�ˬd���f�O�_����
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
			ErrorLogger::Log(L"�Y�����~:�b WM_NCCREATE �������V���f�e�������w����");
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
		//��L�T��
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
	default:
	{
		//�^�_���w�ܵ��f���O
		WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		//��e�����ܵ��f�y�`
		return pWindow->WindowProc(hWnd, uMsg, wParam, lParam);
	}
	}
}