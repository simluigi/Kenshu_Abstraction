#pragma once

#include "../../Rendering/Renderer.h"

class D3D12Renderer :
    public Renderer
{
public:

	D3D12Renderer();
	~D3D12Renderer();

	bool Create(const Window& window)override;
	void Render()override;

	Camera& CreateCamera(const char* cameraName, const Int2& screenSize)override;

private:

};

