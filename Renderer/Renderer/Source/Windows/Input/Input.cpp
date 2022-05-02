#include "Input.h"

#include <cassert>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Input::Input()
{
	HRESULT result = S_OK;

	result = DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, &m_InputInterface, nullptr);
	assert(SUCCEEDED(result));

	result = m_InputInterface->CreateDevice(GUID_SysMouse, &m_device, nullptr);
	assert(SUCCEEDED(result));

	result = m_device->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));

	m_device->Acquire();
	m_device->Poll();
}

Input::~Input()
{
	m_device->Unacquire();
}

void Input::Update()
{
	m_prevFrameMouseState = m_mouseState;

	m_device->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseState);
}

bool Input::IsMousePressed(MouseButton mouseButton) const
{
	unsigned int mouseIndex = (int)mouseButton;

	return m_mouseState.rgbButtons[mouseIndex] & 0x80;
}

bool Input::IsMouseDown(MouseButton mouseButton) const
{
	unsigned int mouseIndex = (int)mouseButton;

	return (m_mouseState.rgbButtons[mouseIndex] & 0x80) && !(m_prevFrameMouseState.rgbButtons[mouseIndex] & 0x80);
}

Float3 Input::GetMouseMove()
{
	return Float3((float)m_mouseState.lX, (float)m_mouseState.lY, (float)m_mouseState.lZ);
}
