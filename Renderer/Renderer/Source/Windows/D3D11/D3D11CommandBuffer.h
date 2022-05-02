#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "../../Rendering/CommandBuffer.h"

class D3D11CommandBuffer :
	public CommandBuffer
{
public:

	D3D11CommandBuffer();
	~D3D11CommandBuffer();

	bool Create(RenderContext& renderContext)override;

	void DrawIndexed(unsigned int indexCount)override;

	void Release()override;

	/**
	 * @brief Set camera constant buffer
	*/
	void SetCamera(RenderingData& renderingData, const char* cameraName)override;

	/**
	 * @brief Set camera constant buffer
	*/
	void SetLightViewCamera(RenderingData& renderingData, const char* cameraName)override;

	/**
	 * @brief Set constant buffer view
	 * @param renderResource Resources to set
	 * @param shaderSlot Shader slot to set
	 * @param updateData Data for update
	*/
	void SetConstantBufferView(RenderResource& renderResource, unsigned int shaderSlot, const void* updateData = nullptr)override;

	/**
	 * @brief Set render target shader resource view
	 * @param renderTargetName The name of the render target to set
	 * @param shaderSlot Shader slot to set
	*/
	void SetRenderTargetTexture(RenderingData& renderingData, const char* renderTargetName, unsigned int shaderSlot)override;

	/**
	 * @brief Set depth texture shader resource view
	 * @param depthTextureName The name of the depth texture to set
	 * @param shaderSlot Shader slot to set
	*/
	void SetDepthTexture(RenderingData& renderingData, const char* depthTextureName, unsigned int shaderSlot)override;

	ID3D11DeviceContext* GetDeferredContext()const;

private:

	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deferredContext;
};
