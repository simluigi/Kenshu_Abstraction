#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "../../Rendering/RenderResource.h"
#include "../../Rendering/RenderCommon.h"

class D3D11RenderResource :
	public RenderResource
{
public:

	D3D11RenderResource();
	D3D11RenderResource(const char* resourceName, ResourceType resourceType, const void* resourceData,
		unsigned int dataCount, unsigned int stride, ResourceFormat format = ResourceFormat::UNKNOWN);

	~D3D11RenderResource();

	static D3D11_BIND_FLAG GetD3D11ResourceBind(ResourceType resourceType);

	void Create(ID3D11Device* device);

	ID3D11Buffer* GetBuffer();
	ID3D11Buffer* const* GetBufferAddress();

private:

	void CreateVertexBuffer(ID3D11Device* device);
	void CreateIndexBuffer(ID3D11Device* device);
	void CreateConstantBuffer(ID3D11Device* device);

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
};
