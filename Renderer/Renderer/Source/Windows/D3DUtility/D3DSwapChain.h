#pragma once

#include <wrl/client.h>
#include <dxgi1_6.h>
#include "../../Utility/Math/Vector.h"

class D3DSwapChain
{
public:

	D3DSwapChain();
	~D3DSwapChain();

	bool Create(IUnknown* device, IDXGIFactory2* factory,
				HWND windowHandle, const Int2& windowSize,
				DXGI_FORMAT format, unsigned int buferCount);

	void Present(UINT vsync);

	IDXGISwapChain4* Get()const;

private:

	Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;

};

