#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include "../../Rendering/RenderCommon.h"
#include "../../Rendering/CommandBuffer.h"

class D3D12CommandBuffer :
	public CommandBuffer
{
public:

	D3D12CommandBuffer();
	~D3D12CommandBuffer();

	bool Create(RenderContext& renderContext)override;

	void DrawIndexed(unsigned int indexCount);

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

	/**
	 * @brief Reset command list & command allocator 
	*/
	void Reset()override;

	/**
	 * @brief Close command list
	*/
	void Close()override;

	void SetCurrentPipelineRootParamIndex(RootParameterIndex* rootParamIndex);
	RootParameterIndex* GetCurrentPipelineRootParamIndex()const;

	ID3D12GraphicsCommandList6* GetCommandList()const;

private:

	void SetResourceBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_commandList;

	RootParameterIndex* m_currentPipelineRootParamIndex = nullptr; /// Convert from shader slot to root parameter index
};

