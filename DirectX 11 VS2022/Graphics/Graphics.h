#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include "Vertex.h"

class Graphics
{
public:
	bool Initialize(HWND hWnd, int width, int height);
	void RenderFrame();
private:
	bool InitializeDirectX(HWND hWnd, int width, int height);
	bool InitializeShaders();
	bool InitializeScene();
	
	//渲染管道基本要件開始
	Microsoft::WRL::ComPtr<ID3D11Device> device;//硬體裝置指針
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;//Frame 幀指針
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;//目標顯示指針
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;//Frame 交換鏈指針

	VertexShader vertexshader;//頂點著色器
	PixelShader pixelshader;//像素著色器
	//渲染管道基本要件結束

	//圖像
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer2;

	//深度
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> resterizerState;
};