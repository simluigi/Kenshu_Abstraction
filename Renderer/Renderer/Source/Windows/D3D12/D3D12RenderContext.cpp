#include "D3D12RenderContext.h"

#include <cassert>
#include "../Window.h"
#include "D3D12CommandBuffer.h"
#include "../D3DUtility/D3DSwapChain.h"
#include "D3D12RenderResource.h"
#include "D3D12RenderingData.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

constexpr unsigned int SWAPCHAIN_BUFFER_COUNT = 2;

D3D12RenderContext::D3D12RenderContext(const Window& window) :
	m_window(window)
{
}

D3D12RenderContext::~D3D12RenderContext()
{
}

bool D3D12RenderContext::Create()
{
	if (!CreateDevice())
	{
		return false;
	}

	if (!CreateCommandQueue())
	{
		return false;
	}

	if (!CreateDepthBuffer())
	{
		return false;
	}

	if (!CreateSwapChain())
	{
		return false;
	}

	m_mainCommandBuffer = std::make_unique<D3D12CommandBuffer>();

	if (!m_mainCommandBuffer->Create(*this))
	{
		return false;
	}

	return true;
}

void D3D12RenderContext::SetSwapChainRenderTarget(CommandBuffer& commandBuffer, unsigned int backBufferIndex, float* clearColor, bool useDepth)
{
	D3D12CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D12CommandBuffer*>(&commandBuffer);
	assert(d3dCommandBuffer != nullptr);

	ID3D12GraphicsCommandList6* cmdList = d3dCommandBuffer->GetCommandList();

	UINT renderTargetIndex = (UINT)backBufferIndex;

	SetResourceBarrier(cmdList, m_renderTargets[renderTargetIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	cmdList->ClearRenderTargetView(m_renderTargetHandles[renderTargetIndex], clearColor, 0, nullptr);

	if (useDepth)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
		dsvHandle.ptr += renderTargetIndex * m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		cmdList->OMSetRenderTargets(1, &m_renderTargetHandles[renderTargetIndex], true, &dsvHandle);
	}
	else
	{
		cmdList->OMSetRenderTargets(1, &m_renderTargetHandles[renderTargetIndex], true, nullptr);
	}
}

void D3D12RenderContext::SetRenderTarget(CommandBuffer& commandBuffer, RenderingData& renderingData, const char* renderTargetName, float* clearColor, const char* depthTextureName)
{
	D3D12CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D12CommandBuffer*>(&commandBuffer);
	assert(d3dCommandBuffer != nullptr);

	D3D12RenderingData* d3dRenderingData = dynamic_cast<D3D12RenderingData*>(&renderingData);
	assert(d3dRenderingData != nullptr);

	ID3D12GraphicsCommandList6* cmdList = d3dCommandBuffer->GetCommandList();

	if (renderTargetName == nullptr)
	{
		if (depthTextureName != nullptr)
		{
			unsigned int resourceIndex = d3dCommandBuffer->GetFrameResourceIndex();

			d3dRenderingData->SetDepthResourceBarrier(depthTextureName, resourceIndex, cmdList,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);

			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = d3dRenderingData->GetDepthTextureCPUHandle(depthTextureName, resourceIndex);
			cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
			cmdList->OMSetRenderTargets(0, nullptr, true, &dsvHandle);
		}
	}
	else
	{
		unsigned int resourceIndex = d3dCommandBuffer->GetFrameResourceIndex();

		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandle = d3dRenderingData->GetRenderTargetCPUHandle(renderTargetName, resourceIndex);

		d3dRenderingData->SetRenderTargetResourceBarrier(renderTargetName, resourceIndex, cmdList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

		cmdList->ClearRenderTargetView(renderTargetHandle, clearColor, 0, nullptr);

		if (depthTextureName != nullptr)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
			cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
			cmdList->OMSetRenderTargets(1, &renderTargetHandle, true, &dsvHandle);
		}
		else
		{
			cmdList->OMSetRenderTargets(1, &renderTargetHandle, true, nullptr);
		}
	}
}

void D3D12RenderContext::ExecuteMainCommandBuffer()
{
	D3D12CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D12CommandBuffer*>(&GetMainCommandBuffer());
	assert(d3dCommandBuffer != nullptr);

	ID3D12GraphicsCommandList6* cmdList = d3dCommandBuffer->GetCommandList();

	cmdList->Close();

	ID3D12CommandList* commandList = cmdList;
	m_commandQueue->ExecuteCommandLists(1, &commandList);
}

void D3D12RenderContext::ExecuteCommandBuffers(CommandBuffer* commandBuffers, unsigned int bufferCount)
{
	std::vector<ID3D12CommandList*> commandLists;
	commandLists.reserve(bufferCount);

	for (unsigned int commandBufferIndex = 0; commandBufferIndex < bufferCount; ++commandBufferIndex)
	{
		D3D12CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D12CommandBuffer*>(&commandBuffers[commandBufferIndex]);
		assert(d3dCommandBuffer != nullptr);

		ID3D12GraphicsCommandList6* cmdList = d3dCommandBuffer->GetCommandList();

		cmdList->Close();

		commandLists.emplace_back(cmdList);
	}

	m_commandQueue->ExecuteCommandLists((UINT)commandLists.size(), commandLists.data());
}

void D3D12RenderContext::EndFrameRendering(CommandBuffer& commandBuffer, unsigned int backBufferIndex)
{
	D3D12CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D12CommandBuffer*>(&commandBuffer);
	assert(d3dCommandBuffer != nullptr);

	UINT renderTargetIndex = (UINT)backBufferIndex;

	ID3D12GraphicsCommandList6* cmdList = d3dCommandBuffer->GetCommandList();

	SetResourceBarrier(cmdList, m_renderTargets[renderTargetIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}

std::shared_ptr<CommandBuffer> D3D12RenderContext::CreateCommandBuffer(const char* bufferName)
{
	std::shared_ptr<D3D12CommandBuffer> commandBuffer = std::make_shared<D3D12CommandBuffer>();

	commandBuffer->Create(*this);

	m_commandBuffers[bufferName] = commandBuffer;

	return m_commandBuffers[bufferName];
}

void D3D12RenderContext::WaitForCommandExecute()
{
	m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);

	m_commandQueue->Signal(m_fence.Get(), m_fenceValue);
	WaitForSingleObject(m_fenceEvent, INFINITE);

	++m_fenceValue;
}

void D3D12RenderContext::SwapChainPresent(int vsync)
{
	m_swapChain->Present(vsync);
}

ID3D12Device* D3D12RenderContext::GetDevice() const
{
	return m_device.Get();
}

unsigned int D3D12RenderContext::GetSwapChainRenderTargetIndex() const
{
	return (unsigned int)m_swapChain->Get()->GetCurrentBackBufferIndex();
}

bool D3D12RenderContext::CreateDevice()
{
	HRESULT result = S_OK;

	UINT debugFlag = 0;

#if _DEBUG

	ID3D12Debug* debug = nullptr;

	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));

	if (FAILED(result))
	{
		return false;
	}

	debug->EnableDebugLayer();
	debug->Release();

	debugFlag |= DXGI_CREATE_FACTORY_DEBUG;

#endif // _DEBUG

	result = CreateDXGIFactory2(debugFlag, IID_PPV_ARGS(&m_dxgiFactory));

	if (FAILED(result))
	{
		return false;
	}

	IDXGIAdapter* adapter = nullptr;

	result = m_dxgiFactory->EnumAdapters(0, &adapter);

	if (FAILED(result))
	{
		return false;
	}

	result = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));

	if (FAILED(result))
	{
		return false;
	}

	adapter->Release();


	return true;
}

bool D3D12RenderContext::CreateSwapChain()
{
	m_swapChain = std::make_unique<D3DSwapChain>();

	if (!m_swapChain->Create(m_commandQueue.Get(), m_dxgiFactory.Get(),
		m_window.GetWindowHandle(), m_window.GetWindowSize(),
		DXGI_FORMAT_R8G8B8A8_UNORM, SWAPCHAIN_BUFFER_COUNT))
	{
		return false;
	}

	HRESULT result = S_OK;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = SWAPCHAIN_BUFFER_COUNT;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	result = m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap));

	if (FAILED(result))
	{
		return false;
	}

	m_renderTargets.resize(SWAPCHAIN_BUFFER_COUNT);
	m_renderTargetHandles.resize(SWAPCHAIN_BUFFER_COUNT);

	UINT handleStride = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	IDXGISwapChain4* swapChain = m_swapChain->Get();

	for (UINT rtvIndex = 0; rtvIndex < SWAPCHAIN_BUFFER_COUNT; ++rtvIndex)
	{
		result = swapChain->GetBuffer(rtvIndex, IID_PPV_ARGS(&m_renderTargets[rtvIndex]));

		if (FAILED(result))
		{
			return false;
		}

		m_renderTargetHandles[rtvIndex] = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
		m_renderTargetHandles[rtvIndex].ptr += rtvIndex * handleStride;

		m_device->CreateRenderTargetView(m_renderTargets[rtvIndex].Get(), nullptr, m_renderTargetHandles[rtvIndex]);
	}

	return true;
}

bool D3D12RenderContext::CreateCommandQueue()
{
	HRESULT result = S_OK;

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	result = m_device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_commandQueue));

	if (FAILED(result))
	{
		return false;
	}

	m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);

	result = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool D3D12RenderContext::CreateDepthBuffer()
{
	Int2 windowSize = m_window.GetWindowSize();

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 2;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	HRESULT result = m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap));

	if (FAILED(result))
	{
		return false;
	}

	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = windowSize.x;
	resourceDesc.Height = windowSize.y;
	resourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.SampleDesc.Count = 1;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;

	m_depthBuffer.resize(SWAPCHAIN_BUFFER_COUNT);

	for (unsigned int bufferCount = 0; bufferCount < SWAPCHAIN_BUFFER_COUNT; ++bufferCount)
	{
		result = m_device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&m_depthBuffer[bufferCount]));

		if (FAILED(result))
		{
			return false;
		}

		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
		cpuHandle.ptr += bufferCount * m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		m_device->CreateDepthStencilView(m_depthBuffer[bufferCount].Get(), &depthStencilViewDesc, cpuHandle);
	}

	return true;
}

void D3D12RenderContext::SetResourceBarrier(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
	D3D12_RESOURCE_BARRIER desc = {};
	desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	desc.Transition.StateBefore = before;
	desc.Transition.StateAfter = after;
	desc.Transition.pResource = resource;
	desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	cmdList->ResourceBarrier(1, &desc);
}