#include "Graphics.h"

bool Graphics::Initialize(HWND hWnd, int width, int height)
{
	this->WindowWidth = width;
	this->WindowHeight = height;
	this->fpsTimer.Start();

	if (!InitializeDirectX(hWnd))
		return false;

	if (!InitializeShaders())
		return false;

	if (!InitializeScene())
		return false;

	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
	ImGui::StyleColorsClassic();

	return true;
}

void Graphics::RenderFrame()
{
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);//參數1 NULL 為使用預設, this->blendState.Get() 使用混合器
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(this->pixelshader.GetShader(), NULL, 0);

	UINT offset = 0;//偏移值
	
	{   //草地
		this->model.Draw(camera.GetViewMatrix() * camera.GetProjectionMatrix());
	}

	//字串輸出
	static int fpsCounter = 0;
	static std::wstring fpsString = L"FPS: 0";
	fpsCounter++;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0f)
	{
		fpsString = L"FPS: " + std::to_wstring(fpsCounter);
		fpsCounter = 0;
		fpsTimer.Restart();
	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), fpsString.c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	//spriteFont->DrawString(spriteBatch.get(), L"HELLO World\n你好，世界", DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();


	//------------------ ImGui Implement --------------------
	//執行Dear ImGui 框架
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//建立 ImGui 測試視窗
	ImGui::Begin("ImGui Window");
	//ImGui::DragFloat3("Translation X/Y/Z", translationOffset, 0.01f, -5.0f, 5.0f);
	//ImGui::DragFloat("Alpha", &alpha, 0.02f, 0.0f, 1.0f);
	ImGui::End();
	//Assemble Together Draw Data
	ImGui::Render();
	//Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//------------------ ImGui Implement --------------------

	this->swapchain->Present(1, NULL);// 1=V-SYNC ON, 0=V-SYNC OFF
}

bool Graphics::InitializeDirectX(HWND hWnd)
{
	try {
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		if (adapters.size() < 1)
		{
			ErrorLogger::Log(L"找不到IDXGI適配器\nNo IDXGI Adapters found");
			return false;
		}

		DXGI_SWAP_CHAIN_DESC scd = { 0 };

		scd.BufferDesc.Width = this->WindowWidth;
		scd.BufferDesc.Height = this->WindowHeight;
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
									D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1 };
		HRESULT hr;
		hr = D3D11CreateDeviceAndSwapChain(
			adapters[0].pAdapter,//IDXGI Adapter
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL,//FOR SOFTWARE DRIVER TYPE
			NULL,//創建時的額外屬性
			FeatureLevel,//功能等級陣列
			_countof(FeatureLevel),//支援的功能等級內的元素數
			D3D11_SDK_VERSION,//SDK版本
			&scd, //SwapChain描述
			this->swapchain.GetAddressOf(),//SwapChain指針
			this->device.GetAddressOf(),	//Device指針
			NULL,//支援的功能等級
			this->deviceContext.GetAddressOf()//Device幀指針
		);
		COM_ERROR_IF_FAILED(hr, L"裝置與交換鏈創建失敗\nFailed to create device and swapchain.");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, L"緩衝區獲取失敗\nGetBuffer Failed.");

		hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"渲染視圖創建失敗\nFailed to create render target view.");

		//深度模板緩衝區描述(Depth/Stencil Buffer)
		//D3D11_TEXTURE2D_DESC depthStencilDesc{};
		CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->WindowWidth, this->WindowHeight);
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"深度模板創建失敗\nFailed to create depth stencil buffer");

		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"深度模板創建失敗\nFailed to create depth stencil view.");

		//Output Merger 合併輸出
		this->deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

		//Create depth stencil 建立深度模板狀態
		//D3D11_DEPTH_STENCIL_DESC depthstencildesc;
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;//決定深度替換顯示
		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"深度模板狀態創建失敗\nFailed to create depth stencil state.");

		//光柵器設置
		//創建 與 設置 VIEWPORT
		//D3D11_VIEWPORT viewport;
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->WindowWidth), static_cast<float>(this->WindowHeight));
		this->deviceContext->RSSetViewports(1, &viewport);//設置Viewport

		//光柵器狀態
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);		
		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"光柵器狀態建立失敗\nFailed to create rasterizer state.");

		//光柵器狀態 繪製正面
		CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront(D3D11_DEFAULT);
		rasterizerDesc_CullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;//決定繪製正面或背面
		hr = this->device->CreateRasterizerState(&rasterizerDesc_CullFront, this->rasterizerState_CullFront.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"光柵器狀態建立失敗\nFailed to create rasterizer state.");

		//光柵器設置完成

		//建立混合狀態	
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));

		D3D11_RENDER_TARGET_BLEND_DESC rtbd;
		ZeroMemory(&rtbd, sizeof(rtbd));

		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.RenderTarget[0] = rtbd;

		hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"混合狀態建立失敗\nFailed to create blend state.");
		//混合狀態設置完成

		//字串輸出
		spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
		spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\MS_Sans_Serif_16");

		//創建採樣器描述並附加至採樣器狀態
		//D3D11_SAMPLER_DESC sampDesc;
		CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;//X座標
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;//Y座標
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;//Z座標(暫不使用)
		hr = this->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"採樣器創建失敗\nFailed to  create sampler state.");
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
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

	WCHAR exeP[256];
	GetCurrentDirectory(256, exeP);

	std::wstring str2;
	str2 = exeP;
	str2 += L"\\Data\\Shader\\vertexshader.cso";

	if (!vertexshader.Initialize(this->device, str2.c_str(), layout, numElements))
		return false;

	str2 = exeP;
	str2 += L"\\Data\\Shader\\pixelshader.cso";

	if (!pixelshader.Initialize(this->device, str2.c_str()))
		return false;

	return true;
}

bool Graphics::InitializeScene()
{
	try {

		WCHAR exe[256];
		GetCurrentDirectory(256, exe);

		std::wstring str1;
		str1 = exe;
		str1 += L"\\Data\\Textures\\seamless_pavement.jpg";
		//紋理可以儲存在 ID3DResource 或 ID3DShaderResourceView
		//加載紋理
		HRESULT hr = DirectX::CreateWICTextureFromFile(this->device.Get(), str1.c_str(), nullptr, this->grassTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"從檔案提取紋理失敗\nFailed to create wic texture from file.");

		str1 = exe;
		str1 += L"\\Data\\Textures\\pinksquare.jpg";
		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), str1.c_str(), nullptr, this->pinkTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"從檔案提取紋理失敗\nFailed to create wic texture from file.");

		//初始化常量緩衝區
		hr = this->cb_vs_vertexshader.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, L"頂點著色器緩衝區初始化失敗\nFailed to initialize vertexshader buffer.");

		hr = this->cb_ps_pixelshader.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, L"像素著色器緩衝區初始化失敗\nFailed to initialize pixelshader buffer.");

		//初始化模組(s)
		if (!model.Initialize(this->device.Get(), this->deviceContext.Get(), this->grassTexture.Get(), cb_vs_vertexshader))
			return false;

		camera.SetPosition(0.0f, 0.0f, -2.0f);
		camera.SetProjectionValues(90.0f, static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight), 0.1f, 1000.0f);
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}