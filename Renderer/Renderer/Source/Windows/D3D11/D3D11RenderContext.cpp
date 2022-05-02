#include "D3D11RenderContext.h"

#include <cassert>
#include "../Window.h"
#include "../D3DUtility/D3DSwapChain.h"
#include "D3D11CommandBuffer.h"
#include "D3D11RenderingData.h"

using namespace Microsoft::WRL;

constexpr unsigned int SWAPCHAIN_BUFFER_COUNT = 2;

D3D11RenderContext::D3D11RenderContext(const Window& window) :
	m_window(window)
{
}

D3D11RenderContext::~D3D11RenderContext()
{

}

bool D3D11RenderContext::Create()
{
	if (!CreateDevice())
	{
		return false;
	}

	if (!CreateSwapChain())
	{
		return false;
	}

	if (!CreateDepthBuffer())
	{
		return false;
	}

	m_mainCommandBuffer = std::make_unique<D3D11CommandBuffer>();
	m_mainCommandBuffer->Create(*this);

	return true;
}

std::shared_ptr<CommandBuffer> D3D11RenderContext::CreateCommandBuffer(const char* bufferName)
{
	std::shared_ptr<D3D11CommandBuffer> commandBuffer = std::make_shared<D3D11CommandBuffer>();

	commandBuffer->Create(*this);

	m_commandBuffers[bufferName] = commandBuffer;

	return m_commandBuffers[bufferName];
}

void D3D11RenderContext::ExecuteMainCommandBuffer()
{
	D3D11CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D11CommandBuffer*>(&GetMainCommandBuffer());
	assert(d3dCommandBuffer != nullptr);

	ComPtr<ID3D11CommandList> cmdList;

	ID3D11DeviceContext* deviceContext = d3dCommandBuffer->GetDeferredContext();

	deviceContext->FinishCommandList(false, &cmdList);
	m_immediateContext->ExecuteCommandList(cmdList.Get(), false);
}

void D3D11RenderContext::ExecuteCommandBuffers(CommandBuffer* commandBuffers, unsigned int bufferCount)
{
	ComPtr<ID3D11CommandList> cmdList;

	for (unsigned int commandBufferIndex = 0; commandBufferIndex < bufferCount; ++commandBufferIndex)
	{
		D3D11CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D11CommandBuffer*>(&commandBuffers[commandBufferIndex]);
		assert(d3dCommandBuffer != nullptr);

		ID3D11DeviceContext* deviceContext = d3dCommandBuffer->GetDeferredContext();

		deviceContext->FinishCommandList(false, &cmdList);
		m_immediateContext->ExecuteCommandList(cmdList.Get(), false);
	}
}

void D3D11RenderContext::EndFrameRendering(CommandBuffer& commandBuffer, unsigned int backBufferIndex)
{
	if (commandBuffer.GetFrameResourceIndex() >= SWAPCHAIN_BUFFER_COUNT)
	{
		assert(false);
	}

	if (backBufferIndex >= SWAPCHAIN_BUFFER_COUNT)
	{
		assert(false);
	}
}

void D3D11RenderContext::SwapChainPresent(int vsync)
{
	m_swapChain->Present(vsync);
}

void D3D11RenderContext::SetRenderTarget(CommandBuffer& commandBuffer, RenderingData& renderingData, const char* renderTargetName, float* clearColor, const char* depthTextureName)
{
	D3D11CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D11CommandBuffer*>(&commandBuffer);
	assert(d3dCommandBuffer != nullptr);

	D3D11RenderingData* d3dRenderingData = dynamic_cast<D3D11RenderingData*>(&renderingData);
	assert(d3dRenderingData != nullptr);

	ID3D11RenderTargetView* renderTargetView = renderTargetName == nullptr ? nullptr : d3dRenderingData->GetRenderTargetView(renderTargetName);
	ID3D11DepthStencilView* depthView = depthTextureName == nullptr ? nullptr : d3dRenderingData->GetDepthStencilView(depthTextureName);

	ID3D11DeviceContext* deviceContext = d3dCommandBuffer->GetDeferredContext();

	if (renderTargetName != nullptr)
	{
		deviceContext->OMSetRenderTargets(1, &renderTargetView, depthView);
		deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
	}
	else
	{
		deviceContext->OMSetRenderTargets(0, nullptr, depthView);
	}

	if (depthTextureName != nullptr)
	{
		deviceContext->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void D3D11RenderContext::SetSwapChainRenderTarget(CommandBuffer& commandBuffer, unsigned int backBufferIndex, float* clearColor, bool useDepth)
{
	D3D11CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D11CommandBuffer*>(&commandBuffer);
	assert(d3dCommandBuffer != nullptr);

	ID3D11DepthStencilView* depthView = useDepth ? m_depthStencilViews[backBufferIndex].Get() : nullptr;

	ID3D11DeviceContext* deviceContext = d3dCommandBuffer->GetDeferredContext();

	deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), depthView);

	deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);

	if (useDepth)
	{
		deviceContext->ClearDepthStencilView(m_depthStencilViews[backBufferIndex].Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

ID3D11Device* D3D11RenderContext::GetDevice() const
{
	return m_device.Get();
}

unsigned int D3D11RenderContext::GetSwapChainRenderTargetIndex() const
{
	return (unsigned int)m_swapChain->Get()->GetCurrentBackBufferIndex();
}

bool D3D11RenderContext::CreateDevice()
{
	HRESULT result = S_OK;

	UINT debugFlag = 0;

#ifdef _DEBUG

	debugFlag |= D3D11_CREATE_DEVICE_DEBUG;

#endif // _DEBUG

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

	result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, debugFlag,
		featureLevels, _countof(featureLevels), D3D11_SDK_VERSION,
		&m_device, nullptr, &m_immediateContext);

	if (FAILED(result))
	{
		return false;
	}

	ComPtr<IDXGIDevice2> dxgiDevice;
	m_device.As(&dxgiDevice);

	ComPtr<IDXGIAdapter> dxgiAdapter;
	dxgiDevice->GetAdapter(&dxgiAdapter);

	dxgiAdapter->GetParent(IID_PPV_ARGS(&m_dxgiFactory));

	return true;
}

bool D3D11RenderContext::CreateSwapChain()
{
	m_swapChain = std::make_unique<D3DSwapChain>();

	if (!m_swapChain->Create(m_device.Get(), m_dxgiFactory.Get(),
		m_window.GetWindowHandle(), m_window.GetWindowSize(),
		DXGI_FORMAT_R8G8B8A8_UNORM, SWAPCHAIN_BUFFER_COUNT))
	{
		return false;
	}

	ID3D11Texture2D* backBuffer = nullptr;

	HRESULT result = m_swapChain->Get()->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

	if (FAILED(result))
	{
		return false;
	}

	result = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);

	if (FAILED(result))
	{
		return false;
	}

	backBuffer->Release();

	return true;
}

bool D3D11RenderContext::CreateDepthBuffer()
{
	HRESULT result = S_OK;

	Int2 windowSize = m_window.GetWindowSize();

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = windowSize.x;
	textureDesc.Height = windowSize.y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.SampleDesc = { 1, 0 };
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	m_depthBuffers.resize(SWAPCHAIN_BUFFER_COUNT);
	m_depthStencilViews.resize(SWAPCHAIN_BUFFER_COUNT);

	for (unsigned int bufferCount = 0; bufferCount < SWAPCHAIN_BUFFER_COUNT; ++bufferCount)
	{
		result = m_device->CreateTexture2D(&textureDesc, nullptr, &m_depthBuffers[bufferCount]);

		if (FAILED(result))
		{
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc = {};
		dsDesc.Format = textureDesc.Format;
		dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsDesc.Texture2D.MipSlice = 0;

		result = m_device->CreateDepthStencilView(m_depthBuffers[bufferCount].Get(), &dsDesc, &m_depthStencilViews[bufferCount]);

		if (FAILED(result))
		{
			return false;
		}
	}

	return true;
}
