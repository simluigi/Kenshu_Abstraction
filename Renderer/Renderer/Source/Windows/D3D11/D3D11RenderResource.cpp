#include "D3D11RenderResource.h"

#include <cassert>

D3D11RenderResource::D3D11RenderResource()
{
}

D3D11RenderResource::D3D11RenderResource(const char* resourceName, ResourceType resourceType, const void* resourceData, unsigned int dataCount, unsigned int stride, ResourceFormat format) :
	RenderResource(resourceName, resourceType, resourceData, dataCount, stride, format)
{
}

D3D11RenderResource::~D3D11RenderResource()
{
}

D3D11_BIND_FLAG D3D11RenderResource::GetD3D11ResourceBind(ResourceType resourceType)
{
	D3D11_BIND_FLAG bindFlag = {};

	switch (resourceType)
	{
	case ResourceType::VertexBuffer:

		bindFlag = D3D11_BIND_VERTEX_BUFFER;
		break;

	case ResourceType::IndexBuffer:

		bindFlag = D3D11_BIND_INDEX_BUFFER;
		break;

	case ResourceType::ConstantBuffer:

		bindFlag = D3D11_BIND_INDEX_BUFFER;
		break;

	default:

		assert(false);
		break;
	}

	return bindFlag;
}

void D3D11RenderResource::Create(ID3D11Device* device)
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

ID3D11Buffer* D3D11RenderResource::GetBuffer()
{
	return m_buffer.Get();
}

ID3D11Buffer* const* D3D11RenderResource::GetBufferAddress()
{
	return m_buffer.GetAddressOf();
}

void D3D11RenderResource::CreateVertexBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = m_dataSize;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pSysMem = m_resourceData;

	device->CreateBuffer(&bufferDesc, &subResourceData, &m_buffer);
}

void D3D11RenderResource::CreateIndexBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = m_dataSize;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pSysMem = m_resourceData;

	device->CreateBuffer(&bufferDesc, &subResourceData, &m_buffer);
}

void D3D11RenderResource::CreateConstantBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = m_dataSize;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	device->CreateBuffer(&bufferDesc, nullptr, &m_buffer);
}
