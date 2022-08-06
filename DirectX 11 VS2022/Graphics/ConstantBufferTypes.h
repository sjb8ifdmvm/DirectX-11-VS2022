#pragma once
#include <DirectXMath.h>

//頂點著色器常量緩衝區
struct CB_VS_vertexshader
{
	DirectX::XMMATRIX mat;
};

struct CB_PS_pixelshader
{
	float alpha = 1.0f;
};