#include "D3D12RenderResource.h"

#include <cassert>
#include "../D3DUtility/D3DUtility.h"

using namespace D3DUtility;

D3D12RenderResource::D3D12RenderResource()
{
}

D3D12RenderResource::D3D12RenderResource(const char* resourceName, ResourceType resourceType, const void* resourceData, unsigned int dataCount, unsigned int stride, ResourceFormat format) :
	RenderResource(resourceName, resourceType, resourceData, dataCount, stride, format)
{
}

D3D12RenderResource::~D3D12RenderResource()
{
	Unmap();
}

void D3D12RenderResource::Create(ID3D12Device* device)
{
	switch (m_resourceType)
	{
	case ResourceType::VertexBuffer:

		CreateVertexBuffer(device);
		break;

	case ResourceType::IndexBuffer:

		CreateIndexBuffer(device);
		break;

	case ResourceType::ConstantBuffer:

		CreateConstantBuffer(device);
		break;

	default:

		assert(false);
		break;
	}
}

void D3D12RenderResource::CreateDepthTexture(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, const Int2& textureSize, float depthClearValue)
{
	m_resourceType = ResourceType::DepthBuffer;

	CreateDepthBuffer(device, cpuHandle, textureSize, depthClearValue);
}

ID3D12Resource* D3D12RenderResource::GetBuffer()
{
	return m_buffer.Get();
}

ID3D12Resource* const* D3D12RenderResource::GetBufferAddress()
{
	return m_buffer.GetAddressOf();
}

const D3D12_VERTEX_BUFFER_VIEW& D3D12RenderResource::GetVertexBufferView() const
{
	return m_vertexBufferView;
}

const D3D12_INDEX_BUFFER_VIEW& D3D12RenderResource::GetIndexBufferView() const
{
	return m_indexBufferView;
}

void D3D12RenderResource::Map()
{
	if (m_mappedPtr != nullptr)
	{
		return;
	}

	m_buffer->Map(0, nullptr, &m_mappedPtr);
}

void D3D12RenderResource::Unmap()
{
	if (m_mappedPtr == nullptr)
	{
		return;
	}

	m_buffer->Unmap(0, nullptr);

	m_mappedPtr = nullptr;
}

void* D3D12RenderResource::GetMappedPtr() const
{
	return m_mappedPtr;
}

void D3D12RenderResource::CreateVertexBuffer(ID3D12Device* device)
{
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = m_dataSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.SampleDesc.Count = 1;

	device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_buffer));

	void* mappedPtr = nullptr;

	m_buffer->Map(0, nullptr, &mappedPtr);
	memcpy(mappedPtr, m_resourceData, m_dataSize);
	m_buffer->Unmap(0, nullptr);

	m_vertexBufferView.BufferLocation = m_buffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_dataStride;
	m_vertexBufferView.SizeInBytes = m_dataSize;
}

void D3D12RenderResource::CreateIndexBuffer(ID3D12Device* device)
{
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = m_dataSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.SampleDesc.Count = 1;

	device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_buffer));

	void* mappedPtr = nullptr;

	m_buffer->Map(0, nullptr, &mappedPtr);
	memcpy(mappedPtr, m_resourceData, m_dataSize);
	m_buffer->Unmap(0, nullptr);

	m_indexBufferView.BufferLocation = m_buffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = GetD3DFormat(m_dataFormat);
	m_indexBufferView.SizeInBytes = m_dataSize;
}

void D3D12RenderResource::CreateConstantBuffer(ID3D12Device* device)
{
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = m_dataSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_buffer));

	Map();
}

void D3D12RenderResource::CreateDepthBuffer(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, const Int2& textureSize, float depthClearValue)
{
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = textureSize.x;
	resourceDesc.Height = textureSize.y;
	resourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.SampleDesc.Count = 1;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = (FLOAT)depthClearValue;

	device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&m_buffer));

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(m_buffer.Get(), &depthStencilViewDesc, cpuHandle);
}
