#include "MeshRenderPass.h"

#include "../Windows/Input/Input.h"
#include "../Rendering/Camera.h"
#include "../Rendering/Texture.h"
#include "../Rendering/RenderObject.h"
#include "../Rendering/RenderContext.h"
#include "../Rendering/RenderingData.h"
#include "../Rendering/CommandBuffer.h"
#include "../Windows/ThreadPool.h"

MeshRenderPass::MeshRenderPass()
{
}

MeshRenderPass::~MeshRenderPass()
{
}

bool MeshRenderPass::Create(RenderContext& renderContext, RenderingData& renderingData)
{
	if (!CreateRenderingPipeline(renderingData))
	{
		return false;
	}

    Camera& camera = renderingData.GetCamera("MainCamera");
    camera.SetPosiiton(Float3(0, 2, -4.5));

    m_commandBuffers.resize(2);

    m_commandBuffers[0] = renderContext.GetCommandBuffer("CommandBuffer1");
    m_commandBuffers[1] = renderContext.GetCommandBuffer("CommandBuffer2");

    m_boxTransforms.resize(3);

    m_boxTransforms[0].pos.x = -1.5f;
    m_boxTransforms[1].pos.x = 1.5f;

    m_boxTransforms[0].pos.y = 1.5f;
    m_boxTransforms[1].pos.y = 1.5f;
    m_boxTransforms[2].pos.y = 1.5f;

    m_boxTransforms[0].pos.z = 1.5f;
    m_boxTransforms[1].pos.z = 1.5f;

    m_boxRenderObjects.resize(3);

    m_boxRenderObjects[0] = renderingData.GetRenderObject("Box0");
    m_boxRenderObjects[1] = renderingData.GetRenderObject("Box1");
    m_boxRenderObjects[2] = renderingData.GetRenderObject("Box2");
    m_planeRenderObject = renderingData.GetRenderObject("Plane");

    m_input = std::make_unique<Input>();

    m_planeTransform.scale = Float3(1000, 1, 1000);

    m_currentThreadIndex = renderContext.GetSwapChainRenderTargetIndex();

    renderingData.CreateRenderTarget("MainTexture", renderingData.GetWindowSize(), ResourceFormat::R8G8B8A8_UNORM);
    renderingData.CreateDepthTexture("MainDepthTexture", renderingData.GetWindowSize(), 1.0f);

	return true;
}

void MeshRenderPass::Render(RenderContext& renderContext, RenderingData& renderingData)
{
    m_input->Update();

    UpdateCamera(renderingData, "MainCamera");

    for (Transform& transform : m_boxTransforms)
    {
        transform.rotation.y += 0.01f;
    }

	float clearColor[4] = { 0.5843f, 0.7529f, 0.9255f, 0.0f };

    CommandBuffer& commandBuffer = *m_commandBuffers[m_currentThreadIndex];

    commandBuffer.SetFrameResourceIndex(m_currentThreadIndex);

    renderContext.SetRenderTarget(commandBuffer, renderingData, "MainTexture", clearColor, "MainDepthTexture");

	constexpr float minDepth = 0.0f;
	constexpr float maxDepth = 1.0f;
	renderingData.SetRenderingPipeline(commandBuffer, "MeshRender", renderingData.GetWindowSize(), minDepth, maxDepth);

    commandBuffer.SetCamera(renderingData, "MainCamera");
    commandBuffer.SetLightViewCamera(renderingData, "ShadowMapCamera");

    commandBuffer.SetDepthTexture(renderingData, "ShadowDepthTexture", 1);

    for (int objectIndex = 0; objectIndex < m_boxRenderObjects.size(); ++objectIndex)
    {
        m_boxRenderObjects[objectIndex]->DrawIndexed(commandBuffer, m_boxTransforms[objectIndex], true);
    }

    m_planeRenderObject->DrawIndexed(commandBuffer, m_planeTransform);

    m_currentThreadIndex = ++m_currentThreadIndex % 2;
}

void MeshRenderPass::Terminate(RenderContext& renderContext)
{
    renderContext.Terminate();
}

bool MeshRenderPass::CreateRenderingPipeline(RenderingData& renderingData)
{
	RenderingData::PipelineStateSettings pipelineSetting = {};

    pipelineSetting.renderTargetResourceFormat.emplace_back(ResourceFormat::R8G8B8A8_UNORM);

	pipelineSetting.inputLayout.resize(3);

	pipelineSetting.inputLayout = { {"POSITION", ResourceFormat::R32G32B32_FLOAT},
									 {"NORMAL", ResourceFormat::R32G32B32_FLOAT},
									 {"TEXCOORD", ResourceFormat::R32G32_FLOAT} };

	pipelineSetting.vertexShader.filePath = L"Source/Shader/VertexShader.hlsl";
	pipelineSetting.vertexShader.entryPoint = "main";
	pipelineSetting.vertexShader.target = "vs_5_0";

	pipelineSetting.pixelShader.filePath = L"Source/Shader/PixelShader.hlsl";
	pipelineSetting.pixelShader.entryPoint = "main";
	pipelineSetting.pixelShader.target = "ps_5_0";

    pipelineSetting.shaderParameter.resize(5);

    // camera
    pipelineSetting.shaderParameter[0].type = RenderingData::ShaderParameterType::CBV;

    // worldMatrix
    pipelineSetting.shaderParameter[1].type = RenderingData::ShaderParameterType::CBV;
    pipelineSetting.shaderParameter[1].registerNum = 1;

    // shadowCamera
    pipelineSetting.shaderParameter[2].type = RenderingData::ShaderParameterType::CBV;
    pipelineSetting.shaderParameter[2].registerNum = 2;

    // meshTexture
    pipelineSetting.shaderParameter[3].type = RenderingData::ShaderParameterType::SRV;
    pipelineSetting.shaderParameter[3].useDescriptorTable = true;

    // shadowDepth
    pipelineSetting.shaderParameter[4].type = RenderingData::ShaderParameterType::SRV;
    pipelineSetting.shaderParameter[4].useDescriptorTable = true;
    pipelineSetting.shaderParameter[4].registerNum = 1;

	return renderingData.CreateRenderingPipeline("MeshRender", pipelineSetting);
}

void MeshRenderPass::UpdateCamera(RenderingData& renderingData, const char* cameraName)
{
    constexpr float MOUSE_MOVE_SPEED_XY = 0.008f;
    constexpr float MOUSE_MOVE_SPEED_Z = 0.01f;

    Camera& camera = renderingData.GetCamera(cameraName);

    Float3 mouseMove = m_input->GetMouseMove();

    camera.AddCameraArmLength(mouseMove.z * MOUSE_MOVE_SPEED_Z);

    if (m_input->IsMousePressed(Input::MouseButton::middle))
    {
        camera.AddPosition(Float3(-mouseMove.x * MOUSE_MOVE_SPEED_XY, mouseMove.y * MOUSE_MOVE_SPEED_XY, 0.0f));
    }
}
