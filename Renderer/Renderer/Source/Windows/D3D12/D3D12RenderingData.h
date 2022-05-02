#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include <unordered_map>
#include "../../Rendering/RenderingData.h"

class Texture;
class D3D12RenderTarget;
class D3D12CommandBuffer;
class D3D12RenderObject;
class D3D12RenderResource;

class D3D12RenderingData :
    public RenderingData
{
public:

	D3D12RenderingData(const Window& window);
	~D3D12RenderingData();

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

	void AddCamera(const char* cameraName, std::unique_ptr<Camera> camera)override;

	void SetD3DDevice(ID3D12Device* device);

	void SetRenderingPipeline(CommandBuffer& commandBuffer, const char* pipelineName, const Int2& screenSize, float minDepth, float maxDepth)override;

	void SetRTVHeap(D3D12CommandBuffer& commandBuffer);
	void SetRenderTargetSRVHeap(D3D12CommandBuffer& commandBuffer);

	D3D12RenderResource& GetCameraResource(const char* cameraName, unsigned int frameResourceIndex)const;

	/// Render target handle 
	D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetCPUHandle(const char* resourceName, unsigned int frameResourceIndex)const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetRenderTargetGPUHandle(const char* resourceName, unsigned int frameResourceIndex)const;

	/// depth texture handle
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthTextureCPUHandle(const char* resourceName, unsigned int frameResourceIndex)const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetDepthTextureGPUHandle(const char* resourceName, unsigned int frameResourceIndex)const;

	ID3D12Resource* GetRenderTargetResource(const char* resourceName, unsigned int frameResourceIndex)const;
	ID3D12Resource* GetDepthTextureResource(const char* resourceName, unsigned int frameResourceIndex)const;

	std::shared_ptr<RenderObject> GetRenderObject(const char* objectName)override;

	void SetResourceBarrier(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
	void SetRenderTargetResourceBarrier(const char* resourceName, unsigned int frameResourceIndex, ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
	void SetDepthResourceBarrier(const char* resourceName, unsigned int frameResourceIndex, ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

private:

	struct RenderingPipelineResource
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSingature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	};

	void CreateRenderTargetHeap();
	void CreateDepthHeap();

	D3D12_ROOT_PARAMETER_TYPE GetD3D12RootParameterType(RenderingData::ShaderParameterType type);
	D3D12_DESCRIPTOR_RANGE_TYPE GetD3D12DescriptorRangeType(RenderingData::ShaderParameterType type);

	ID3D12Device* m_device = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

	unsigned int m_srvHeapHandleCount = 0; /// Number of GPU handles created

	/// Render target descriptors
	std::unordered_map<std::string, std::vector<std::unique_ptr<D3D12RenderTarget>>> m_renderTargets;
	std::unordered_map<std::string, std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>> m_renderTargetCPUHandles;
	std::unordered_map<std::string, std::vector<D3D12_GPU_DESCRIPTOR_HANDLE>> m_renderTargetGPUHandles;

	/// Depth texture descriptors
	std::unordered_map<std::string, std::vector<std::unique_ptr<D3D12RenderResource>>> m_depthTextureResource;
	std::unordered_map<std::string, std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>> m_depthTextureCPUHandles;
	std::unordered_map<std::string, std::vector<D3D12_GPU_DESCRIPTOR_HANDLE>> m_depthTextureGPUHandles;

	std::unordered_map<std::string, std::shared_ptr<D3D12RenderObject>> m_renderObjects;

	std::unordered_map<std::string, RenderingPipelineResource> m_pipelineResources;

	std::unordered_map<std::string, RootParameterIndex> m_rootParameterIndices;

	std::unordered_map<std::string, std::vector<std::unique_ptr<D3D12RenderResource>>> m_cameraResources;

	std::unordered_map<std::string, std::vector<D3D12_RESOURCE_STATES>> m_currentDepthResourceStates;
	std::unordered_map<std::string, std::vector<D3D12_RESOURCE_STATES>> m_currentRenderTargetResourceStates;
};

