#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <unordered_map>
#include "../../Rendering/RenderingData.h"

class D3D11RenderObject;
class D3D11RenderResource;

class D3D11RenderingData :
	public RenderingData
{
public:

	D3D11RenderingData(const Window& window);
	~D3D11RenderingData();

	void Create()override;

	/**
	 * @brief Create render resource
	*/
	std::shared_ptr<RenderResource> CreateResource(const char* resourceName, ResourceType resourceType,
		const void* resourceData, unsigned int dataCount, unsigned int stride,
		ResourceFormat format = ResourceFormat::UNKNOWN)override;

	/**
	 * @brief Create depth texture
	*/
	void CreateDepthTexture(const char* resourceName, const Int2& textureSize, float depthClearValue)override;

	/**
	 * @brief Create texture resource
	 * @param textureData Texture source
	 * @param textureSize Texture size
	 * @param pixelDataSize 1pixel data size
	*/
	std::unique_ptr<Texture> CreateTexture(const std::vector<uint8_t>& textureData, const Int2& textureSize, unsigned int pixelDataSize)override;

	void CreateRenderTarget(const char* renderTargetName, const Int2& renderTargetSize, ResourceFormat format)override;

	/**
	 * @brief Create rendering pipeline
	 * @param pipelineName Name to use when setting
	 * @param pipelineData pipeline settings
	*/
	bool CreateRenderingPipeline(const char* pipelineName, PipelineStateSettings& pipelineData)override;

	std::shared_ptr<RenderObject> CreateRenderObject(const char* objectName, std::vector<MeshVertex>& vertices, std::vector<uint16_t>& indices)override;

	void SetRenderingPipeline(CommandBuffer& commandBuffer, const char* pipelineName, const Int2& screenSize, float minDepth, float maxDepth)override;

	void SetD3DDevice(ID3D11Device* device);

	void AddCamera(const char* cameraName, std::unique_ptr<Camera> camera)override;

	std::shared_ptr<RenderObject> GetRenderObject(const char* objectName)override;

	D3D11RenderResource& GetCameraResource(const char* cameraName, unsigned int frameResourceIndex)const;

	ID3D11RenderTargetView* GetRenderTargetView(const char* resourceName)const;
	ID3D11ShaderResourceView* GetShaderResourceView(const char* resourceName)const;
	ID3D11DepthStencilView* GetDepthStencilView(const char* resourceName)const;
	ID3D11ShaderResourceView* GetDepthTextureShaderResourceView(const char* resourceName)const;

	ID3D11SamplerState* GetSamplerState()const;

private:

	struct RenderingPipelineResource
	{
		Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	};

	void CreateSamplerState();
	void CreateInputLayout(PipelineStateSettings& pipelineData, RenderingPipelineResource& pipelineResource);

	ID3D11Device* m_device = nullptr;

	std::unordered_map<std::string, RenderingPipelineResource> m_pipelineResources;

	std::unordered_map<std::string, std::shared_ptr<D3D11RenderObject>> m_renderObjects;

	std::unordered_map<std::string, std::vector<std::unique_ptr<D3D11RenderResource>>> m_cameraResources;

	std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> m_renderTargetResources;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_renderTargetsRTV;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_renderTargetsSRV;

	std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> m_depthTextureResources;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11DepthStencilView>> m_depthStencilView;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_depthTextureSRV;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
};
