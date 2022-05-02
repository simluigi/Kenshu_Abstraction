#pragma once

#include <memory>
#include <vector>
#include <Rendering/RenderPass.h>
#include <Utility/Math/Transform.h>

class CommandBuffer;
class RenderObject;

class ShadowMap :
    public RenderPass
{
public:

    bool Create(RenderContext& renderContext, RenderingData& renderingData)override;
    void Render(RenderContext& renderContext, RenderingData& renderingData)override;
    void Terminate(RenderContext& renderContext)override;

private:

    bool CreateRenderingPipeline(RenderingData& renderingData);
    bool CreateBox(RenderingData& renderingData);
    bool CreatePlane(RenderingData& renderingData);
    bool CreateBoxTexture(RenderingData& renderingData);
    bool CreatePlaneTexture(RenderingData& renderingData);

    std::vector<std::shared_ptr<CommandBuffer>> m_commandBuffers;

    unsigned int m_currentThreadIndex = 0;

    std::vector<Transform> m_boxTransforms = {};
    std::vector<std::shared_ptr<RenderObject>> m_boxRenderObjects;

    Transform m_planeTransform = {};
    std::shared_ptr<RenderObject> m_planeRenderObject;
};

