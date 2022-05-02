#include "D3DUtility.h"

#include <cassert>
#include "../../Rendering/RenderingData.h"

#pragma comment(lib, "d3dcompiler.lib")

DXGI_FORMAT D3DUtility::GetD3DFormat(ResourceFormat format)
{
	DXGI_FORMAT dxgiFormat = {};

	switch (format)
	{
	case ResourceFormat::UNKNOWN:

		dxgiFormat = DXGI_FORMAT_UNKNOWN;
		break;

	case ResourceFormat::R16_UINT:

		dxgiFormat = DXGI_FORMAT_R16_UINT;
		break;

	case ResourceFormat::R32_UINT:

		dxgiFormat = DXGI_FORMAT_R32_UINT;
		break;

	case ResourceFormat::R8G8B8A8_UNORM:

		dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;

	case ResourceFormat::R32G32_FLOAT:

		dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
		break;

	case ResourceFormat::R32G32B32_FLOAT:

		dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		break;

	case ResourceFormat::R32G32B32A32_FLOAT:

		dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;

	default:

		assert(false);
		break;
	}

	return dxgiFormat;
}

Microsoft::WRL::ComPtr<ID3DBlob> D3DUtility::CompileShader(const wchar_t* filePath, const char* entryPoint, const char* target)
{
#ifdef _DEBUG

	UINT compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

#else

	UINT compileFlag = 0;

#endif // _DEBUG

	HRESULT result = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;

	result = D3DCompileFromFile(filePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, target, compileFlag, 0, shaderBlob.GetAddressOf(), nullptr);

	return shaderBlob;
}
