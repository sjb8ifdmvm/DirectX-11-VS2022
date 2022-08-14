#include "Mesh.h"

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture> textures)
{
	this->deviceContext = deviceContext;
	this->textures = textures;

	HRESULT hr = this->vertexBuffer.Initialize(device, vertices.data(), (UINT)indices.size());
	COM_ERROR_IF_FAILED(hr, L"���I�w�İϺ����l�ƥ���\nFailed to initialize vertex buffer for mesh.");

	hr = this->indexBuffer.Initialize(device, indices.data(), (UINT)indices.size());
	COM_ERROR_IF_FAILED(hr, L"���޽w�İϺ����l�ƥ���\nFailed to initialize index buffer for mesh.");

}

Mesh::Mesh(const Mesh& mesh)
{
	this->deviceContext = mesh.deviceContext;
	this->indexBuffer = mesh.indexBuffer;
	this->vertexBuffer = mesh.vertexBuffer;
	this->textures = mesh.textures;
}

void Mesh::Draw()
{
	UINT offset = 0;

	for (int i = 0; i < textures.size(); i++)
	{
		if (textures[i].GetType() == aiTextureType::aiTextureType_DIFFUSE)
		{
			this->deviceContext->PSSetShaderResources(0, 1, textures[i].GetTextureResourceViewAddress());
			break;
		}
	}

	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(this->indexBuffer.IndexCount(), 0, 0);
}