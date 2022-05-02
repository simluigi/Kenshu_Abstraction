#pragma once

#include <wrl/client.h>
#include <d3dcompiler.h>
#include <dxgiformat.h>
#include "../../Rendering/RenderCommon.h"

namespace D3DUtility
{
	DXGI_FORMAT GetD3DFormat(ResourceFormat format);

	Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const wchar_t* filePath, const char* entryPoint, const char* target);
}

