#pragma once

#include <memory>
#include <DirectXMath.h>
#include "../../Rendering/Camera.h"

class RenderResource;

class D3DCamera :
	public Camera
{
public:

	struct Info
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4 pos;
	};

public:

	D3DCamera();
	~D3DCamera();

	void Update()override;

	const Info& GetCameraInfo()const;

private:

	Info m_cameraInfo = {};
};

