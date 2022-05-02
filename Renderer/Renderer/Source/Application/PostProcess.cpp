#include "PostProcess.h"

#include "../Rendering/RenderingData.h"
#include "../Rendering/RenderContext.h"
#include "../Rendering/CommandBuffer.h"
#include "../Rendering/RenderObject.h"
#include "../Windows/ThreadPool.h"

bool PostProcess::Create(RenderContext& renderContext, RenderingData& renderingData)
{
	if (!CreateFullScreenPlane(renderingData))
	{
        return false;
	}

    if (!CreateRenderingPipeline(renderingData))
    {
        return false;
    }
    
    m_commandBuffers.resize(2);

    m_commandBuffers[0] = renderContext.GetCommandBuffer("CommandBuffer1");
    m_commandBuffers[1] = renderContext.GetCommandBuffer("CommandBuffer2");

    m_currentThreadIndex = renderContext.GetSwapChainRenderTargetIndex();

	return true;
}

void PostProcess::Render(RenderContext& renderContext, RenderingData& renderingData)
{
    float clearColor[4] = { 0, 0, 0, 0.0f };

    CommandBuffer& commandBuffer = *m_commandBuffers[m_currentThreadIndex];

    commandBuffer.SetFrameResourceIndex(m_currentThreadIndex);

    renderContext.SetSwapChainRenderTarget(commandBuffer, m_currentThreadIndex, clearColor, true);

    constexpr float minDepth = 0.0f;
    constexpr float maxDepth = 1.0f;
    renderingData.SetRenderingPipeline(commandBuffer, "PostProcess", renderingData.GetWindowSize(), minDepth, maxDepth);

    commandBuffer.SetRenderTargetTexture(renderingData, "MainTexture", 0);

    m_fullScreenPlane->DrawIndexed(commandBuffer);

    renderContext.EndFrameRendering(commandBuffer, m_currentThreadIndex);

    if (m_threadAwaiter.valid())
    {
        m_threadAwaiter.wait();
    }

    auto ExecuteCommandBufferThread = [this, &renderContext, &renderingData, &commandBuffer]
    {
        renderContext.ExecuteCommandBuffers(&commandBuffer, 1);

        renderContext.WaitForCommandExecute();

        constexpr int vsync = 1;
        renderContext.SwapChainPresent(vsync);

        commandBuffer.Reset();
    };

    m_threadAwaiter = ThreadPool::AddTask(ExecuteCommandBufferThread);

    m_currentThreadIndex = ++m_currentThreadIndex % 2;
}

void PostProcess::Terminate(RenderContext& renderContext)
{
    if (m_threadAwaiter.valid())
    {
        m_threadAwaiter.wait();
    }

    for (std::shared_ptr<CommandBuffer> commandBuffer : m_commandBuffers)
    {
        renderContext.ExecuteCommandBuffers(commandBuffer.get(), 1);

        renderContext.WaitForCommandExecute();

        commandBuffer->Reset();
    }
}

bool PostProcess::CreateRenderingPipeline(RenderingData& renderingData)
{
    RenderingData::PipelineStateSettings pipelineSetting = {};

    pipelineSetting.renderTargetResourceFormat.emplace_back(ResourceFormat::R8G8B8A8_UNORM);

    pipelineSetting.inputLayout.resize(3);

    pipelineSetting.inputLayout = { {"POSITION", ResourceFormat::R32G32B32_FLOAT},
                                     {"NORMAL", ResourceFormat::R32G32B32_FLOAT},
                                     {"TEXCOORD", ResourceFormat::R32G32_FLOAT} };

    pipelineSetting.vertexShader.filePath = L"Source/Shader/PostProcessVS.hlsl";
    pipelineSetting.vertexShader.entryPoint = "main";
    pipelineSetting.vertexShader.target = "vs_5_0";

    pipelineSetting.pixelShader.filePath = L"Source/Shader/PostProcessPS.hlsl";
    pipelineSetting.pixelShader.entryPoint = "main";
    pipelineSetting.pixelShader.target = "ps_5_0";

    pipelineSetting.shaderParameter.resize(1);

    // mainTexture
    pipelineSetting.shaderParameter[0].type = RenderingData::ShaderParameterType::SRV;
    pipelineSetting.shaderParameter[0].useDescriptorTable = true;

    return renderingData.CreateRenderingPipeline("PostProcess", pipelineSetting);
}

bool PostProcess::CreateFullScreenPlane(RenderingData& renderingData)
{
    std::vector<MeshVertex> vertices =
    {
        { Float3(-1, -1, 0), Float3(0, 1, 0),Float2(0, 1) },
        { Float3(-1, 1, 0), Float3(0, 1, 0),Float2(0, 0) },
        { Float3(1, -1, 0), Float3(0, 1, 0),Float2(1, 1) },
        { Float3(1, 1, 0), Float3(0, 1, 0),Float2(1, 0) }
    };

    std::vector<uint16_t> indices = { 0, 1, 2, 3, 2, 1 };

    m_fullScreenPlane = renderingData.CreateRenderObject("FullScreenPlane", vertices, indices);

    return m_fullScreenPlane.get() != nullptr;
}
