#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include "../../Rendering/RenderResource.h"

class D3D12RenderResource :
	public RenderResource
{
public:

	D3D12RenderResource();
	D3D12RenderResource(const char* resourceName, ResourceType resourceType, const void* resourceData,
		unsigned int dataCount, unsigned int stride, ResourceFormat format = ResourceFormat::UNKNOWN);

	~D3D12RenderResource();

	void Create(ID3D12Device* device);
	void CreateDepthTexture(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, const Int2& textureSize, float depthClearValue);

	ID3D12Resource* GetBuffer();
	ID3D12Resource* const* GetBufferAddress();

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView()const;
	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView()const;

	void Map();
	void Unmap();

	void* GetMappedPtr()const;

private:

	void CreateVertexBuffer(ID3D12Device* device);
	void CreateIndexBuffer(ID3D12Device* device);
	void CreateConstantBuffer(ID3D12Device* device);
	void CreateDepthBuffer(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, const Int2& textureSize, float depthClearValue);

	Microsoft::WRL::ComPtr<ID3D12Resource> m_buffer;

	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};

	void* m_mappedPtr = nullptr;
};

