#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "RenderCommon.h"

class Camera;
class Window;
class Texture;
class RenderContext;
class CommandBuffer;
class RenderResource;
class RenderObject;
class RenderTarget;

class RenderingData
{
public:

	struct ShaderFile
	{
		const wchar_t* filePath;
		const char* entryPoint;
		const char* target;
	};

	struct InputLayout
	{
		const char* semanticName;
		ResourceFormat format;
	};

	enum class ShaderParameterType
	{
		SRV,
		CBV,
		UAV,
	};

	struct ShaderParameter
	{
		ShaderParameterType type = {};
		unsigned int registerNum = 0;
		bool useDescriptorTable = false;
	};

	struct PipelineStateSettings
	{
		ShaderFile vertexShader;
		ShaderFile pixelShader;
		std::vector<InputLayout> inputLayout;
		std::vector<ShaderParameter> shaderParameter;
		std::vector<ResourceFormat> renderTargetResourceFormat;
	};

public:

	RenderingData(const Window& window);
	virtual ~RenderingData();

	virtual void Create() = 0;

	void UpdateCamera();

	/**
	 * @brief Create render resource
	*/
	virtual std::shared_ptr<RenderResource> CreateResource(const char* resourceName, ResourceType resourceType,
														   const void* resourceData, unsigned int dataCount, unsigned int stride,
														   ResourceFormat format = ResourceFormat::UNKNOWN) = 0;
	/**
	 * @brief Create depth texture
	*/
	virtual void CreateDepthTexture(const char* resourceName, const Int2& textureSize, float depthClearValue) = 0;

	/**
	 * @brief Create texture resource
	 * @param textureData Texture source
	 * @param textureSize Texture size
	 * @param pixelDataSize 1pixel data size
	*/
	virtual std::unique_ptr<Texture> CreateTexture(const std::vector<uint8_t>& textureData, const Int2& textureSize, unsigned int pixelDataSize) = 0;

	virtual void CreateRenderTarget(const char* renderTargetName, const Int2& renderTargetSize, ResourceFormat format) = 0;

	/**
	 * @brief Create rendering pipeline
	 * @param pipelineName Name to use when setting
	 * @param pipelineData pipeline settings
	*/
	virtual bool CreateRenderingPipeline(const char* pipelineName, PipelineStateSettings& pipelineData) = 0;

	virtual std::shared_ptr<RenderObject> CreateRenderObject(const char* objectName, std::vector<MeshVertex>& vertices, std::vector<uint16_t>& indices) = 0;

	void SetRenderingPipeline(RenderContext& renderContext, const char* pipelineName, const Int2& screenSize, float minDepth, float maxDepth);
	virtual void SetRenderingPipeline(CommandBuffer& commandBuffer, const char* pipelineName, const Int2& screenSize, float minDepth, float maxDepth) = 0;

	virtual void AddCamera(const char* cameraName, std::unique_ptr<Camera> camera);

	const Int2& GetWindowSize()const;
	Camera& GetCamera(const char* cameraName);
	virtual std::shared_ptr<RenderObject> GetRenderObject(const char* objectName) = 0;

protected:

	const Window& m_window;

	std::unordered_map<std::string, std::shared_ptr<RenderResource>> m_renderResources;

	std::unordered_map<std::string, std::unique_ptr<Camera>> m_cameras;
};