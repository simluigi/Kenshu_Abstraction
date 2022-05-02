#include "D3D12Texture.h"

#include <cassert>
#include "D3D12CommandBuffer.h"

D3D12Texture::D3D12Texture()
{
}

D3D12Texture::~D3D12Texture()
{
}

void D3D12Texture::Create(ID3D12Device* device, const std::vector<uint8_t>& textureData, const Int2& textureSize, unsigned int pixelDataSize)
{
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = textureSize.x;
	resourceDesc.Height = textureSize.y;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resourceDesc.SampleDesc.Count = 1;

	device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_textureResource));

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_descriptorHeap));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	device->CreateShaderResourceView(m_textureResource.Get(), &srvDesc, m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());

	D3D12_BOX box = { 0, 0, 0, (UINT)textureSize.x, (UINT)textureSize.y, 1 };

	m_textureResource->WriteToSubresource(0, &box, textureData.data(), textureSize.x * pixelDataSize, textureSize.x * textureSize.y * pixelDataSize);
}

void D3D12Texture::SetTexture(CommandBuffer& commandBuffer, unsigned int shaderSlot)
{
	D3D12CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D12CommandBuffer*>(&commandBuffer);
	assert(d3dCommandBuffer != nullptr);

	ID3D12GraphicsCommandList6* cmdList = d3dCommandBuffer->GetCommandList();

	cmdList->SetDescriptorHeaps(1, m_descriptorHeap.GetAddressOf());

	UINT rootParamIndex = (UINT)d3dCommandBuffer->GetCurrentPipelineRootParamIndex()->srvIndex[shaderSlot];

	cmdList->SetGraphicsRootDescriptorTable(rootParamIndex, m_descriptorHeap->GetGPUDescriptorHandleForHeapStart());
}
