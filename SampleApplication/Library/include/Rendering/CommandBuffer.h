#pragma once

class Camera;
class RenderObject;
class RenderingData;
class RenderContext;
class RenderResource;

class CommandBuffer
{
public:

	CommandBuffer();
	virtual ~CommandBuffer();

	virtual bool Create(RenderContext& renderContext) = 0;

	virtual void Release() = 0;

	/**
	 * @brief Reset command buffer
	*/
	virtual void Reset() {};

	/**
	 * @brief Close command buffer
	*/
	virtual void Close() {};

	virtual void DrawIndexed(unsigned int indexCount) = 0;

	/**
	 * @brief Set camera constant buffer
	*/
	virtual void SetCamera(RenderingData& renderingData, const char* cameraName) = 0;

	/**
	 * @brief Set light view camera constant buffer
	*/
	virtual void SetLightViewCamera(RenderingData& renderingData, const char* cameraName) = 0;

	/**
	 * @brief Set constant buffer view
	 * @param renderResource Resources to set
	 * @param shaderSlot Shader slot to set
	 * @param updateData Data for update
	*/
	virtual void SetConstantBufferView(RenderResource& renderResource, unsigned int shaderSlot, const void* updateData = nullptr) = 0;

	/**
	 * @brief Set render target shader resource view
	 * @param renderTargetName The name of the render target to set
	 * @param shaderSlot Shader slot to set
	*/
	virtual void SetRenderTargetTexture(RenderingData& renderingData, const char* renderTargetName, unsigned int shaderSlot) = 0;

	/**
	 * @brief Set depth texture shader resource view
	 * @param depthTextureName The name of the depth texture to set
	 * @param shaderSlot Shader slot to set
	*/
	virtual void SetDepthTexture(RenderingData& renderingData, const char* depthTextureName, unsigned int shaderSlot) = 0;

	/**
	 * @brief Set index of resources to access in the current frame
	*/
	void SetFrameResourceIndex(unsigned int frameResourceIndex);

	/**
	 * @brief Get the resource index used in the current frame
	*/
	unsigned int GetFrameResourceIndex()const;

protected:

	unsigned int m_frameResourceIndex = 0;
};