#include "Shaders.h"

bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
{
	HRESULT hr;


	hr = D3DReadFileToBlob(shaderpath.c_str(), shader_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"著色器讀取失敗:" + shaderpath + L"\nFailed to load shader:" + shaderpath);
		return false;
	}

	hr = device->CreateVertexShader(this->shader_buffer.Get()->GetBufferPointer(), this->shader_buffer.Get()->GetBufferSize(), NULL, this->shader.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"頂點著色器創建失敗:" + shaderpath + L"\nFailed to create vertex shader:" + shaderpath);
		return false;
	}

	hr = device->CreateInputLayout(layoutDesc, numElements, this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), this->inputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"輸入布局創建失敗\nFailed to create inputlayout.");
		return false;
	}
	return true;
}

ID3D11InputLayout* VertexShader::GetInputLayout()
{
	return this->inputLayout.Get();
}

ID3D11VertexShader* VertexShader::GetShader()
{
	return this->shader.Get();
}

ID3DBlob* VertexShader::GetBuffer()
{
	return this->shader_buffer.Get();
}

bool PixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath)
{
	HRESULT hr;
	hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"著色器讀取失敗:" + shaderpath + L"\nFailed to load shader:" + shaderpath);
		return false;
	}

	hr = device->CreatePixelShader(this->shader_buffer.Get()->GetBufferPointer(), this->shader_buffer.Get()->GetBufferSize(), NULL, this->pixelshader.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"像素著色器創建失敗:" + shaderpath + L"\nFail to create pixel shader:" + shaderpath);
		return false;
	}

	return true;
}

ID3D11PixelShader* PixelShader::GetShader()
{
	return this->pixelshader.Get();
}

ID3DBlob* PixelShader::GetBuffer()
{
	return this->shader_buffer.Get();
}
