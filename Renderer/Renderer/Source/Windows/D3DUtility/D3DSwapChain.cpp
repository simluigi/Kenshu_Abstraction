#include "D3DSwapChain.h"

D3DSwapChain::D3DSwapChain()
{
}

D3DSwapChain::~D3DSwapChain()
{
}

bool D3DSwapChain::Create(IUnknown* device, IDXGIFactory2* factory, HWND windowHandle, const Int2& windowSize, DXGI_FORMAT format, unsigned int buferCount)
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = buferCount;
	swapChainDesc.Width = windowSize.x;
	swapChainDesc.Height = windowSize.y;
	swapChainDesc.Format = format;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

	HRESULT result = factory->CreateSwapChainForHwnd(device, windowHandle, &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1**)m_swapChain.GetAddressOf());

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void D3DSwapChain::Present(UINT vsync)
{
	m_swapChain->Present(vsync, 0);
}

IDXGISwapChain4* D3DSwapChain::Get() const
{
	return m_swapChain.Get();
}
