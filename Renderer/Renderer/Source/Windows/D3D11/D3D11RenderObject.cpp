#include "D3D11RenderObject.h"

#include <string>
#include <sstream>
#include "D3D11RenderResource.h"
#include "D3D11RenderingData.h"
#include "D3D11CommandBuffer.h"
#include "D3D11Renderer.h"
#include "../D3DUtility/D3DUtility.h"
#include "D3D11Texture.h"

using namespace DirectX;
using namespace D3DUtility;

D3D11RenderObject::D3D11RenderObject()
{
}

D3D11RenderObject::~D3D11RenderObject()
{
}

void D3D11RenderObject::Create(RenderingData& renderingData, const char* objectName, std::vector<MeshVertex>& vertices, std::vector<uint16_t>& indices)
{
	/// Create vertex buffer
	std::string vertexResourceName = objectName;
	vertexResourceName += "VertexBuffer";

	std::shared_ptr<RenderResource> vertexResource = renderingData.CreateResource(vertexResourceName.c_str(), ResourceType::VertexBuffer,
		vertices.data(), (unsigned int)vertices.size(), (unsigned int)sizeof(vertices[0]));

	m_verticesResource = dynamic_cast<D3D11RenderResource*>(vertexResource.get());
	assert(m_verticesResource != nullptr);

	/// Create index buffer
	std::string indexResourceName = objectName;
	indexResourceName += "IndexBuffer";

	std::shared_ptr<RenderResource> indexResource = renderingData.CreateResource(indexResourceName.c_str(), ResourceType::IndexBuffer,
		indices.data(), (unsigned int)indices.size(), (unsigned int)sizeof(indices[0]), ResourceFormat::R16_UINT);

	m_indicesResource = dynamic_cast<D3D11RenderResource*>(indexResource.get());
	assert(m_indicesResource != nullptr);

	constexpr unsigned int FRAME_RESOURCE_COUNT = 2;

	/// Create world matrix resource
	for (unsigned int frameResourceCount = 0; frameResourceCount < FRAME_RESOURCE_COUNT; ++frameResourceCount)
	{
		std::stringstream frameResourceIndexName;
		frameResourceIndexName << frameResourceCount;

		std::string worldMatrixResourceName = objectName;
		worldMatrixResourceName += "worldMatrixBuffer";
		worldMatrixResourceName += frameResourceIndexName.str();

		std::shared_ptr<RenderResource> wrldMatrixResource = renderingData.CreateResource(worldMatrixResourceName.c_str(), ResourceType::ConstantBuffer,
			nullptr, 1, (unsigned int)sizeof(XMFLOAT4X4));

		m_worldMatrixResources.emplace_back(dynamic_cast<D3D11RenderResource*>(wrldMatrixResource.get()));
		assert(m_worldMatrixResources[frameResourceCount] != nullptr);
	}

	m_vertices = vertices;
	m_indices = indices;
}

void D3D11RenderObject::DrawIndexed(CommandBuffer& commandBuffer)
{
	D3D11CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D11CommandBuffer*>(&commandBuffer);
	assert(d3dCommandBuffer != nullptr);

	ID3D11DeviceContext* deviceContext = d3dCommandBuffer->GetDeferredContext();

	UINT offset = 0;
	UINT stride = (UINT)m_verticesResource->GetDataStride();

	deviceContext->IASetVertexBuffers(0, 1, m_verticesResource->GetBufferAddress(), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indicesResource->GetBuffer(), GetD3DFormat(m_indicesResource->GetDataFormat()), 0);

	deviceContext->DrawIndexed((UINT)m_indicesResource->GetDataCount(), 0, 0);
}

void D3D11RenderObject::DrawIndexed(CommandBuffer& commandBuffer, Transform& transform, bool setTexture)
{
	D3D11CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D11CommandBuffer*>(&commandBuffer);
	assert(d3dCommandBuffer != nullptr);

	ID3D11DeviceContext* deviceContext = d3dCommandBuffer->GetDeferredContext();

	unsigned int frameResourceIndex = d3dCommandBuffer->GetFrameResourceIndex();

	/// Set texture
	if (setTexture)
	{
		for (std::pair<std::unique_ptr<Texture>, unsigned int>& texture : m_textures)
		{
			texture.first->SetTexture(commandBuffer, texture.second);
		}
	}

	/// Update world matrix
	XMFLOAT4X4 worldMatrix = GetWorldMatrix(transform);

	deviceContext->UpdateSubresource(m_worldMatrixResources[frameResourceIndex]->GetBuffer(), 0, nullptr, &worldMatrix, 0, 0);

	deviceContext->VSSetConstantBuffers(D3D11Renderer::WorldMatrixShaderSlot(), 1, m_worldMatrixResources[frameResourceIndex]->GetBufferAddress());

	UINT offset = 0;
	UINT stride = (UINT)m_verticesResource->GetDataStride();

	deviceContext->IASetVertexBuffers(0, 1, m_verticesResource->GetBufferAddress(), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indicesResource->GetBuffer(), GetD3DFormat(m_indicesResource->GetDataFormat()), 0);

	deviceContext->DrawIndexed((UINT)m_indicesResource->GetDataCount(), 0, 0);
}

DirectX::XMFLOAT4X4 D3D11RenderObject::GetWorldMatrix(Transform& transform)
{
	XMVECTOR scalingOrigin = XMVectorZero();
	XMVECTOR scalingOrientationQuaternion = XMQuaternionIdentity();
	XMVECTOR scaling = XMVectorSet(transform.scale.x, transform.scale.y, transform.scale.z, 0);
	XMVECTOR rotationOrigin = XMVectorZero();
	XMVECTOR rotation = XMQuaternionRotationMatrix(XMMatrixRotationRollPitchYaw(transform.rotation.x, transform.rotation.y, transform.rotation.z));
	XMVECTOR translation = XMVectorSet(transform.pos.x, transform.pos.y, transform.pos.z, 0);

	XMMATRIX worldXMMatrix = XMMatrixTransformation(scalingOrigin, scalingOrientationQuaternion, scaling,
		rotationOrigin, rotation, translation);

	XMFLOAT4X4 worldMatrix;

	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(worldXMMatrix));

	return worldMatrix;
}
