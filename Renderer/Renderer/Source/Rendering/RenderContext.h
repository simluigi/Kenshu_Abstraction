#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

class CommandBuffer;
class RenderingData;
class RenderResource;

class RenderContext
{
public:

	RenderContext();
	virtual ~RenderContext();

	virtual bool Create() = 0;

	virtual std::shared_ptr<CommandBuffer> CreateCommandBuffer(const char* bufferName) = 0;

	virtual void ExecuteMainCommandBuffer() = 0;

	virtual void ExecuteCommandBuffers(CommandBuffer* commandBuffers, unsigned int bufferCount) = 0;

	virtual void WaitForCommandExecute() {};

	virtual void Terminate() {};

	void ResetMainCommandBuffer()const;

	/**
	 * @brief Rendering termination process (Use main command buffer)
	 * @param backBufferIndex Swap chain back buffer index in use
	*/
	void EndFrameRendering(unsigned int backBufferIndex);

	/**
	 * @brief Rendering termination process
	 * @param commandBuffer Command buffer to use
	 * @param backBufferIndex Swap chain back buffer index in use
	*/
	virtual void EndFrameRendering(CommandBuffer& commandBuffer, unsigned int backBufferIndex) = 0;

	virtual void SwapChainPresent(int vsync) = 0;

	/**
	 * @brief Set constant buffer view
	 * @param renderResource Resources to set
	 * @param shaderSlot Shader slot to set
	 * @param updateData Data for update
	*/
	void SetConstantBufferView(RenderResource& resource, unsigned int shaderSlot, const void* updateData);

	/**
	 * @brief Set render target shader resource view
	 * @param renderTargetName The name of the render target to set
	 * @param shaderSlot Shader slot to set
	*/
	void SetRenderTargetTexture(RenderingData& renderingData, const char* renderTargetName, unsigned int shaderSlot);

	/**
	 * @brief Set depth texture shader resource view
	 * @param depthTextureName The name of the depth texture to set
	 * @param shaderSlot Shader slot to set
	*/
	void SetDepthTexture(RenderingData& renderingData, const char* depthTextureName, unsigned int shaderSlot);

	/**
	 * @brief Set camera constant buffer
	*/
	void SetCamera(RenderingData& renderingData, const char* cameraName);

	/**
	 * @brief Set renderTarget (Use main command buffer)
	 * @param renderTargetName The name of the render target to set
	 * @param clearColor Screen clear color
	*/
	void SetRenderTarget(RenderingData& renderingData, const char* renderTargetName, float* clearColor, const char* depthTextureName = nullptr);

	/**
	 * @brief Set renderTarget
	 * @param commandBuffer Command buffer to set render target
	 * @param renderTargetName The name of the render target to set
	 * @param clearColor Screen clear color
	*/
	virtual void SetRenderTarget(CommandBuffer& commandBuffer, RenderingData& renderingData, const char* renderTargetName, float* clearColor, const char* depthTextureName = nullptr) = 0;

	/**
	 * @brief Set current swapChain renderTarget
	 * @param clearColor Screen clear color
	*/
	void SetSwapChainRenderTarget(float* clearColor, unsigned int backBufferIndex, bool useDepth);

	/**
	 * @brief Set current swapChain renderTarget
	 * @param commandBuffer Command buffer to set render target
	 * @param backBufferIndex Backbuffer index to set
	 * @param clearColor Screen clear color
	*/
	virtual void SetSwapChainRenderTarget(CommandBuffer& commandBuffer, unsigned int backBufferIndex, float* clearColor, bool useDepth) = 0;

	CommandBuffer& GetMainCommandBuffer()const;
	std::shared_ptr<CommandBuffer> GetCommandBuffer(const char* bufferName)const;

	virtual unsigned int GetSwapChainRenderTargetIndex()const = 0;

protected:

	std::unique_ptr<CommandBuffer> m_mainCommandBuffer;
	std::unordered_map<std::string, std::shared_ptr<CommandBuffer>> m_commandBuffers;
};