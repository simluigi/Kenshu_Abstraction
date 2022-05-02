
#include <Application/Application.h>
#include "MeshDraw.h"
#include "PostProcess.h"
#include "ShadowMap.h"

INT WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ INT)
{
	Int2 windowSize = Int2(1600, 900);

	Application::Create(windowSize);

	Application::CreateCamera("MainCamera", windowSize);
	Application::CreateCamera("ShadowMapCamera", windowSize);

	ShadowMap shadowMap;
	MeshDraw meshDraw;
	PostProcess postProcess;

	Application::EnqueueRenderPass(&shadowMap);
	Application::EnqueueRenderPass(&meshDraw);
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