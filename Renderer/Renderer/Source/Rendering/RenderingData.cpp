#include "RenderingData.h"

#include "Camera.h"
#include "RenderContext.h"
#include "../Windows/Window.h"

RenderingData::RenderingData(const Window& window) :
	m_window(window)
{
}

RenderingData::~RenderingData()
{
}

void RenderingData::UpdateCamera()
{
	for (auto& camera : m_cameras)
	{
		camera.second->Update();
	}
}

void RenderingData::SetRenderingPipeline(RenderContext& renderContext, const char* pipelineName, const Int2& screenSize, float minDepth, float maxDepth)
{
	SetRenderingPipeline(renderContext.GetMainCommandBuffer(), pipelineName, screenSize, minDepth, maxDepth);
}

Camera& RenderingData::GetCamera(const char* cameraName)
{
	return *m_cameras[cameraName];
}

void RenderingData::AddCamera(const char* cameraName, std::unique_ptr<Camera> camera)
{
	m_cameras[cameraName] = std::move(camera);
}

const Int2& RenderingData::GetWindowSize() const
{
	return m_window.GetWindowSize();
}
