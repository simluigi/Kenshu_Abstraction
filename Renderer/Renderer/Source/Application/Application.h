#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "../Windows/Window.h"
#include "../Utility/Math/Vector.h"

class Renderer;
class RenderPass;

class Application
{
public:

	enum class APIType
	{
		D3D11,
		D3D12,
	};

public:

	Application();
	~Application();
	
	static void Create(const Int2& windowSize);
	static bool Render();

	static void ChangeAPI(APIType apiType);

	static void SetWindowName(const wchar_t* windowName);
	static void CreateCamera(const char* cameraName, const Int2& screenSize);

	static void EnqueueRenderPass(RenderPass* renderPass);

private:

	static Application& Instance();

	void InitializeAPI(APIType type);

	Window m_window = {};
	std::unique_ptr<Renderer> m_renderer;

	std::vector<std::pair<std::string, Int2>> m_cameraNames;
	std::vector<RenderPass*> m_renderPasses;

	bool m_changeAPI = false;
	APIType m_changeAPIType = {};
};