#include "D3D12RenderTarget.h"

#include "../D3DUtility/D3DUtility.h"

using namespace D3DUtility;

D3D12RenderTarget::D3D12RenderTarget(const char* resourceName, const Int2& resourceSize, ResourceFormat format) :
	RenderTarget(resourceName, resourceSize, format)
{
}

D3D12RenderTarget::~D3D12RenderTarget()
{
}

bool D3D12RenderTarget::Create(ID3D12Device* device)
{
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = (UINT64)m_resourceSize.x;
	resourceDesc.Height = (UINT)m_resourceSize.y;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = GetD3DFormat(m_format);
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = GetD3DFormat(m_format);
	clearValue.Color[0] = 0.5843f;
	clearValue.Color[1] = 0.7529f;
	clearValue.Color[2] = 0.9255f;
	clearValue.Color[3] = 0.0f;

	HRESULT result = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &clearValue, IID_PPV_ARGS(&m_renderTargetResource));

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

ID3D12Resource* D3D12RenderTarget::GetResource() const
{
	return m_renderTargetResource.Get();
}
