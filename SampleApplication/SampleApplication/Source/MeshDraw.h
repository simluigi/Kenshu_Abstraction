#pragma once

#include <memory>
#include <vector>
#include <Rendering/RenderPass.h>
#include <Utility/Math/Transform.h>

class Input;
class RenderObject;
class CommandBuffer;

class MeshDraw :
    public RenderPass
{
public:

	MeshDraw();
	~MeshDraw();

	bool Create(RenderContext& renderContext, RenderingData& renderingData)override;
	void Render(RenderContext& renderContext, RenderingData& renderingData)override;
	void Terminate(RenderContext& renderContext)override;

private:

	bool CreateRenderingPipeline(RenderingData& renderingData);

	void UpdateCamera(RenderingData& renderingData, const char* cameraName);

	std::vector<std::shared_ptr<CommandBuffer>> m_commandBuffers;

	std::vector<Transform> m_boxTransforms = {};
	std::vector<std::shared_ptr<RenderObject>> m_boxRenderObjects;

	Transform m_planeTransform = {};
	std::shared_ptr<RenderObject> m_planeRenderObject;

	unsigned int m_currentThreadIndex = 0;

	std::unique_ptr<Input> m_input;
};

