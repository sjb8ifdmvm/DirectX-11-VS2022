#pragma once
#include "..\\ErrorLogger.h"
#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11.h>
#include <wrl\client.h>
#include <d3dcompiler.h>

class VertexShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements);
	ID3D11InputLayout* GetInputLayout();
	ID3D11VertexShader* GetShader();
	ID3DBlob* GetBuffer();
private:

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout = nullptr;//¥¬§½«ü°w
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> shader_buffer = nullptr;
};

class PixelShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath);
	ID3D11PixelShader* GetShader();
	ID3DBlob* GetBuffer();

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelshader = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> shader_buffer = nullptr;
};
