#pragma once

#include "../../Rendering/Renderer.h"

class D3DCamera;
class D3D11RenderContext;
class D3D11RenderingData;

class D3D11Renderer :
	public Renderer
{
public:

	D3D11Renderer();
	~D3D11Renderer();

	bool Create(const Window& window)override;
	void Render()override;

	Camera& CreateCamera(const char* cameraName, const Int2& screenSize)override;

private:

};

