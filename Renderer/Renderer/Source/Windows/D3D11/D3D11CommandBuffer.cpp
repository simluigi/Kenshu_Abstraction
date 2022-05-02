#include "D3D11CommandBuffer.h"

#include <cassert>
#include "D3D11RenderContext.h"
#include "D3D11RenderingData.h"
#include "../D3DUtility/D3DCamera.h"
#include "D3D11RenderResource.h"
#include "D3D11Renderer.h"

D3D11CommandBuffer::D3D11CommandBuffer()
{
}

D3D11CommandBuffer::~D3D11CommandBuffer()
{
}

bool D3D11CommandBuffer::Create(RenderContext& renderContext)
{
	D3D11RenderContext* d3dRenderContext = dynamic_cast<D3D11RenderContext*>(&renderContext);
	assert(d3dRenderContext != nullptr);

	HRESULT result = d3dRenderContext->GetDevice()->CreateDeferredContext(0, &m_deferredContext);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void D3D11CommandBuffer::DrawIndexed(unsigned int indexCount)
{
	m_deferredContext->DrawIndexed(indexCount, 0, 0);
}

void D3D11CommandBuffer::Release()
{
	m_deferredContext->Release();
}

void D3D11CommandBuffer::SetCamera(RenderingData& renderingData, const char* cameraName)
{
	D3D11RenderingData* d3dRenderingData = dynamic_cast<D3D11RenderingData*>(&renderingData);
	assert(d3dRenderingData != nullptr);

	Camera& camera = renderingData.GetCamera(cameraName);

	D3DCamera* d3dCamera = dynamic_cast<D3DCamera*>(&camera);
	assert(d3dCamera != nullptr);

	D3D11RenderResource& renderResource = d3dRenderingData->GetCameraResource(cameraName, m_frameResourceIndex);

	SetConstantBufferView(renderResource, Renderer::CameraShaderSlot(), &d3dCamera->GetCameraInfo());
}

void D3D11CommandBuffer::SetLightViewCamera(RenderingData& renderingData, const char* cameraName)
{
	D3D11RenderingData* d3dRenderingData = dynamic_cast<D3D11RenderingData*>(&renderingData);
	assert(d3dRenderingData != nullptr);

	Camera& camera = renderingData.GetCamera(cameraName);

	D3DCamera* d3dCamera = dynamic_cast<D3DCamera*>(&camera);
	assert(d3dCamera != nullptr);

	D3D11RenderResource& renderResource = d3dRenderingData->GetCameraResource(cameraName, m_frameResourceIndex);

	SetConstantBufferView(renderResource, Renderer::LightViewCameraShaderSlot(), &d3dCamera->GetCameraInfo());
}

void D3D11CommandBuffer::SetConstantBufferView(RenderResource& renderResource, unsigned int shaderSlot, const void* updateData)
{
	D3D11RenderResource* d3dRenderResource = dynamic_cast<D3D11RenderResource*>(&renderResource);
	assert(d3dRenderResource != nullptr);

	m_deferredContext->UpdateSubresource(d3dRenderResource->GetBuffer(), 0, nullptr, updateData, 0, 0);

	m_deferredContext->VSSetConstantBuffers(shaderSlot, 1, d3dRenderResource->GetBufferAddress());
}

void D3D11CommandBuffer::SetRenderTargetTexture(RenderingData& renderingData, const char* renderTargetName, unsigned int shaderSlot)
{
	D3D11RenderingData* d3dRenderingData = dynamic_cast<D3D11RenderingData*>(&renderingData);
	assert(d3dRenderingData != nullptr);

	ID3D11ShaderResourceView* shaderResourceView = d3dRenderingData->GetShaderResourceView(renderTargetName);
	ID3D11SamplerState* samplerState = d3dRenderingData->GetSamplerState();

	m_deferredContext->PSSetShaderResources(shaderSlot, 1, &shaderResourceView);
	m_deferredContext->PSSetSamplers(0, 1, &samplerState);
}

void D3D11CommandBuffer::SetDepthTexture(RenderingData& renderingData, const char* depthTextureName, unsigned int shaderSlot)
{
	D3D11RenderingData* d3dRenderingData = dynamic_cast<D3D11RenderingData*>(&renderingData);
	assert(d3dRenderingData != nullptr);

	ID3D11ShaderResourceView* shaderResourceView = d3dRenderingData->GetDepthTextureShaderResourceView(depthTextureName);
	ID3D11SamplerState* samplerState = d3dRenderingData->GetSamplerState();

	m_deferredContext->PSSetShaderResources(shaderSlot, 1, &shaderResourceView);
	m_deferredContext->PSSetSamplers(0, 1, &samplerState);
}

ID3D11DeviceContext* D3D11CommandBuffer::GetDeferredContext() const
{
	return m_deferredContext.Get();
}
