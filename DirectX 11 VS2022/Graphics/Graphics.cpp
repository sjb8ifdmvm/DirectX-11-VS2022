#include "Graphics.h"

bool Graphics::Initialize(HWND hWnd, int width, int height)
{
	if (!InitializeDirectX(hWnd, width, height))
		return false;

	if (!InitializeShaders())
		return false;

	if (!InitializeScene())
		return false;

	return true;
}

void Graphics::RenderFrame()
{
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.2f, 0);

	this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->resterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(this->pixelshader.GetShader(), NULL, 0);
	
	UINT stride = sizeof(Vertex);//�w�İϤj�p
	UINT offset = 0;//������
		//���T����
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer2.GetAddressOf(), &stride, &offset);
	this->deviceContext->Draw(3, 0);
		//����T����
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), &stride, &offset);
	this->deviceContext->Draw(3, 0);





	this->swapchain->Present(1, NULL);// 1=V-SYNC ON, 0=V-SYNC OFF
}

bool Graphics::InitializeDirectX(HWND hWnd, int width, int height)
{
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

	if (adapters.size() < 1)
	{
		ErrorLogger::Log(L"�䤣��IDXGI�A�t��\nNo IDXGI Adapters found");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.RefreshRate.Numerator = 60;//��s�v���l
	scd.BufferDesc.RefreshRate.Denominator = 1;//��s�v����
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//Ū���榡
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//�u�����y����
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;//���w�Y��H�ŦX�ѪR�����
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	
	D3D_FEATURE_LEVEL FeatureLevel[] = { D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0,
								D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
								D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
								D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1};
	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
									adapters[0].pAdapter,//IDXGI Adapter
									D3D_DRIVER_TYPE_UNKNOWN,
									NULL,//FOR SOFTWARE DRIVER TYPE
									NULL,//�Ыخɪ��B�~�ݩ�
									FeatureLevel	,//�\�൥�Ű}�C
									_countof(FeatureLevel),//�䴩���\�൥�Ť���������
									D3D11_SDK_VERSION,//SDK����
									&scd, //SwapChain�y�z
									this->swapchain.GetAddressOf(),//SwapChain���w
									this->device.GetAddressOf(),	//Device���w
									NULL,//�䴩���\�൥��
									this->deviceContext.GetAddressOf()//Device�V���w
									);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"�˸m�P�洫��Ыإ���\nFailed to create device and swapchain.");
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"�w�İ��������\nGetBuffer Failed.");
		return false;
	}

	hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"��V���ϳЫإ���\nFailed to create render target view.");
		return false;
	}

	//�`�׼ҪO�w�İϴy�z(Depth/Stencil Buffer)
	D3D11_TEXTURE2D_DESC depthStencilDesc {};
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"�`�׼ҪO�Ыإ���\nFailed to create depth stencil buffer");
		return false;
	}

	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"�`�׼ҪO�Ыإ���\nFailed to create depth stencil view.");
		return false;
	}

	//Output Merger �X�ֿ�X
	this->deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

	//Create depth stencil �إ߲`�׼ҪO���A
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthstencildesc.DepthEnable = true;//�ҥβ`�״���
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;//�}�Ҳ`�׼ҪO�g�J�ܽw�İ�
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;//�M�w�`�״������
	hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"�`�׼ҪO���A�Ыإ���\nFailed to create depth stencil state.");
		return false;
	}

	//���]���]�m
	//�Ы�VIEWPORT
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	this->deviceContext->RSSetViewports(1, &viewport);//�]�mViewport
	//���]�����A
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;//�M�wø�s�����έI��
	rasterizerDesc.FrontCounterClockwise = FALSE;//��ܩ�-�i���]�w�ATRUE=�f�ɰwø�s, (�w�])FALSE=���ɰwø�s
	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->resterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"���]�����A�إߥ���\nFailed to create rasterizer state.");
		return false;
	}
	//���]���]�m����
	return true;
}

bool Graphics::InitializeShaders()
{

	//InputLayout�w�q���I�p��ǻ��쳻�I�ۦ⾹
	//���c�ѻ�:https://blog.csdn.net/u010333737/article/details/78682213#%E5%89%8D%E8%A8%80
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numElements = ARRAYSIZE(layout);

	WCHAR exePath[MAX_PATH + 1];
	ZeroMemory(&exePath, sizeof(exePath));
	GetModuleFileName(NULL, exePath, MAX_PATH);
	
	if (!vertexshader.Initialize(this->device, (std::wstring)exePath + L"\\..\\vertexshader.cso", layout, numElements))
		return false;

	if (!pixelshader.Initialize(this->device, (std::wstring)exePath + L"\\..\\pixelshader.cso"))
		return false;

	return true;
}

bool Graphics::InitializeScene()
{
	//�Ĥ@�ӤT����
	Vertex v[] =
	{
		Vertex(-3.0f, -3.0f, 1.0f, 1.0f, 0.0f, 0.0f),//�ĤT�H��
		Vertex( 0.0f,  3.0f, 1.0f, 1.0f, 0.0f, 0.0f),//Y�b
		Vertex( 3.0f, -3.0f, 1.0f, 1.0f, 0.0f, 0.0f),//�ĥ|�H��
		//Vertex(0.1f,0.0f),
		//Vertex(0.0f,0.1f),
	};
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(v);
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;

	HRESULT hr = this->device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->vertexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"���I�w�İϳЫإ���\nFailed To create vertex buffer.");
		return false;
	}

	//�ĤG�ӤT����
	Vertex v2[] =
	{
		Vertex(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f),//�ĤT�H��
		Vertex( 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f),//Y�b
		Vertex( 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f),//�ĥ|�H��
		//Vertex(0.1f,0.0f),
		//Vertex(0.0f,0.1f),
	};

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(v2);
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v2;

	hr = this->device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->vertexBuffer2.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"���I�w�İϳЫإ���\nFailed To create vertex buffer.");
		return false;
	}

	return true;
}
