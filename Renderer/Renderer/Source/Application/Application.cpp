#include "Application.h"

#include "../Windows/Window.h"
#include "../Windows/MenuResource.h"
#include "../Windows/D3D11/D3D11Renderer.h"
#include "../Windows/D3D12/D3D12Renderer.h"
#include "../Rendering/Renderer.h"
#include "../Application/MeshRenderPass.h"
#include "../Application/PostProcess.h"

Application::Application()
{
}

Application::~Application()
{
}

void Application::Create(const Int2& windowSize)
{
	Application& app = Instance();

	if (!app.m_window.Create(windowSize))
	{
		/// error check
		MessageBox(nullptr, L"Failed to create window!", L"Error", MB_OK);
		return;
	}

	app.m_renderer = std::make_unique<D3D12Renderer>();

	if (!app.m_renderer->Create(app.m_window))
	{
		/// error check
		MessageBox(nullptr, L"Failed to create D3D12 renderer!", L"Error", MB_OK);

		return;
	}

	CheckMenuItem(GetMenu(app.m_window.GetWindowHandle()), IDR_D3D12, MFS_CHECKED);
}

bool Application::Render()
{
	Application& app = Instance();

	if (!app.m_window.Update())
	{
		return false;
	}

	app.m_renderer->Render();

	if (app.m_changeAPI)
	{
		app.InitializeAPI(app.m_changeAPIType);

		app.m_changeAPI = false;
	}

	return true;
}

void Application::ChangeAPI(APIType apiType)
{
	Application& app = Instance();

	switch (apiType)
	{
	case Application::APIType::D3D11:

		app.m_changeAPI = true;
		app.m_changeAPIType = Application::APIType::D3D11;

		CheckMenuItem(GetMenu(app.m_window.GetWindowHandle()), IDR_D3D11, MFS_CHECKED);
		CheckMenuItem(GetMenu(app.m_window.GetWindowHandle()), IDR_D3D12, MFS_UNCHECKED);

		break;

	case Application::APIType::D3D12:

		app.m_changeAPI = true;
		app.m_changeAPIType = Application::APIType::D3D12;

		CheckMenuItem(GetMenu(app.m_window.GetWindowHandle()), IDR_D3D11, MFS_UNCHECKED);
		CheckMenuItem(GetMenu(app.m_window.GetWindowHandle()), IDR_D3D12, MFS_CHECKED);

		break;

	default:
		break;
	}
}

void Application::SetWindowName(const wchar_t* windowName)
{
	Application& app = Instance();

	app.m_window.SetWindowName(windowName);
}

void Application::CreateCamera(const char* cameraName, const Int2& screenSize)
{
	Application& app = Instance();
	app.m_renderer->CreateCamera(cameraName, screenSize);

	app.m_cameraNames.emplace_back(std::make_pair(cameraName, screenSize));
}

void Application::EnqueueRenderPass(RenderPass* renderPass)
{
	Application& app = Instance();
	app.m_renderer->EnqueueRenderPass(renderPass);

	app.m_renderPasses.emplace_back(renderPass);
}

Application& Application::Instance()
{
	static Application instance;
	return instance;
}

void Application::InitializeAPI(APIType type)
{
	Application& app = Instance();

	switch (type)
	{
	case Application::APIType::D3D11:

		MessageBox(app.m_window.GetWindowHandle(), L"Switched to D3D11", L"D3D11", MB_OK);

		app.m_renderer->Terminate();

		app.m_renderer = std::make_unique<D3D11Renderer>();

		if (!app.m_renderer->Create(app.m_window))
		{
			/// error check
			MessageBox(nullptr, L"Failed to create D3D12 renderer!", L"Error", MB_OK);
			return;
		}

		for (auto& camera : app.m_cameraNames)
		{
			app.m_renderer->CreateCamera(camera.first.c_str(), camera.second);
		}

		for (auto& renderPass : app.m_renderPasses)
		{
			app.m_renderer->EnqueueRenderPass(renderPass);
		}

		break;

	case Application::APIType::D3D12:

		MessageBox(app.m_window.GetWindowHandle(), L"Switched to D3D12", L"D3D12", MB_OK);

		app.m_renderer->Terminate();

		app.m_renderer = std::make_unique<D3D12Renderer>();

		if (!app.m_renderer->Create(app.m_window))
		{
			/// error check
			MessageBox(nullptr, L"Failed to create D3D12 renderer!", L"Error", MB_OK);
			return;
		}

		for (auto& camera : app.m_cameraNames)
		{
			app.m_renderer->CreateCamera(camera.first.c_str(), camera.second);
		}

		for (auto& renderPass : app.m_renderPasses)
		{
			app.m_renderer->EnqueueRenderPass(renderPass);
		}

		break;

	default:
		break;
	}
}
