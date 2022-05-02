#include "D3D11Renderer.h"

#include "../D3DUtility/D3DCamera.h"
#include "D3D11RenderingData.h"
#include "D3D11RenderContext.h"
#include "../../Rendering/RenderPass.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

D3D11Renderer::D3D11Renderer()
{
}

D3D11Renderer::~D3D11Renderer()
{
}

bool D3D11Renderer::Create(const Window& window)
{
	std::unique_ptr<D3D11RenderingData> renderingData = std::make_unique<D3D11RenderingData>(window);
	std::unique_ptr<D3D11RenderContext> renderContext = std::make_unique<D3D11RenderContext>(window);

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

void D3D11Renderer::Render()
{
	m_renderingData->UpdateCamera();

	for (RenderPass* renderPass : m_renderPasses)
	{
		renderPass->Render(*m_renderContext, *m_renderingData);
	}
}

Camera& D3D11Renderer::CreateCamera(const char* cameraName, const Int2& screenSize)
{
	m_renderingData->AddCamera(cameraName, std::make_unique<D3DCamera>());

	Camera& camera = m_renderingData->GetCamera(cameraName);

	camera.SetScreenSize(screenSize);

	return camera;
}
