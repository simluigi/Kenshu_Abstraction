#include "D3D11RenderingData.h"

#include <sstream>
#include "../D3DUtility/D3DCamera.h"
#include "D3D11Texture.h"
#include "../D3DUtility/D3DUtility.h"
#include "D3D11RenderResource.h"
#include "D3D11RenderObject.h"
#include "D3D11CommandBuffer.h"

using namespace Microsoft::WRL;
using namespace D3DUtility;

D3D11RenderingData::D3D11RenderingData(const Window& window) :
	RenderingData(window)
{
}

D3D11RenderingData::~D3D11RenderingData()
{
}

void D3D11RenderingData::Create()
{
	CreateSamplerState();
}

std::shared_ptr<RenderResource> D3D11RenderingData::CreateResource(const char* resourceName, ResourceType resourceType, const void* resourceData, unsigned int dataCount, unsigned int stride, ResourceFormat format)
{
	std::shared_ptr<D3D11RenderResource> renderResource = std::make_shared<D3D11RenderResource>(resourceName, resourceType, resourceData, dataCount, stride, format);
	renderResource->Create(m_device);

	m_renderResources[resourceName] = renderResource;

	return renderResource;
}

void D3D11RenderingData::CreateDepthTexture(const char* resourceName, const Int2& textureSize, float depthClearValue)
{
	if (depthClearValue > 1.0f)
	{
		return;
	}

	ComPtr<ID3D11Texture2D> depthTextureResource;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = textureSize.x;
	textureDesc.Height = textureSize.y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc = { 1, 0 };
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	m_device->CreateTexture2D(&textureDesc, nullptr, &depthTextureResource);

	ComPtr<ID3D11DepthStencilView> depthStencilView;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc = {};
	dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;

	m_device->CreateDepthStencilView(depthTextureResource.Get(), &dsDesc, &depthStencilView);

	ComPtr<ID3D11ShaderResourceView> shaderResourceView;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	m_device->CreateShaderResourceView(depthTextureResource.Get(), &srvDesc, &shaderResourceView);

	m_depthTextureResources.emplace_back(depthTextureResource);
	m_depthStencilView[resourceName] = depthStencilView;
	m_depthTextureSRV[resourceName] = shaderResourceView;
}

std::unique_ptr<Texture> D3D11RenderingData::CreateTexture(const std::vector<uint8_t>& textureData, const Int2& textureSize, unsigned int pixelDataSize)
{
	std::unique_ptr<D3D11Texture> texture = std::make_unique<D3D11Texture>();

	texture->Create(m_device, textureData, textureSize, pixelDataSize);

	return std::move(texture);
}

void D3D11RenderingData::CreateRenderTarget(const char* renderTargetName, const Int2& renderTargetSize, ResourceFormat format)
{
	ComPtr<ID3D11Texture2D> renderTargetResource;

	D3D11_TEXTURE2D_DESC resourceDesc = {};
	resourceDesc.Width = (UINT)renderTargetSize.x;
	resourceDesc.Height = (UINT)renderTargetSize.y;
	resourceDesc.Format = GetD3DFormat(format);
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Usage = D3D11_USAGE_DEFAULT;
	resourceDesc.ArraySize = 1;
	resourceDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	m_device->CreateTexture2D(&resourceDesc, nullptr, &renderTargetResource);

	ComPtr<ID3D11RenderTargetView> renderTargetView;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = GetD3DFormat(format);
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	m_device->CreateRenderTargetView(renderTargetResource.Get(), &rtvDesc, &renderTargetView);

	ComPtr<ID3D11ShaderResourceView> shaderResourceView;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = rtvDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	m_device->CreateShaderResourceView(renderTargetResource.Get(), &srvDesc, &shaderResourceView);

	m_renderTargetResources.emplace_back(renderTargetResource);
	m_renderTargetsRTV[renderTargetName] = renderTargetView;
	m_renderTargetsSRV[renderTargetName] = shaderResourceView;
}

bool D3D11RenderingData::CreateRenderingPipeline(const char* pipelineName, PipelineStateSettings& pipelineData)
{
	RenderingPipelineResource& pipelineResource = m_pipelineResources[pipelineName];

	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;

	if (pipelineData.vertexShader.filePath != nullptr)
	{
		vertexShader = CompileShader(pipelineData.vertexShader.filePath, pipelineData.vertexShader.entryPoint, pipelineData.vertexShader.target);
		m_device->CreateVertexShader(vertexShader->GetBufferPointer(), vertexShader->GetBufferSize(), nullptr, &pipelineResource.vertexShader);
	}

	if (pipelineData.pixelShader.filePath != nullptr)
	{
		pixelShader = CompileShader(pipelineData.pixelShader.filePath, pipelineData.pixelShader.entryPoint, pipelineData.pixelShader.target);
		m_device->CreatePixelShader(pixelShader->GetBufferPointer(), pixelShader->GetBufferSize(), nullptr, &pipelineResource.pixelShader);
	}

	pipelineResource.vertexShaderBlob = vertexShader;

	CreateInputLayout(pipelineData, pipelineResource);

	return true;
}

std::shared_ptr<RenderObject> D3D11RenderingData::CreateRenderObject(const char* objectName, std::vector<MeshVertex>& vertices, std::vector<uint16_t>& indices)
{
	m_renderObjects[objectName] = std::make_shared<D3D11RenderObject>();

	m_renderObjects[objectName]->Create(*this, objectName, vertices, indices);

	return m_renderObjects[objectName];
}

void D3D11RenderingData::SetRenderingPipeline(CommandBuffer& commandBuffer, const char* pipelineName, const Int2& screenSize, float minDepth, float maxDepth)
{
	D3D11CommandBuffer* d3dCommandBuffer = dynamic_cast<D3D11CommandBuffer*>(&commandBuffer);
	assert(d3dCommandBuffer != nullptr);

	ComPtr<ID3D11CommandList> cmdList;

	ID3D11DeviceContext* deviceContext = d3dCommandBuffer->GetDeferredContext();

	RenderingPipelineResource& pipelineData = m_pipelineResources[pipelineName];

	D3D11_VIEWPORT viewport = {};
	viewport.Width = (FLOAT)screenSize.x;
	viewport.Height = (FLOAT)screenSize.y;
	viewport.MaxDepth = maxDepth;
	viewport.MinDepth = minDepth;

	deviceContext->RSSetViewports(1, &viewport);
	deviceContext->IASetInputLayout(pipelineData.inputLayout.Get());

	deviceContext->VSSetShader(pipelineData.vertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(pipelineData.pixelShader.Get(), nullptr, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void D3D11RenderingData::AddCamera(const char* cameraName, std::unique_ptr<Camera> camera)
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

		m_cameraResources[cameraName].emplace_back(std::make_unique<D3D11RenderResource>(frameResourceName.c_str(), ResourceType::ConstantBuffer, &cameraInfo, 1, (unsigned int)sizeof(cameraInfo)));

		m_cameraResources[cameraName][frameResourceIndex]->Create(m_device);
	}
}

std::shared_ptr<RenderObject> D3D11RenderingData::GetRenderObject(const char* objectName)
{
	return m_renderObjects[objectName];
}

D3D11RenderResource& D3D11RenderingData::GetCameraResource(const char* cameraName, unsigned int frameResourceIndex) const
{
	return *m_cameraResources.at(cameraName)[frameResourceIndex];
}

ID3D11RenderTargetView* D3D11RenderingData::GetRenderTargetView(const char* resourceName) const
{
	return m_renderTargetsRTV.at(resourceName).Get();
}

ID3D11ShaderResourceView* D3D11RenderingData::GetShaderResourceView(const char* resourceName) const
{
	return m_renderTargetsSRV.at(resourceName).Get();
}

ID3D11DepthStencilView* D3D11RenderingData::GetDepthStencilView(const char* resourceName) const
{
	return m_depthStencilView.at(resourceName).Get();
}

ID3D11ShaderResourceView* D3D11RenderingData::GetDepthTextureShaderResourceView(const char* resourceName) const
{
	return m_depthTextureSRV.at(resourceName).Get();
}

ID3D11SamplerState* D3D11RenderingData::GetSamplerState() const
{
	return m_samplerState.Get();
}

void D3D11RenderingData::SetD3DDevice(ID3D11Device* device)
{
	m_device = device;
}

void D3D11RenderingData::CreateSamplerState()
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	m_device->CreateSamplerState(&samplerDesc, &m_samplerState);
}

void D3D11RenderingData::CreateInputLayout(PipelineStateSettings& pipelineData, RenderingPipelineResource& pipelineResource)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> elementDesc(pipelineData.inputLayout.size());

	for (int elementIndex = 0; elementIndex < elementDesc.size(); ++elementIndex)
	{
		InputLayout inputLayout = pipelineData.inputLayout[elementIndex];

		DXGI_FORMAT format = GetD3DFormat(inputLayout.format);

		elementDesc[elementIndex] = { inputLayout.semanticName, 0, format, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}

	ID3DBlob* shaderBlob = pipelineResource.vertexShaderBlob.Get();

	m_device->CreateInputLayout(&elementDesc[0], (UINT)elementDesc.size(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), pipelineResource.inputLayout.GetAddressOf());
}
