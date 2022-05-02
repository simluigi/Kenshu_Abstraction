
#include "Application/MeshRenderPass.h"
#include "Application/PostProcess.h"
#include "Application/Application.h"
#include "Application/ShadowPass.h"

INT WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ INT)
{
	Int2 windowSize = Int2(1600, 900);

	Application::Create(windowSize);

	Application::CreateCamera("MainCamera", windowSize);
	Application::CreateCamera("ShadowMapCamera", windowSize);

	MeshRenderPass meshRenderPass;
	ShadowPass shadowPass;
	PostProcess postProcess;

	Application::EnqueueRenderPass(&shadowPass);
	Application::EnqueueRenderPass(&meshRenderPass);
	Application::EnqueueRenderPass(&postProcess);

	while (true)
	{
		if (!Application::Render())
		{
			break;
		}
	}

	return 0;
}