#include "D3D12RenderObject.h"

#include <string>
#include <cassert>
#include <sstream>
#include <DirectXMath.h>
#include "D3D12RenderingData.h"
#include "D3D12RenderResource.h"
#include "D3D12CommandBuffer.h"
#include "D3D12Renderer.h"
#include "../../Rendering/Texture.h"

using namespace DirectX;

D3D12RenderObject::D3D12RenderObject()
{
}

D3D12RenderObject::~D3D12RenderObject()
{
}

void D3D12RenderObject::Create(RenderingData& renderingData, const char* objectName, std::vector<MeshVertex>& vertices, std::vector<uint16_t>& indices)
{
	/// Create vertex buffer
	std::string vertexResourceName = objectName;
	vertexResourceName += "VertexBuffer";

	std::shared_ptr<RenderResource> vertexResource = renderingData.CreateResource(vertexResourceName.c_str(), ResourceType::VertexBuffer,
		vertices.data(), (unsigned int)vertices.size(), (unsigned int)sizeof(vertices[0]));

	m_verticesResource = dynamic_cast<D3D12RenderResource*>(vertexResource.get());
	assert(m_verticesResource != nullptr);

	/// Create index buffer
	std::string indexResourceName = objectName;
	indexResourceName += "IndexBuffer";

	std::shared_ptr<RenderResource> indexResource = renderingData.CreateResource(indexResourceName.c_str(), ResourceType::IndexBuffer,
		indices.data(), (unsigned int)indices.size(), (unsigned int)sizeof(indices[0]), ResourceFormat::R16_UINT);

	m_indicesResource = dynamic_cast<D3D12RenderResource*>(indexResource.get());
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

		m_worldMatrixResources.emplace_back(dynamic_cast<D3D12RenderResource*>(wrldMatrixResource.get()));
		assert(m_worldMatrixResources[frameResourceCount] != nullptr);
	}

	m_vertices = vertices;
	m_indices = indices;
}

void D3D12RenderObject::DrawIndexed(CommandBuffer& commandBuffer)
{
	D3D12CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D12CommandBuffer*>(&commandBuffer);
	assert(d3dCommandBuffer != nullptr);

	ID3D12GraphicsCommandList6* cmdList = d3dCommandBuffer->GetCommandList();

	cmdList->IASetVertexBuffers(0, 1, &m_verticesResource->GetVertexBufferView());
	cmdList->IASetIndexBuffer(&m_indicesResource->GetIndexBufferView());

	/// Draw call
	d3dCommandBuffer->DrawIndexed((unsigned int)m_indices.size());
}

void D3D12RenderObject::DrawIndexed(CommandBuffer& commandBuffer, Transform& transform, bool setTexture)
{
	D3D12CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D12CommandBuffer*>(&commandBuffer);
	assert(d3dCommandBuffer != nullptr);

	ID3D12GraphicsCommandList6* cmdList = d3dCommandBuffer->GetCommandList();

	unsigned int frameResourceIndex = d3dCommandBuffer->GetFrameResourceIndex();

	D3D12RenderResource* resource = m_worldMatrixResources[frameResourceIndex];

	/// Update world matrix
	XMFLOAT4X4 worldMatrix = GetWorldMatrix(transform);
	memcpy(resource->GetMappedPtr(), &worldMatrix, resource->GetDataStride() * resource->GetDataCount());

	/// Set world matrix
	cmdList->SetGraphicsRootConstantBufferView(D3D12Renderer::WorldMatrixShaderSlot(), resource->GetBuffer()->GetGPUVirtualAddress());

	/// Set texture
	if (setTexture)
	{
		for (std::pair<std::unique_ptr<Texture>, unsigned int>& texture : m_textures)
		{
			texture.first->SetTexture(commandBuffer, texture.second);
		}
	}

	cmdList->IASetVertexBuffers(0, 1, &m_verticesResource->GetVertexBufferView());
	cmdList->IASetIndexBuffer(&m_indicesResource->GetIndexBufferView());

	/// Draw call
	d3dCommandBuffer->DrawIndexed((unsigned int)m_indices.size());
}

DirectX::XMFLOAT4X4 D3D12RenderObject::GetWorldMatrix(Transform& transform)
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
