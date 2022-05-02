#include "D3DCamera.h"

using namespace DirectX;

D3DCamera::D3DCamera()
{
}

D3DCamera::~D3DCamera()
{
}

void D3DCamera::Update()
{
	XMVECTOR eye = XMVectorSet(m_pos.x, m_pos.y, m_pos.z, 0);
	XMVECTOR target = XMVectorSet(m_target.x, m_target.y, m_target.z, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);

	XMMATRIX viewMatrix = XMMatrixTranspose(XMMatrixLookAtLH(eye, target, up));

	XMStoreFloat4x4(&m_cameraInfo.view, viewMatrix);

	constexpr float fov = XMConvertToRadians(60.0f);
	float aspect = (float)m_screenSize.x / (float)m_screenSize.y;

	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspect, m_near, m_far);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	XMStoreFloat4x4(&m_cameraInfo.projection, projectionMatrix);
	m_cameraInfo.pos = { eye.m128_f32[0], eye.m128_f32[1], eye.m128_f32[2], 1.0f };
}

const D3DCamera::Info& D3DCamera::GetCameraInfo() const
{
    return m_cameraInfo;
}
