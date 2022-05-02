#include "D3D11Texture.h"

#include <cassert>
#include "D3D11CommandBuffer.h"

D3D11Texture::D3D11Texture()
{
}

D3D11Texture::~D3D11Texture()
{
}

void D3D11Texture::Create(ID3D11Device* device, const std::vector<uint8_t>& textureData, const Int2& textureSize, unsigned int pixelDataSize)
{
	HRESULT result = S_OK;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = textureSize.x;
	textureDesc.Height = textureSize.y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc = { 1, 0 };
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = &textureData[0];
	subresourceData.SysMemPitch = textureSize.x * pixelDataSize;
	subresourceData.SysMemSlicePitch = textureSize.x * textureSize.y * pixelDataSize;

	device->CreateTexture2D(&textureDesc, &subresourceData, &m_textureResource);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(m_textureResource.Get(), &srvDesc, &m_shaderResourceView);

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_samplerState);
}

void D3D11Texture::SetTexture(CommandBuffer& commandBuffer, unsigned int shaderSlot)
{
	D3D11CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D11CommandBuffer*>(&commandBuffer);
	assert(d3dCommandBuffer != nullptr);

	ID3D11DeviceContext* deviceContext = d3dCommandBuffer->GetDeferredContext();

	deviceContext->PSSetShaderResources(shaderSlot, 1, m_shaderResourceView.GetAddressOf());
	deviceContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
}
