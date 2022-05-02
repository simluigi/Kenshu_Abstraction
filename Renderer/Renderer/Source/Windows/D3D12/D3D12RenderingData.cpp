#include "D3D12RenderingData.h"

#include <cassert>
#include <sstream>
#include "D3D12CommandBuffer.h"
#include "D3D12RenderObject.h"
#include "D3D12RenderResource.h"
#include "D3D12Texture.h"
#include "D3D12RenderTarget.h"
#include "../D3DUtility/D3DUtility.h"
#include "../D3DUtility/D3DCamera.h"

using namespace Microsoft::WRL;
using namespace D3DUtility;

constexpr unsigned int SWAPCHAIN_BUFFER_COUNT = 2;

D3D12RenderingData::D3D12RenderingData(const Window& window) :
	RenderingData(window)
{
}

D3D12RenderingData::~D3D12RenderingData()
{
}

void D3D12RenderingData::Create()
{
	CreateRenderTargetHeap();
	CreateDepthHeap();
}

std::shared_ptr<RenderResource> D3D12RenderingData::CreateResource(const char* resourceName, ResourceType resourceType, const void* resourceData, unsigned int dataCount, unsigned int stride, ResourceFormat format)
{
    std::shared_ptr<D3D12RenderResource> renderResource = std::make_shared<D3D12RenderResource>(resourceName, resourceType, resourceData, dataCount, stride, format);
    renderResource->Create(m_device);

    m_renderResources[resourceName] = renderResource;

    return renderResource;
}

void D3D12RenderingData::CreateDepthTexture(const char* resourceName, const Int2& textureSize, float depthClearValue)
{
	m_depthTextureResource[resourceName].resize(SWAPCHAIN_BUFFER_COUNT);
	m_depthTextureCPUHandles[resourceName].resize(SWAPCHAIN_BUFFER_COUNT);
	m_depthTextureGPUHandles[resourceName].resize(SWAPCHAIN_BUFFER_COUNT);

	for (int bufferIndex = 0; bufferIndex < SWAPCHAIN_BUFFER_COUNT; ++bufferIndex)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
		cpuHandle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV) * (m_depthTextureResource.size() * SWAPCHAIN_BUFFER_COUNT + bufferIndex);

		D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle = m_srvHeap->GetCPUDescriptorHandleForHeapStart();
		srvCpuHandle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * m_srvHeapHandleCount;

		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_srvHeap->GetGPUDescriptorHandleForHeapStart();
		gpuHandle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * m_srvHeapHandleCount;

		m_depthTextureResource[resourceName][bufferIndex] = std::make_unique<D3D12RenderResource>();
		m_depthTextureResource[resourceName][bufferIndex]->CreateDepthTexture(m_device, cpuHandle, textureSize, depthClearValue);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		m_device->CreateShaderResourceView(m_depthTextureResource[resourceName][bufferIndex]->GetBuffer(), &srvDesc, srvCpuHandle);

		m_depthTextureCPUHandles[resourceName][bufferIndex] = cpuHandle;
		m_depthTextureGPUHandles[resourceName][bufferIndex] = gpuHandle;

		++m_srvHeapHandleCount;
	}
}

std::unique_ptr<Texture> D3D12RenderingData::CreateTexture(const std::vector<uint8_t>& textureData, const Int2& textureSize, unsigned int pixelDataSize)
{
	std::unique_ptr<D3D12Texture> texture = std::make_unique<D3D12Texture>();

	texture->Create(m_device, textureData, textureSize, pixelDataSize);

	return std::move(texture);
}

void D3D12RenderingData::CreateRenderTarget(const char* renderTargetName, const Int2& renderTargetSize, ResourceFormat format)
{
	m_renderTargets[renderTargetName].resize(SWAPCHAIN_BUFFER_COUNT);
	m_renderTargetCPUHandles[renderTargetName].resize(SWAPCHAIN_BUFFER_COUNT);
	m_renderTargetGPUHandles[renderTargetName].resize(SWAPCHAIN_BUFFER_COUNT);

	for (int bufferIndex = 0; bufferIndex < SWAPCHAIN_BUFFER_COUNT; ++bufferIndex)
	{
		std::unique_ptr<D3D12RenderTarget> renderTarget = std::make_unique<D3D12RenderTarget>(renderTargetName, renderTargetSize, format);

		renderTarget->Create(m_device);

		D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvCpuHandle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * (m_renderTargets.size() * SWAPCHAIN_BUFFER_COUNT + bufferIndex);

		D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle = m_srvHeap->GetCPUDescriptorHandleForHeapStart();
		srvCpuHandle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * m_srvHeapHandleCount;

		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_srvHeap->GetGPUDescriptorHandleForHeapStart();
		gpuHandle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * m_srvHeapHandleCount;

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = GetD3DFormat(format);
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		m_device->CreateRenderTargetView(renderTarget->GetResource(), &rtvDesc, rtvCpuHandle);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = GetD3DFormat(format);
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		m_device->CreateShaderResourceView(renderTarget->GetResource(), &srvDesc, srvCpuHandle);

		m_renderTargetCPUHandles[renderTargetName][bufferIndex] = rtvCpuHandle;
		m_renderTargetGPUHandles[renderTargetName][bufferIndex] = gpuHandle;

		m_renderTargets[renderTargetName][bufferIndex] = std::move(renderTarget);

		++m_srvHeapHandleCount;
	}
}

bool D3D12RenderingData::CreateRenderingPipeline(const char* pipelineName, PipelineStateSettings& pipelineData)
{
	HRESULT result = S_OK;

	RootParameterIndex& rootParamindex = m_rootParameterIndices[pipelineName];
	RenderingPipelineResource& pipelineResource = m_pipelineResources[pipelineName] = RenderingPipelineResource();

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.NumParameters = (UINT)pipelineData.shaderParameter.size();

	std::vector<D3D12_ROOT_PARAMETER> rootParams(pipelineData.shaderParameter.size());
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges;

	unsigned int useDescriptorCount = 0;

	for (size_t parameterIndex = 0; parameterIndex < pipelineData.shaderParameter.size(); ++parameterIndex)
	{
		ShaderParameter& shaderParam = pipelineData.shaderParameter[parameterIndex];

		if (shaderParam.useDescriptorTable)
		{
			++useDescriptorCount;
		}
	}

	descriptorRanges.reserve(useDescriptorCount);

	for (size_t parameterIndex = 0; parameterIndex < pipelineData.shaderParameter.size(); ++parameterIndex)
	{
		ShaderParameter& shaderParam = pipelineData.shaderParameter[parameterIndex];
		D3D12_ROOT_PARAMETER& rootParam = rootParams[parameterIndex];

		if (shaderParam.useDescriptorTable)
		{
			D3D12_DESCRIPTOR_RANGE& range = descriptorRanges.emplace_back(D3D12_DESCRIPTOR_RANGE());
			range.BaseShaderRegister = (UINT)shaderParam.registerNum;
			range.NumDescriptors = 1;
			range.RangeType = GetD3D12DescriptorRangeType(shaderParam.type);

			rootParam.DescriptorTable.pDescriptorRanges = &range;
			rootParam.DescriptorTable.NumDescriptorRanges = 1;
		}
		else
		{
			rootParam.ParameterType = GetD3D12RootParameterType(shaderParam.type);
			rootParam.Constants.ShaderRegister = (UINT)shaderParam.registerNum;
		}

		rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		switch (shaderParam.type)
		{
		case RenderingData::ShaderParameterType::SRV:

			rootParamindex.srvIndex.emplace_back((unsigned int)parameterIndex);
			break;

		case RenderingData::ShaderParameterType::CBV:

			rootParamindex.cbvIndex.emplace_back((unsigned int)parameterIndex);
			break;

		case RenderingData::ShaderParameterType::UAV:

			rootParamindex.uavIndex.emplace_back((unsigned int)parameterIndex);
			break;
		}
	}

	rootSignatureDesc.pParameters = rootParams.size() == 0 ? nullptr : &rootParams[0];

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = FLT_MAX;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	rootSignatureDesc.NumStaticSamplers = 1;
	rootSignatureDesc.pStaticSamplers = &samplerDesc;

	ID3DBlob* signatureBlob = nullptr;

	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, nullptr);

	if (FAILED(result))
	{
		return false;
	}

	result = m_device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&pipelineResource.rootSingature));

	if (FAILED(result))
	{
		return false;
	}

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc(pipelineData.inputLayout.size());

	for (size_t inputLayoutIndex = 0; inputLayoutIndex < pipelineData.inputLayout.size(); ++inputLayoutIndex)
	{
		InputLayout& inputLayout = pipelineData.inputLayout[inputLayoutIndex];
		D3D12_INPUT_ELEMENT_DESC& elementDesc = inputElementDesc[inputLayoutIndex];
		elementDesc.SemanticName = inputLayout.semanticName;
		elementDesc.Format = GetD3DFormat(inputLayout.format);
		elementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;

	if (pipelineData.vertexShader.filePath != nullptr)
	{
		vertexShader = CompileShader(pipelineData.vertexShader.filePath, pipelineData.vertexShader.entryPoint, pipelineData.vertexShader.target);

		psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
	}

	if (pipelineData.pixelShader.filePath != nullptr)
	{
		pixelShader = CompileShader(pipelineData.pixelShader.filePath, pipelineData.pixelShader.entryPoint, pipelineData.pixelShader.target);

		psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
	}

	psoDesc.pRootSignature = pipelineResource.rootSingature.Get();
	psoDesc.InputLayout.pInputElementDescs = &inputElementDesc[0];
	psoDesc.InputLayout.NumElements = (UINT)inputElementDesc.size();
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.NumRenderTargets = (UINT)pipelineData.renderTargetResourceFormat.size();

	for (size_t renderTargetCount = 0; renderTargetCount < pipelineData.renderTargetResourceFormat.size(); ++renderTargetCount)
	{
		psoDesc.RTVFormats[renderTargetCount] = GetD3DFormat(pipelineData.renderTargetResourceFormat[renderTargetCount]);
	}

	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.DepthClipEnable = true;
	psoDesc.DepthStencilState.DepthEnable = true;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	for (UINT renderTargetIndex = 0; renderTargetIndex < psoDesc.NumRenderTargets; ++renderTargetIndex)
	{
		psoDesc.BlendState.RenderTarget[renderTargetIndex].BlendEnable = false;
		psoDesc.BlendState.RenderTarget[renderTargetIndex].SrcBlend = D3D12_BLEND_ONE;
		psoDesc.BlendState.RenderTarget[renderTargetIndex].DestBlend = D3D12_BLEND_ZERO;
		psoDesc.BlendState.RenderTarget[renderTargetIndex].BlendOp = D3D12_BLEND_OP_ADD;
		psoDesc.BlendState.RenderTarget[renderTargetIndex].SrcBlendAlpha = D3D12_BLEND_ONE;
		psoDesc.BlendState.RenderTarget[renderTargetIndex].DestBlendAlpha = D3D12_BLEND_ZERO;
		psoDesc.BlendState.RenderTarget[renderTargetIndex].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		psoDesc.BlendState.RenderTarget[renderTargetIndex].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}

	m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineResource.pipelineState));

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void D3D12RenderingData::SetRenderingPipeline(CommandBuffer& commandBuffer, const char* pipelineName, const Int2& screenSize, float minDepth, float maxDepth)
{
	D3D12CommandBuffer* d3dCommandBuffer= dynamic_cast<D3D12CommandBuffer*>(&commandBuffer);
	assert(d3dCommandBuffer != nullptr);

	ID3D12GraphicsCommandList6* cmdList = d3dCommandBuffer->GetCommandList();

	D3D12_VIEWPORT viewport = {};
	viewport.Width = (FLOAT)screenSize.x;
	viewport.Height = (FLOAT)screenSize.y;
	viewport.MinDepth = minDepth;
	viewport.MaxDepth = maxDepth;

	D3D12_RECT scissorRect = {};
	scissorRect.right = screenSize.x;
	scissorRect.bottom = screenSize.y;

	cmdList->RSSetViewports(1, &viewport);
	cmdList->RSSetScissorRects(1, &scissorRect);

	const RenderingPipelineResource& pipelineResource = m_pipelineResources.at(pipelineName);

	cmdList->SetGraphicsRootSignature(pipelineResource.rootSingature.Get());
	cmdList->SetPipelineState(pipelineResource.pipelineState.Get());

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	d3dCommandBuffer->SetCurrentPipelineRootParamIndex(&m_rootParameterIndices.at(pipelineName));
}

void D3D12RenderingData::SetRTVHeap(D3D12CommandBuffer& commandBuffer)
{
	ID3D12GraphicsCommandList6* cmdList = commandBuffer.GetCommandList();

	cmdList->SetDescriptorHeaps(1, m_rtvHeap.GetAddressOf());
}

void D3D12RenderingData::SetRenderTargetSRVHeap(D3D12CommandBuffer& commandBuffer)
{
	ID3D12GraphicsCommandList6* cmdList = commandBuffer.GetCommandList();

	cmdList->SetDescriptorHeaps(1, m_srvHeap.GetAddressOf());
}

std::shared_ptr<RenderObject> D3D12RenderingData::CreateRenderObject(const char* objectName, std::vector<MeshVertex>& vertices, std::vector<uint16_t>& indices)
{
    m_renderObjects[objectName] = std::make_shared<D3D12RenderObject>();

    m_renderObjects[objectName]->Create(*this, objectName, vertices, indices);

    return m_renderObjects[objectName];
}

void D3D12RenderingData::AddCamera(const char* cameraName, std::unique_ptr<Camera> camera)
{
	m_cameras[cameraName] = std::move(camera);

	D3DCamera* d3dCamera = dynamic_cast<D3DCamera*>(&*m_cameras[cameraName]);
	assert(d3dCamera != nullptr);

	/// Create camera resources
	const D3DCamera::Info& cameraInfo = d3dCamera->GetCameraInfo();
	std::string cameraResourceName = cameraName;
	cameraResourceName += "Resource";

	constexpr unsigned int FRAME_RESOURCE_COUNT = 2;

	for (unsigned int frameResourceIndex = 0; frameResourceIndex < FRAME_RESOURCE_COUNT; ++frameResourceIndex)
	{
		std::stringstream frameResourceIndexName;
		frameResourceIndexName << frameResourceIndex;

		std::string frameResourceName = cameraResourceName;
		frameResourceName += frameResourceIndexName.str();

		m_cameraResources[cameraName].emplace_back(std::make_unique<D3D12RenderResource>(frameResourceName.c_str(), ResourceType::ConstantBuffer, &cameraInfo, 1, (unsigned int)sizeof(cameraInfo)));

		m_cameraResources[cameraName][frameResourceIndex]->Create(m_device);
	}
}

void D3D12RenderingData::SetD3DDevice(ID3D12Device* device)
{
    m_device = device;
}

D3D12RenderResource& D3D12RenderingData::GetCameraResource(const char* cameraName, unsigned int frameResourceIndex) const
{
	return *m_cameraResources.at(cameraName)[frameResourceIndex];
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderingData::GetRenderTargetCPUHandle(const char* resourceName, unsigned int frameResourceIndex) const
{
	return m_renderTargetCPUHandles.at(resourceName)[frameResourceIndex];
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12RenderingData::GetRenderTargetGPUHandle(const char* resourceName, unsigned int frameResourceIndex) const
{
	return m_renderTargetGPUHandles.at(resourceName)[frameResourceIndex];
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderingData::GetDepthTextureCPUHandle(const char* resourceName, unsigned int frameResourceIndex) const
{
	return m_depthTextureCPUHandles.at(resourceName)[frameResourceIndex];
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12RenderingData::GetDepthTextureGPUHandle(const char* resourceName, unsigned int frameResourceIndex) const
{
	return m_depthTextureGPUHandles.at(resourceName)[frameResourceIndex];
}

ID3D12Resource* D3D12RenderingData::GetRenderTargetResource(const char* resourceName, unsigned int frameResourceIndex) const
{
	return m_renderTargets.at(resourceName)[frameResourceIndex]->GetResource();
}

ID3D12Resource* D3D12RenderingData::GetDepthTextureResource(const char* resourceName, unsigned int frameResourceIndex) const
{
	return m_depthTextureResource.at(resourceName)[frameResourceIndex]->GetBuffer();
}

std::shared_ptr<RenderObject> D3D12RenderingData::GetRenderObject(const char* objectName)
{
	return m_renderObjects.at(objectName);
}

void D3D12RenderingData::SetResourceBarrier(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
	D3D12_RESOURCE_BARRIER desc = {};
	desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	desc.Transition.StateBefore = before;
	desc.Transition.StateAfter = after;
	desc.Transition.pResource = resource;
	desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	cmdList->ResourceBarrier(1, &desc);
}

void D3D12RenderingData::SetRenderTargetResourceBarrier(const char* resourceName, unsigned int frameResourceIndex, ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
	if (m_currentRenderTargetResourceStates[resourceName].size() <= frameResourceIndex)
	{
		m_currentRenderTargetResourceStates[resourceName].emplace_back(D3D12_RESOURCE_STATE_RENDER_TARGET);

		return;
	}

	if (m_currentRenderTargetResourceStates[resourceName][frameResourceIndex] == after)
	{
		return;
	}

	SetResourceBarrier(cmdList, m_renderTargets[resourceName][frameResourceIndex]->GetResource(), before, after);

	m_currentRenderTargetResourceStates[resourceName][frameResourceIndex] = after;
}

void D3D12RenderingData::SetDepthResourceBarrier(const char* resourceName, unsigned int frameResourceIndex, ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
	if (m_currentDepthResourceStates[resourceName].size() <= frameResourceIndex)
	{
		m_currentDepthResourceStates[resourceName].emplace_back(D3D12_RESOURCE_STATE_DEPTH_WRITE);

		return;
	}

	if (m_currentDepthResourceStates[resourceName][frameResourceIndex] == after)
	{
		return;
	}

	SetResourceBarrier(cmdList, m_depthTextureResource[resourceName][frameResourceIndex]->GetBuffer(), before, after);

	m_currentDepthResourceStates[resourceName][frameResourceIndex] = after;
}

void D3D12RenderingData::CreateRenderTargetHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = 10;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 10;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	m_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap));
}

void D3D12RenderingData::CreateDepthHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 10;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap));
}

D3D12_ROOT_PARAMETER_TYPE D3D12RenderingData::GetD3D12RootParameterType(RenderingData::ShaderParameterType type)
{
	D3D12_ROOT_PARAMETER_TYPE rangeType = {};

	switch (type)
	{
	case RenderingData::ShaderParameterType::SRV:

		rangeType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		break;

	case RenderingData::ShaderParameterType::CBV:

		rangeType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		break;

	case RenderingData::ShaderParameterType::UAV:

		rangeType = D3D12_ROOT_PARAMETER_TYPE_UAV;
		break;

	default:
		break;
	}

	return rangeType;
}

D3D12_DESCRIPTOR_RANGE_TYPE D3D12RenderingData::GetD3D12DescriptorRangeType(RenderingData::ShaderParameterType type)
{
	D3D12_DESCRIPTOR_RANGE_TYPE rangeType = {};

	switch (type)
	{
	case RenderingData::ShaderParameterType::SRV:

		rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		break;

	case RenderingData::ShaderParameterType::CBV:

		rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		break;

	case RenderingData::ShaderParameterType::UAV:

		rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		break;

	default:
		break;
	}

	return rangeType;
}
