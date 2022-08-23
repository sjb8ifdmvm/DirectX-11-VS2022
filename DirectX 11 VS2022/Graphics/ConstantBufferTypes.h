#pragma once
#include <DirectXMath.h>

//頂點著色器常量緩衝區
struct CB_VS_vertexshader
{
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};

struct CB_PS_light
{
	DirectX::XMFLOAT3 ambientLightColor;
	float ambientLightStrength;
};