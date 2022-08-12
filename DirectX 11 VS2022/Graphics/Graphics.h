#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include "Camera.h"
#include "..\\Timer.h"
#include "GameObject.h"

//------- ImGui
#include "ImGui\\imgui.h"
#include "ImGui\\imgui_impl_win32.h"
#include "ImGui\\imgui_impl_dx11.h"


class Graphics
{
public:
	bool Initialize(HWND hWnd, int width, int height);
	void RenderFrame();
	Camera camera;
	GameObject gameObject;

private:
	bool InitializeDirectX(HWND hWnd);
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
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_pixelshader> cb_ps_pixelshader;

	//深度
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	//光柵器
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_CullFront;

	//混合器
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

	//字串顯示輸出
	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	//紋理
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;//採樣器
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;//儲存紋理
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;//儲存紋理

	int WindowWidth = 0;
	int WindowHeight = 0;
	Timer fpsTimer;
};