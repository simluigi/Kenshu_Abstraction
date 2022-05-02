#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl/client.h>

#include "../../Utility/Math/Vector.h"

class Input
{
public:

	Input();
	~Input();

	enum class MouseButton
	{
		left,
		right,
		middle,
	};

	void Update();

	bool IsMousePressed(MouseButton mouseButton)const;
	bool IsMouseDown(MouseButton mouseButton)const;

	Float3 GetMouseMove();

private:

	DIMOUSESTATE m_mouseState = {};
	DIMOUSESTATE m_prevFrameMouseState = {};

	Microsoft::WRL::ComPtr<IDirectInputDevice8> m_device;
	Microsoft::WRL::ComPtr<IDirectInput8> m_InputInterface;
};