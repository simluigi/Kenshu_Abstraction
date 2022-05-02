#pragma once

#include <d3d11.h>
#include <vector>
#include <wrl/client.h>
#include "../../Rendering/Texture.h"
#include "../../Utility/Math/Vector.h"

class D3D11Texture :
	public Texture
{
public:

	D3D11Texture();
	~D3D11Texture();

	void Create(ID3D11Device* device, const std::vector<uint8_t>& textureData, const Int2& textureSize, unsigned int pixelDataSize);

	void SetTexture(CommandBuffer& commandBuffer, unsigned int shaderSlot)override;

private:

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_textureResource;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
};
