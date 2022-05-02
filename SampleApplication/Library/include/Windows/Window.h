#pragma once

#include <Windows.h>
#include "../Utility/Math/Vector.h"



class Window
{
public:

	Window();
	~Window();

	bool Create(const Int2& windowSize);

	bool Update();

	void SetWindowName(const wchar_t* windowName);

	const HWND& GetWindowHandle()const;
	const Int2& GetWindowSize()const;



private:

	HWND m_hwnd = nullptr;

	const wchar_t* m_windowName = L"DirectX";

	Int2 m_windowSize;
};