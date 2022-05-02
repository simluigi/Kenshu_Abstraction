#pragma once

#include <vector>
#include "../Utility/Math/Vector.h"

class RenderContext;
class RenderingData;

class Camera
{
public:

	Camera();
	virtual ~Camera();

	virtual void Update() = 0;

	const Int2& GetScreenSize()const;
	void SetScreenSize(const Int2& screenSize);

	const Float3& GetPosition()const;
	void SetPosiiton(const Float3& pos);

	void AddPosition(const Float3& pos);
	void AddCameraArmLength(float addLength);

protected:

	const char* m_cameraName = "";

	Int2 m_screenSize;

	Float3 m_pos;
	Float3 m_rotation;
	Float3 m_target;
	float m_near = 0.01f;
	float m_far = 10000.0f;
};