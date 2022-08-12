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

	//��V�޹D�򥻭n��}�l
	Microsoft::WRL::ComPtr<ID3D11Device> device;//�w��˸m���w
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;//Frame �V���w
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;//�ؼ���ܫ��w
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;//Frame �洫����w

	VertexShader vertexshader;//���I�ۦ⾹
	PixelShader pixelshader;//�����ۦ⾹
	//��V�޹D�򥻭n�󵲧�

	//�Ϲ�
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_pixelshader> cb_ps_pixelshader;

	//�`��
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	//���]��
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_CullFront;

	//�V�X��
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

	//�r����ܿ�X
	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	//���z
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;//�ļ˾�
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;//�x�s���z
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;//�x�s���z

	int WindowWidth = 0;
	int WindowHeight = 0;
	Timer fpsTimer;
};