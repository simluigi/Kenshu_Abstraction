#include "D3D12CommandBuffer.h"

#include <cassert>
#include "../D3DUtility/D3DCamera.h"
#include "D3D12RenderContext.h"
#include "D3D12RenderResource.h"
#include "D3D12RenderingData.h"
#include "../../Rendering/Renderer.h"

D3D12CommandBuffer::D3D12CommandBuffer()
{
}

D3D12CommandBuffer::~D3D12CommandBuffer()
{
}

bool D3D12CommandBuffer::Create(RenderContext& renderContext)
{
    D3D12RenderContext* d3dRenderContext = dynamic_cast<D3D12RenderContext*>(&renderContext);
    assert(d3dRenderContext != nullptr);

    ID3D12Device* device = d3dRenderContext->GetDevice();

	HRESULT result = S_OK;

	D3D12_COMMAND_LIST_TYPE commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;

	result = device->CreateCommandAllocator(commandListType, IID_PPV_ARGS(&m_commandAllocator));

	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateCommandList(0, commandListType, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));

	if (FAILED(result))
	{
		return false;
	}

    return true;
}

void D3D12CommandBuffer::DrawIndexed(unsigned int indexCount)
{
	m_commandList->DrawIndexedInstanced((UINT)indexCount, 1, 0, 0, 0);
}

void D3D12CommandBuffer::Release()
{
	m_commandAllocator->Release();
	m_commandList->Release();
}

void D3D12CommandBuffer::SetCamera(RenderingData& renderingData, const char* cameraName)
{
	D3D12RenderingData* d3dRenderingData = dynamic_cast<D3D12RenderingData*>(&renderingData);
	assert(d3dRenderingData != nullptr);

	Camera& camera = renderingData.GetCamera(cameraName);

	D3DCamera* d3dCamera = dynamic_cast<D3DCamera*>(&camera);
	assert(d3dCamera != nullptr);

	D3D12RenderResource& renderResource = d3dRenderingData->GetCameraResource(cameraName, m_frameResourceIndex);

	SetConstantBufferView(renderResource, Renderer::CameraShaderSlot(), &d3dCamera->GetCameraInfo());
}

void D3D12CommandBuffer::SetLightViewCamera(RenderingData& renderingData, const char* cameraName)
{
	D3D12RenderingData* d3dRenderingData = dynamic_cast<D3D12RenderingData*>(&renderingData);
	assert(d3dRenderingData != nullptr);

	Camera& camera = renderingData.GetCamera(cameraName);

	D3DCamera* d3dCamera = dynamic_cast<D3DCamera*>(&camera);
	assert(d3dCamera != nullptr);

	D3D12RenderResource& renderResource = d3dRenderingData->GetCameraResource(cameraName, m_frameResourceIndex);

	SetConstantBufferView(renderResource, Renderer::LightViewCameraShaderSlot(), &d3dCamera->GetCameraInfo());
}

void D3D12CommandBuffer::SetConstantBufferView(RenderResource& renderResource, unsigned int shaderSlot, const void* updateData)
{
	D3D12RenderResource* d3dRenderResource = dynamic_cast<D3D12RenderResource*>(&renderResource);
	assert(d3dRenderResource != nullptr);

	UINT rootParamIndex = (UINT)m_currentPipelineRootParamIndex->cbvIndex[shaderSlot];

	if (updateData != nullptr)
	{
		memcpy(d3dRenderResource->GetMappedPtr(), updateData, d3dRenderResource->GetDataStride() * d3dRenderResource->GetDataCount());
	}

	m_commandList->SetGraphicsRootConstantBufferView(rootParamIndex, d3dRenderResource->GetBuffer()->GetGPUVirtualAddress());
}

void D3D12CommandBuffer::SetRenderTargetTexture(RenderingData& renderingData, const char* renderTargetName, unsigned int shaderSlot)
{
	D3D12RenderingData* d3dRenderingData = dynamic_cast<D3D12RenderingData*>(&renderingData);
	assert(d3dRenderingData != nullptr);

	d3dRenderingData->SetRenderTargetResourceBarrier(renderTargetName, m_frameResourceIndex, m_commandList.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	d3dRenderingData->SetRenderTargetSRVHeap(*this);

	UINT rootParamIndex = (UINT)m_currentPipelineRootParamIndex->srvIndex[shaderSlot];

	m_commandList->SetGraphicsRootDescriptorTable(rootParamIndex, d3dRenderingData->GetRenderTargetGPUHandle(renderTargetName, m_frameResourceIndex));
}

void D3D12CommandBuffer::SetDepthTexture(RenderingData& renderingData, const char* depthTextureName, unsigned int shaderSlot)
{
	D3D12RenderingData* d3dRenderingData = dynamic_cast<D3D12RenderingData*>(&renderingData);
	assert(d3dRenderingData != nullptr);

	d3dRenderingData->SetDepthResourceBarrier(depthTextureName, m_frameResourceIndex, m_commandList.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	d3dRenderingData->SetRenderTargetSRVHeap(*this);

	UINT rootParamIndex = (UINT)m_currentPipelineRootParamIndex->srvIndex[shaderSlot];

	m_commandList->SetGraphicsRootDescriptorTable(rootParamIndex, d3dRenderingData->GetDepthTextureGPUHandle(depthTextureName, m_frameResourceIndex));
}

void D3D12CommandBuffer::Reset()
{
	m_commandAllocator->Reset();
	m_commandList->Reset(m_commandAllocator.Get(), nullptr);
}

void D3D12CommandBuffer::Close()
{
	m_commandList->Close();
}

void D3D12CommandBuffer::SetCurrentPipelineRootParamIndex(RootParameterIndex* rootParamIndex)
{
	m_currentPipelineRootParamIndex = rootParamIndex;
}

RootParameterIndex* D3D12CommandBuffer::GetCurrentPipelineRootParamIndex() const
{
	return m_currentPipelineRootParamIndex;
}

ID3D12GraphicsCommandList6* D3D12CommandBuffer::GetCommandList() const
{
	return m_commandList.Get();
}

void D3D12CommandBuffer::SetResourceBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
	D3D12_RESOURCE_BARRIER desc = {};
	desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	desc.Transition.StateBefore = before;
	desc.Transition.StateAfter = after;
	desc.Transition.pResource = resource;
	desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_commandList->ResourceBarrier(1, &desc);
}
