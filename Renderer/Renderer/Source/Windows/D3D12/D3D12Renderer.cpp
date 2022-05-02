#include "D3D12Renderer.h"

#include "../../Rendering/RenderPass.h"
#include "../D3DUtility/D3DCamera.h"
#include "D3D12RenderContext.h"
#include "D3D12RenderingData.h"

D3D12Renderer::D3D12Renderer()
{
}

D3D12Renderer::~D3D12Renderer()
{
}

bool D3D12Renderer::Create(const Window& window)
{
	std::unique_ptr<D3D12RenderingData> renderingData = std::make_unique<D3D12RenderingData>(window);
	std::unique_ptr<D3D12RenderContext> renderContext = std::make_unique<D3D12RenderContext>(window);

	if (!renderContext->Create())
	{
		return false;
	}

	renderingData->SetD3DDevice(renderContext->GetDevice());

	renderingData->Create();

	m_renderContext = std::move(renderContext);
	m_renderingData = std::move(renderingData);

	return true;
}

void D3D12Renderer::Render()
{
	m_renderingData->UpdateCamera();

	for (RenderPass* renderPass : m_renderPasses)
	{
		renderPass->Render(*m_renderContext, *m_renderingData);
	}
}

Camera& D3D12Renderer::CreateCamera(const char* cameraName, const Int2& screenSize)
{
	m_renderingData->AddCamera(cameraName, std::make_unique<D3DCamera>());

	Camera& camera = m_renderingData->GetCamera(cameraName);

	camera.SetScreenSize(screenSize);

	return camera;
}
