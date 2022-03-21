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
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(this->pixelshader.GetShader(), NULL, 0);
	


	UINT stride = sizeof(Vertex);//緩衝區大小
	UINT offset = 0;//偏移值

	//正方形
	this->deviceContext->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf());
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), &stride, &offset);
	this->deviceContext->Draw(6, 0);

	//字串輸出
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), L"HELLO World\n你好，世界", DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();


	this->swapchain->Present(1, NULL);// 1=V-SYNC ON, 0=V-SYNC OFF
}

bool Graphics::InitializeDirectX(HWND hWnd, int width, int height)
{
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

	if (adapters.size() < 1)
	{
		ErrorLogger::Log(L"找不到IDXGI適配器\nNo IDXGI Adapters found");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.RefreshRate.Numerator = 60;//刷新率分子
	scd.BufferDesc.RefreshRate.Denominator = 1;//刷新率分母
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//讀取格式
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//線條掃描順序
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;//指定縮放以符合解析度顯示
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
									NULL,//創建時的額外屬性
									FeatureLevel	,//功能等級陣列
									_countof(FeatureLevel),//支援的功能等級內的元素數
									D3D11_SDK_VERSION,//SDK版本
									&scd, //SwapChain描述
									this->swapchain.GetAddressOf(),//SwapChain指針
									this->device.GetAddressOf(),	//Device指針
									NULL,//支援的功能等級
									this->deviceContext.GetAddressOf()//Device幀指針
									);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"裝置與交換鏈創建失敗\nFailed to create device and swapchain.");
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"緩衝區獲取失敗\nGetBuffer Failed.");
		return false;
	}

	hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"渲染視圖創建失敗\nFailed to create render target view.");
		return false;
	}

	//深度模板緩衝區描述(Depth/Stencil Buffer)
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
		ErrorLogger::Log(hr, L"深度模板創建失敗\nFailed to create depth stencil buffer");
		return false;
	}

	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"深度模板創建失敗\nFailed to create depth stencil view.");
		return false;
	}

	//Output Merger 合併輸出
	this->deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

	//Create depth stencil 建立深度模板狀態
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthstencildesc.DepthEnable = true;//啟用深度測試
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;//開啟深度模板寫入至緩衝區
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;//決定深度替換顯示
	hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"深度模板狀態創建失敗\nFailed to create depth stencil state.");
		return false;
	}

	//光柵器設置
	//創建VIEWPORT
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	this->deviceContext->RSSetViewports(1, &viewport);//設置Viewport
	//光柵器狀態
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;//決定繪製正面或背面
	rasterizerDesc.FrontCounterClockwise = FALSE;//選擇性-可不設定，TRUE=逆時針繪製, (預設)FALSE=順時針繪製
	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->resterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"光柵器狀態建立失敗\nFailed to create rasterizer state.");
		return false;
	}
	//光柵器設置完成

	//字串輸出
	spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
	spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"..\\Data\\Fonts\\MS_Sans_Serif_16");

	//創建採樣器描述並附加至採樣器狀態
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;//X座標
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;//Y座標
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;//Z座標(暫不使用)
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = this->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"採樣器創建失敗\nFailed to  create sampler state.");
		return false;
	}

	return true;
}

bool Graphics::InitializeShaders()
{

	//InputLayout定義頂點如何傳遞到頂點著色器
	//結構解說:https://blog.csdn.net/u010333737/article/details/78682213#%E5%89%8D%E8%A8%80
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
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
	//第一個三角形
	Vertex v[] =
	{
		
		Vertex(-8.0f,  8.0f, 1.0f, 0.0f, 0.0f),//左上
		Vertex( 8.0f,  8.0f, 1.0f, 1.0f, 0.0f),//右上
		Vertex(-8.0f, -8.0f, 1.0f, 0.0f, 1.0f),//左下

		
		Vertex( 8.0f,  8.0f, 1.0f, 1.0f, 0.0f),//右上
		Vertex( 8.0f, -8.0f, 1.0f, 1.0f, 1.0f),//右下
		Vertex(-8.0f, -8.0f, 1.0f, 0.0f, 1.0f),//左下
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
		ErrorLogger::Log(hr, L"頂點緩衝區創建失敗\nFailed To create vertex buffer.");
		return false;
	}

	//紋理可以儲存在 ID3DResource 或 ID3DShaderResourceView
	hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"..\\Data\\Textures\\MakeDirectX.png", nullptr, this->myTexture.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"從檔案提取紋理失敗\nFailed to create wic texture from file.");
		return false;
	}

	return true;
}
