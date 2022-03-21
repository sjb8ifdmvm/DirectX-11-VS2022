#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include "ConstantBuffer.h"

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
	VertexBuffer<Vertex>vertexBuffer;
	IndexBuffer indexBuffer;
	ConstantBuffer<CB_VS_vertexshader> constantBuffer;

	//深度
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	//光柵器
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> resterizerState;

	//字串顯示輸出
	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	//紋理
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;//採樣器
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myTexture;//儲存紋理
};