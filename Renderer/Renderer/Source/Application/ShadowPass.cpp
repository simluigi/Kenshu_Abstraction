#include "ShadowPass.h"

#include <sstream>
#include "../Rendering/Camera.h"
#include "../Rendering/RenderContext.h"
#include "../Rendering/RenderingData.h"
#include "../Rendering/RenderObject.h"
#include "../Rendering/CommandBuffer.h"
#include "../Rendering/Texture.h"

bool ShadowPass::Create(RenderContext& renderContext, RenderingData& renderingData)
{
	if (!CreateBox(renderingData))
	{
		return false;
	}

	if (!CreatePlane(renderingData))
	{
		return false;
	}

	if (!CreateBoxTexture(renderingData))
	{
		return false;
	}

	if (!CreatePlaneTexture(renderingData))
	{
		return false;
	}

	if (!CreateRenderingPipeline(renderingData))
	{
		return false;
	}

	m_commandBuffers.resize(2);

	m_commandBuffers[0] = renderContext.CreateCommandBuffer("CommandBuffer1");
	m_commandBuffers[1] = renderContext.CreateCommandBuffer("CommandBuffer2");

	m_currentThreadIndex = renderContext.GetSwapChainRenderTargetIndex();

	renderingData.CreateDepthTexture("ShadowDepthTexture", Int2(2048, 2048), 1.0f);

	Camera& camera = renderingData.GetCamera("ShadowMapCamera");
	camera.SetPosiiton(Float3(3, 5, -10));
	camera.SetScreenSize(Int2(2048, 2048));

	m_boxTransforms.resize(3);

	m_boxTransforms[0].pos.x = -1.5f;
	m_boxTransforms[1].pos.x = 1.5f;

	m_boxTransforms[0].pos.y = 1.5f;
	m_boxTransforms[1].pos.y = 1.5f;
	m_boxTransforms[2].pos.y = 1.5f;

	m_boxTransforms[0].pos.z = 1.5f;
	m_boxTransforms[1].pos.z = 1.5f;

	m_planeTransform.scale = Float3(1000, 1, 1000);

    return true;
}

void ShadowPass::Render(RenderContext& renderContext, RenderingData& renderingData)
{
	for (Transform& transform : m_boxTransforms)
	{
		transform.rotation.y += 0.01f;
	}

	float clearColor[4] = { 0, 0, 0, 0 };

	CommandBuffer& commandBuffer = *m_commandBuffers[m_currentThreadIndex];

	commandBuffer.SetFrameResourceIndex(m_currentThreadIndex);

	renderContext.SetRenderTarget(commandBuffer, renderingData, nullptr, clearColor, "ShadowDepthTexture");

	constexpr float minDepth = 0.0f;
	constexpr float maxDepth = 1.0f;
	renderingData.SetRenderingPipeline(commandBuffer, "Shadow", Int2(2048, 2048), minDepth, maxDepth);

	commandBuffer.SetCamera(renderingData, "ShadowMapCamera");

	for (int objectIndex = 0; objectIndex < m_boxRenderObjects.size(); ++objectIndex)
	{
		m_boxRenderObjects[objectIndex]->DrawIndexed(commandBuffer, m_boxTransforms[objectIndex], false);
	}

	m_planeRenderObject->DrawIndexed(commandBuffer, m_planeTransform, false);

	m_currentThreadIndex = ++m_currentThreadIndex % 2;
}

void ShadowPass::Terminate(RenderContext& renderContext)
{
	renderContext.Terminate();
}

bool ShadowPass::CreateRenderingPipeline(RenderingData& renderingData)
{
	RenderingData::PipelineStateSettings pipelineSetting = {};

	pipelineSetting.inputLayout.resize(3);

	pipelineSetting.inputLayout = { {"POSITION", ResourceFormat::R32G32B32_FLOAT},
									 {"NORMAL", ResourceFormat::R32G32B32_FLOAT},
									 {"TEXCOORD", ResourceFormat::R32G32_FLOAT} };

	pipelineSetting.vertexShader.filePath = L"Source/Shader/LightViewVertexShader.hlsl";
	pipelineSetting.vertexShader.entryPoint = "main";
	pipelineSetting.vertexShader.target = "vs_5_0";

	pipelineSetting.shaderParameter.resize(3);

	// camera
	pipelineSetting.shaderParameter[0].type = RenderingData::ShaderParameterType::CBV;

	// worldMatrix
	pipelineSetting.shaderParameter[1].type = RenderingData::ShaderParameterType::CBV;
	pipelineSetting.shaderParameter[1].registerNum = 1;

	return renderingData.CreateRenderingPipeline("Shadow", pipelineSetting);
}

bool ShadowPass::CreateBox(RenderingData& renderingData)
{
	std::vector<MeshVertex> vertices =
	{
		// ëOñ 
		{ Float3(-0.5f,  0.5f, -0.5f), Float3(0,  0, -1), Float2(0, 0) },
		{ Float3(0.5f,  0.5f, -0.5f), Float3(0,  0, -1), Float2(1, 0) },
		{ Float3(-0.5f, -0.5f, -0.5f), Float3(0,  0, -1), Float2(0, 1) },
		{ Float3(0.5f, -0.5f, -0.5f), Float3(0,  0, -1), Float2(1, 1) },

		// ó†ñ 
		{ Float3(-0.5f,  0.5f,  0.5f), Float3(0,  0, 1), Float2(0, 1) },
		{ Float3(-0.5f, -0.5f,  0.5f), Float3(0,  0, 1), Float2(0, 0) },
		{ Float3(0.5f,  0.5f,  0.5f), Float3(0,  0, 1), Float2(1, 1) },
		{ Float3(0.5f, -0.5f,  0.5f), Float3(0,  0, 1), Float2(1, 0) },

		// ç∂ñ 
		{ Float3(-0.5f,  0.5f,  0.5f), Float3(-1, 0, 0), Float2(0, 0) },
		{ Float3(-0.5f,  0.5f, -0.5f), Float3(-1, 0, 0), Float2(1, 0) },
		{ Float3(-0.5f, -0.5f,  0.5f), Float3(-1, 0, 0), Float2(0, 1) },
		{ Float3(-0.5f, -0.5f, -0.5f), Float3(-1, 0, 0), Float2(1, 1) },

		// âEñ 
		{ Float3(0.5f,  0.5f,  0.5f), Float3(1, 0, 0), Float2(1, 0) },
		{ Float3(0.5f, -0.5f,  0.5f), Float3(1, 0, 0), Float2(1, 1) },
		{ Float3(0.5f,  0.5f, -0.5f), Float3(1, 0, 0), Float2(0, 0) },
		{ Float3(0.5f, -0.5f, -0.5f), Float3(1, 0, 0), Float2(0, 1) },

		// è„ñ 
		{ Float3(-0.5f,  0.5f,  0.5f), Float3(0, 1, 0), Float2(0, 0) },
		{ Float3(0.5f,  0.5f,  0.5f), Float3(0, 1, 0), Float2(1, 0) },
		{ Float3(-0.5f,  0.5f, -0.5f), Float3(0, 1, 0), Float2(0, 1) },
		{ Float3(0.5f,  0.5f, -0.5f), Float3(0, 1, 0), Float2(1, 1) },

		// â∫ñ 
		{ Float3(-0.5f, -0.5f,  0.5f), Float3(0, -1, 0), Float2(0, 1) },
		{ Float3(-0.5f, -0.5f, -0.5f), Float3(0, -1, 0), Float2(0, 0) },
		{ Float3(0.5f, -0.5f,  0.5f), Float3(0, -1, 0), Float2(1, 1) },
		{ Float3(0.5f, -0.5f, -0.5f), Float3(0, -1, 0), Float2(1, 0) },
	};

	std::vector<uint16_t> indices =
	{
		 0,  1,  2,     3,  2,  1,
		 4,  5,  6,     7,  6,  5,
		 8,  9, 10,    11, 10,  9,
		12, 13, 14,    15, 14, 13,
		16, 17, 18,    19, 18, 17,
		20, 21, 22,    23, 22, 21,
	};

	m_boxRenderObjects.resize(3);

	unsigned int boxCount = 0;
	for (std::shared_ptr<RenderObject>& renderObject : m_boxRenderObjects)
	{
		std::stringstream boxNumStr;
		boxNumStr << "Box" << boxCount;

		renderObject = renderingData.CreateRenderObject(boxNumStr.str().c_str(), vertices, indices);

		++boxCount;
	}

	return true;
}

bool ShadowPass::CreatePlane(RenderingData& renderingData)
{
	std::vector<MeshVertex> vertices =
	{
		{ Float3(-0.5f, 0, -0.5f), Float3(0, 1, 0),Float2(0, 0) },
		{ Float3(-0.5f, -0, 0.5f), Float3(0, 1, 0),Float2(1, 1) },
		{ Float3(0.5f, -0, -0.5f), Float3(0, 1, 0),Float2(0, 1) },
		{ Float3(0.5f, 0, 0.5f), Float3(0, 1, 0),Float2(1, 0) }
	};

	std::vector<uint16_t> indices = { 0, 1, 2, 3, 2, 1 };

	m_planeRenderObject = renderingData.CreateRenderObject("Plane", vertices, indices);

	return m_planeRenderObject.get() != nullptr;
}

bool ShadowPass::CreateBoxTexture(RenderingData& renderingData)
{
	constexpr int TEXTURE_SIZE = 256;
	constexpr int PIXEL_DATA_SIZE = 4;

	struct Color
	{
		uint8_t r, g, b;
	};

	std::vector<Color> boxTextureColor = { Color(0xff, 0x00, 0x00), Color(0x00, 0x00, 0xff), Color(0x00, 0xff, 0x00) };

	unsigned int boxTextureIndex = 0;

	for (const Color& textureColor : boxTextureColor)
	{
		std::vector<uint8_t> textureData;
		textureData.resize(TEXTURE_SIZE * TEXTURE_SIZE * PIXEL_DATA_SIZE);

		Int2 textureSize = Int2(TEXTURE_SIZE, TEXTURE_SIZE);

		Int2 pixelCount;

		for (int y = 0; y < textureSize.y * PIXEL_DATA_SIZE; y += PIXEL_DATA_SIZE)
		{
			for (int x = 0; x < textureSize.x * PIXEL_DATA_SIZE; x += PIXEL_DATA_SIZE)
			{
				constexpr float DIV_COUNT = 10.0f;

				Float2 uv = Float2((float)pixelCount.x / (float)textureSize.x, (float)pixelCount.y / (float)textureSize.y);

				float xValue = floorf(uv.x * DIV_COUNT);
				float yValue = floorf(uv.y * DIV_COUNT);

				if (fmod(xValue + yValue, 2.0) != 0)
				{
					textureData[y * textureSize.x + x] = textureColor.r;
					textureData[y * textureSize.x + x + 1] = textureColor.g;
					textureData[y * textureSize.x + x + 2] = textureColor.b;
					textureData[y * textureSize.x + x + 3] = 0xff;
				}
				else
				{
					textureData[y * textureSize.x + x] = 0xff;
					textureData[y * textureSize.x + x + 1] = 0xff;
					textureData[y * textureSize.x + x + 2] = 0xff;
					textureData[y * textureSize.x + x + 3] = 0xff;
				}

				++pixelCount.x;
			}

			++pixelCount.y;
		}

		std::unique_ptr<Texture> texture = renderingData.CreateTexture(textureData, textureSize, PIXEL_DATA_SIZE);

		if (texture.get() == nullptr)
		{
			return false;
		}

		m_boxRenderObjects[boxTextureIndex]->AddTexture(std::move(texture), 0);

		++boxTextureIndex;
	}

	return true;
}

bool ShadowPass::CreatePlaneTexture(RenderingData& renderingData)
{
	constexpr int TEXTURE_SIZE = 256;
	constexpr int PIXEL_DATA_SIZE = 4;

	std::vector<uint8_t> textureData;
	textureData.resize(TEXTURE_SIZE * TEXTURE_SIZE * PIXEL_DATA_SIZE);

	Int2 textureSize = Int2(TEXTURE_SIZE, TEXTURE_SIZE);

	std::fill(textureData.begin(), textureData.end(), (uint8_t)0xff);

	std::unique_ptr<Texture> texture = renderingData.CreateTexture(textureData, textureSize, PIXEL_DATA_SIZE);

	if (texture.get() == nullptr)
	{
		return false;
	}

	m_planeRenderObject->AddTexture(std::move(texture), 0);

	return true;
}
