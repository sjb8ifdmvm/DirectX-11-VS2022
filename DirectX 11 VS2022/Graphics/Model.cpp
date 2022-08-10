#include "Model.h"

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->texture = texture;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;

	try {
		//�����
		Vertex v[] =
		{
			Vertex(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f),	//�e���U[0]
			Vertex(-0.5f,  0.5f, -0.5f, 0.0f, 0.0f),	//�e���W[1]
			Vertex(0.5f,  0.5f, -0.5f, 1.0f, 0.0f),	//�e�k�W[2]
			Vertex(0.5f, -0.5f, -0.5f, 1.0f, 1.0f),	//�e�k�U[3]
			Vertex(-0.5f, -0.5f,  0.5f, 0.0f, 1.0f),	//�ᥪ�U[4]
			Vertex(-0.5f,  0.5f,  0.5f, 0.0f, 0.0f),	//�ᥪ�W[5]
			Vertex(0.5f,  0.5f,  0.5f, 1.0f, 0.0f),	//��k�W[6]
			Vertex(0.5f, -0.5f,  0.5f, 1.0f, 1.0f),	//��k�U[7]
		};

		//��l�Ƴ��I�ۦ⾹
		HRESULT hr = this->vertexBuffer.Initialize(this->device, v, ARRAYSIZE(v));
		COM_ERROR_IF_FAILED(hr, L"���I�w�İϳЫإ���\nFailed To create vertex buffer.");


		DWORD indices[] =
		{
			0, 1, 2,//�����W
			0, 2, 3,//���k�U
			4, 7, 6,//�I���U
			4, 6, 5,//�I�k�W
			3, 2, 6,//�k���W
			3, 6, 7,//�k�k�U
			4, 1, 0,//���k�U
			4, 5, 1,//�����W
			5, 6, 1,//�W���W
			6, 2, 1,//�W�k�U
			0, 3, 7,//�U�k�U
			0, 7, 4,//�U���W
		};

		//��l�Ƴ��I�ۦ⾹����
		hr = this->indexBuffer.Initialize(this->device, indices, ARRAYSIZE(indices));
		COM_ERROR_IF_FAILED(hr, L"�Ы�Indices�w�İϥ���\nFailed to create indices buffer.");
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