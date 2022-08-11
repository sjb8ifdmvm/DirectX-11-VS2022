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
	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
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
	//�p��۾�����x�}
	this->worldMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
	this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
}

const XMVECTOR& Model::GetPositionVector() const
{
	return this->posVector;
}

const XMFLOAT3& Model::GetPositionFloat3() const
{
	return this->pos;
}

const XMVECTOR& Model::GetRotationVector() const
{
	return this->rotVector;
}

const XMFLOAT3& Model::GetRotationFloat3() const
{
	return this->rot;
}

void Model::SetPosition(const XMVECTOR& pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->UpdateWorldMatrix();
}

void Model::SetPosition(const XMFLOAT3& pos)
{
	this->pos = pos;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateWorldMatrix();
}

void Model::SetPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateWorldMatrix();
}

void Model::SetRotation(const XMVECTOR& rot)
{
	this->rotVector = rot;
	XMStoreFloat3(&this->rot, rot);
	this->UpdateWorldMatrix();
}

void Model::SetRotation(const XMFLOAT3& pos)
{
	this->rot = rot;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void Model::SetRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void Model::AdjustPosition(const XMVECTOR& pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateWorldMatrix();
}

void Model::AdjustPosition(const XMFLOAT3& pos)
{
	this->pos.x += pos.x;
	this->pos.y += pos.y;
	this->pos.z += pos.z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateWorldMatrix();
}

void Model::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateWorldMatrix();
}

void Model::AdjustRotation(const XMVECTOR& rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateWorldMatrix();
}

void Model::AdjustRotation(const XMFLOAT3& rot)
{
	this->rot.x += rot.x;
	this->rot.y += rot.y;
	this->rot.z += rot.z;
	this->UpdateWorldMatrix();
}

void Model::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void Model::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
		return;

	lookAtPos.x = this->pos.x - lookAtPos.x;
	lookAtPos.y = this->pos.y - lookAtPos.y;
	lookAtPos.z = this->pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = (float)sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = (float)atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = (float)atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}

const XMVECTOR& Model::GetForwardVector()
{
	return this->vec_forward;
}

const XMVECTOR& Model::GetRightVector()
{
	return this->vec_right;
}

const XMVECTOR& Model::GetBackwardVector()
{
	return this->vec_backward;
}

const XMVECTOR& Model::GetLeftVector()
{
	return this->vec_left;
}