#pragma once

#include <memory>
#include <vector>
#include <future>
#include <Rendering/RenderPass.h>

class CommandBuffer;
class RenderObject;

class PostProcess :
    public RenderPass
{
public:

    bool Create(RenderContext& renderContext, RenderingData& renderingData)override;
    void Render(RenderContext& renderContext, RenderingData& renderingData)override;
    void Terminate(RenderContext& renderContext)override;

private:

    bool CreateRenderingPipeline(RenderingData& renderingData);
    bool CreateFullScreenPlane(RenderingData& renderingData);

    std::shared_ptr<RenderObject> m_fullScreenPlane;
    std::vector<std::shared_ptr<CommandBuffer>> m_commandBuffers;

    std::future<void> m_threadAwaiter;

    unsigned int m_currentThreadIndex = 0;
};

