#include "Camera.h"

#include "RenderContext.h"
#include "RenderingData.h"
#include "RenderPass.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

const Int2& Camera::GetScreenSize() const
{
	return m_screenSize;
}

void Camera::SetScreenSize(const Int2& screenSize)
{
	m_screenSize = screenSize;
}

const Float3& Camera::GetPosition() const
{
	return m_pos;
}

void Camera::SetPosiiton(const Float3& pos)
{
	m_pos = pos;
}

void Camera::AddPosition(const Float3& pos)
{
	m_pos += pos;
	m_target += pos;
}

void Camera::AddCameraArmLength(float addLength)
{
	Float3 forward = Vector::Normalize(m_target - m_pos);

	m_pos += forward * addLength;
	m_target += forward * addLength;
}
