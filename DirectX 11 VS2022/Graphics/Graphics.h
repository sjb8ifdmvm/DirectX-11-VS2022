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

	//��V�޹D�򥻭n��}�l
	Microsoft::WRL::ComPtr<ID3D11Device> device;//�w��˸m���w
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;//Frame �V���w
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;//�ؼ���ܫ��w
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;//Frame �洫����w

	VertexShader vertexshader;//���I�ۦ⾹
	PixelShader pixelshader;//�����ۦ⾹
	//��V�޹D�򥻭n�󵲧�

	//�Ϲ�
	VertexBuffer<Vertex>vertexBuffer;
	IndexBuffer indexBuffer;
	ConstantBuffer<CB_VS_vertexshader> constantBuffer;

	//�`��
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	//���]��
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> resterizerState;

	//�r����ܿ�X
	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	//���z
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;//�ļ˾�
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myTexture;//�x�s���z
};