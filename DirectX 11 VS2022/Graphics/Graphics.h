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
	
	//��V�޹D�򥻭n��}�l
	Microsoft::WRL::ComPtr<ID3D11Device> device;//�w��˸m���w
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;//Frame �V���w
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;//�ؼ���ܫ��w
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;//Frame �洫����w

	VertexShader vertexshader;//���I�ۦ⾹
	PixelShader pixelshader;//�����ۦ⾹
	//��V�޹D�򥻭n�󵲧�

	//�Ϲ�
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer2;

	//�`��
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> resterizerState;
};