#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <dxgi1_6.h>
#include <memory>
#include "../../Rendering/RenderContext.h"

class Window;
class D3DSwapChain;
class D3D12RenderingData;

class D3D12RenderContext :
	public RenderContext
{
public:

	D3D12RenderContext(const Window& window);
	~D3D12RenderContext();
	
	bool Create()override;

	/**
	 * @brief Set current swapChain renderTarget
	 * @param commandBuffer Command buffer to set render target
	 * @param backBufferIndex Backbuffer index to set
	 * @param clearColor Screen clear color
	*/
	void SetSwapChainRenderTarget(CommandBuffer& commandBuffer, unsigned int backBufferIndex, float* clearColor, bool useDepth)override;

	void SetRenderTarget(CommandBuffer& commandBuffer, RenderingData& renderingData, const char* renderTargetName, float* clearColor, const char* depthTextureName = nullptr)override;

	void ExecuteMainCommandBuffer()override;
	void ExecuteCommandBuffers(CommandBuffer* commandBuffers, unsigned int bufferCount)override;

	void EndFrameRendering(CommandBuffer& commandBuffer, unsigned int backBufferIndex)override;

	std::shared_ptr<CommandBuffer> CreateCommandBuffer(const char* bufferName)override;

	void WaitForCommandExecute();

	void SwapChainPresent(int vsync)override;

	ID3D12Device* GetDevice()const;

	unsigned int GetSwapChainRenderTargetIndex()const override;

private:

	bool CreateDevice();
	bool CreateSwapChain();
	bool CreateCommandQueue();
	bool CreateDepthBuffer();

	void SetResourceBarrier(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	const Window& m_window;

	Microsoft::WRL::ComPtr<ID3D12Device> m_device;
	Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgiFactory;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;

	void* m_fenceEvent = nullptr;
	UINT64 m_fenceValue = 0;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;

	std::unique_ptr<D3DSwapChain> m_swapChain;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_renderTargets;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_renderTargetHandles;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_depthBuffer;
	D3D12_DEPTH_STENCIL_VIEW_DESC m_depthStencilViewDesc = {};
};

