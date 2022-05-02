#include "Renderer.h"

#include "Camera.h"
#include "RenderPass.h"
#include "RenderContext.h"
#include "RenderingData.h"

constexpr unsigned int CAMERA_SHADER_SLOT = 0;
constexpr unsigned int WORLDMATRIX_SHADER_SLOT = 1;
constexpr unsigned int SHADOW_CAMERA_SHADER_SLOT = 2;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::Terminate()
{
	for (RenderPass* renderPass : m_renderPasses)
	{
		renderPass->Terminate(*m_renderContext);
	}
}

unsigned int Renderer::CameraShaderSlot()
{
	return CAMERA_SHADER_SLOT;
}

unsigned int Renderer::WorldMatrixShaderSlot()
{
	return WORLDMATRIX_SHADER_SLOT;
}

unsigned int Renderer::LightViewCameraShaderSlot()
{
	return SHADOW_CAMERA_SHADER_SLOT;
}

void Renderer::EnqueueRenderPass(RenderPass* renderPass)
{
	renderPass->Create(*m_renderContext , *m_renderingData);

	m_renderPasses.emplace_back(renderPass);
}
