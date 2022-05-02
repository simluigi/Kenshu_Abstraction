#include "RenderContext.h"

#include "CommandBuffer.h"

RenderContext::RenderContext()
{
}

RenderContext::~RenderContext()
{
}

void RenderContext::SetConstantBufferView(RenderResource& resource, unsigned int shaderSlot, const void* updateData)
{
	GetMainCommandBuffer().SetConstantBufferView(resource, shaderSlot, updateData);
}

void RenderContext::SetRenderTargetTexture(RenderingData& renderingData, const char* renderTargetName, unsigned int shaderSlot)
{
	GetMainCommandBuffer().SetRenderTargetTexture(renderingData, renderTargetName, shaderSlot);
}

void RenderContext::SetDepthTexture(RenderingData& renderingData, const char* depthTextureName, unsigned int shaderSlot)
{
	GetMainCommandBuffer().SetDepthTexture(renderingData, depthTextureName, shaderSlot);
}

void RenderContext::SetCamera(RenderingData& renderingData, const char* cameraName)
{
	GetMainCommandBuffer().SetCamera(renderingData, cameraName);
}

void RenderContext::SetRenderTarget(RenderingData& renderingData, const char* renderTargetName, float* clearColor, const char* depthTextureName)
{
	SetRenderTarget(GetMainCommandBuffer(), renderingData, renderTargetName, clearColor, depthTextureName);
}

void RenderContext::SetSwapChainRenderTarget(float* clearColor, unsigned int backBufferIndex, bool useDepth)
{
	SetSwapChainRenderTarget(GetMainCommandBuffer(), backBufferIndex, clearColor, useDepth);
}

void RenderContext::ResetMainCommandBuffer() const
{
	m_mainCommandBuffer->Reset();
}

void RenderContext::EndFrameRendering(unsigned int backBufferIndex)
{
	EndFrameRendering(GetMainCommandBuffer(), backBufferIndex);
}

CommandBuffer& RenderContext::GetMainCommandBuffer() const
{
	return *m_mainCommandBuffer;
}

std::shared_ptr<CommandBuffer> RenderContext::GetCommandBuffer(const char* bufferName) const
{
	return m_commandBuffers.at(bufferName);
}
