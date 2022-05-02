#pragma once

#include <d3d12.h>
#include <vector>
#include <wrl/client.h>
#include "../../Utility/Math/Vector.h"
#include "../../Rendering/Texture.h"

class D3D12Texture :
	public Texture
{
public:

	D3D12Texture();
	~D3D12Texture();

	void Create(ID3D12Device* device, const std::vector<uint8_t>& textureData, const Int2& textureSize, unsigned int pixelDataSize);

	void SetTexture(CommandBuffer& commandBuffer, unsigned int shaderSlot)override;

private:

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_textureResource;
};

