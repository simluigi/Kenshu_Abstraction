#pragma once

#include <d3d11.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <memory>
#include <vector>
#include "../../Rendering/RenderContext.h"

class Window;
class D3DSwapChain;

class D3D11RenderContext :
	public RenderContext
{
public:

	D3D11RenderContext(const Window& window);
	~D3D11RenderContext();

	bool Create()override;

	std::shared_ptr<CommandBuffer> CreateCommandBuffer(const char* bufferName)override;

	void ExecuteMainCommandBuffer()override;

	void ExecuteCommandBuffers(CommandBuffer* commandBuffers, unsigned int bufferCount)override;

	/**
	 * @brief Rendering termination process
	 * @param commandBuffer Command buffer to use
	 * @param backBufferIndex Swap chain back buffer index in use
	*/
	void EndFrameRendering(CommandBuffer& commandBuffer, unsigned int backBufferIndex)override;

	void SwapChainPresent(int vsync)override;
	/**
	 * @brief Set renderTarget
	 * @param commandBuffer Command buffer to set render target
	 * @param renderTargetName The name of the render target to set
	 * @param clearColor Screen clear color
	*/
	void SetRenderTarget(CommandBuffer& commandBuffer, RenderingData& renderingData, const char* renderTargetName, float* clearColor, const char* depthTextureName = nullptr)override;

	/**
	 * @brief Set current swapChain renderTarget
	 * @param commandBuffer Command buffer to set render target
	 * @param backBufferIndex Backbuffer index to set
	 * @param clearColor Screen clear color
	*/
	void SetSwapChainRenderTarget(CommandBuffer& commandBuffer, unsigned int backBufferIndex, float* clearColor, bool useDepth)override;

	ID3D11Device* GetDevice()const;

	unsigned int GetSwapChainRenderTargetIndex()const override;

private:

	bool CreateDevice();
	bool CreateSwapChain();
	bool CreateDepthBuffer();

	const Window& m_window;

	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<IDXGIFactory2> m_dxgiFactory;
	std::unique_ptr<D3DSwapChain> m_swapChain;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> m_depthBuffers;
	std::vector<Microsoft::WRL::ComPtr<ID3D11DepthStencilView>> m_depthStencilViews;

	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_immediateContext;
};
