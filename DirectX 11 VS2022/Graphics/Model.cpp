#include "Model.h"

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->texture = texture;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;

	try {
		//正方形
		Vertex v[] =
		{
			Vertex(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f),	//前左下[0]
			Vertex(-0.5f,  0.5f, -0.5f, 0.0f, 0.0f),	//前左上[1]
			Vertex(0.5f,  0.5f, -0.5f, 1.0f, 0.0f),	//前右上[2]
			Vertex(0.5f, -0.5f, -0.5f, 1.0f, 1.0f),	//前右下[3]
			Vertex(-0.5f, -0.5f,  0.5f, 0.0f, 1.0f),	//後左下[4]
			Vertex(-0.5f,  0.5f,  0.5f, 0.0f, 0.0f),	//後左上[5]
			Vertex(0.5f,  0.5f,  0.5f, 1.0f, 0.0f),	//後右上[6]
			Vertex(0.5f, -0.5f,  0.5f, 1.0f, 1.0f),	//後右下[7]
		};

		//初始化頂點著色器
		HRESULT hr = this->vertexBuffer.Initialize(this->device, v, ARRAYSIZE(v));
		COM_ERROR_IF_FAILED(hr, L"頂點緩衝區創建失敗\nFailed To create vertex buffer.");


		DWORD indices[] =
		{
			0, 1, 2,//正左上
			0, 2, 3,//正右下
			4, 7, 6,//背左下
			4, 6, 5,//背右上
			3, 2, 6,//右左上
			3, 6, 7,//右右下
			4, 1, 0,//左右下
			4, 5, 1,//左左上
			5, 6, 1,//上左上
			6, 2, 1,//上右下
			0, 3, 7,//下右下
			0, 7, 4,//下左上
		};

		//初始化頂點著色器索引
		hr = this->indexBuffer.Initialize(this->device, indices, ARRAYSIZE(indices));
		COM_ERROR_IF_FAILED(hr, L"創建Indices緩衝區失敗\nFailed to create indices buffer.");
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	this->UpdateWorldMatrix();
	return true;
}

void Model::SetTexture(ID3D11ShaderResourceView* texture)
{
	this->texture = texture;
}

void Model::Draw(const XMMATRIX& viewProjectionMatrix)
{
	//Update Constant buffer with WVP Matrix
	this->cb_vs_vertexshader->data.mat = this->worldMatrix * viewProjectionMatrix;//Caculate world Matrix
	this->cb_vs_vertexshader->data.mat = XMMatrixTranspose(this->cb_vs_vertexshader->data.mat);
	this->cb_vs_vertexshader->ApplyChanges();
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());
	this->deviceContext->PSSetShaderResources(0, 1, &this->texture); //Set Texture
	this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
;
	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.StridePtr(), &offset);
	this->deviceContext->DrawIndexed(this->indexBuffer.BufferSize(), 0, 0); //Draw
}

void Model::UpdateWorldMatrix()
{
	this->worldMatrix = XMMatrixIdentity();
}