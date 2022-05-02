#include "Window.h"

#include "MenuResource.h"
#include "../Application/Application.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Application app;

    switch (msg)
    {
    case WM_CREATE:
    {
        HMENU menuBar  = CreateMenu();
        HMENU menu     = CreateMenu();

        AppendMenu(menuBar, MF_POPUP,  (UINT_PTR)menu, L"API");
        AppendMenu(menu,    MF_STRING, IDR_D3D11, L"DirectX11");
        AppendMenu(menu,    MF_STRING, IDR_D3D12, L"DirectX12");

        SetMenu(hwnd, menuBar);
    }
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDR_D3D11)
        {
            OutputDebugStringA("D3D11\n");
            Application::ChangeAPI(Application::APIType::D3D11);
            break;
        }

        if (LOWORD(wParam) == IDR_D3D12)
        {
            OutputDebugStringA("D3D12\n");
            Application::ChangeAPI(Application::APIType::D3D12);
            break;
        }
        break;

    case WM_DESTROY:

        PostQuitMessage(0);
        break;

    default:

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0L;
}

Window::Window()
{
}

Window::~Window()
{
}

bool Window::Create(const Int2& windowSize)
{
    m_windowSize = windowSize;

    HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(0);

    if (!hInstance)
    {
        return false;
    }

    WNDCLASS wndClass = {};
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = nullptr;
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = nullptr;
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = m_windowName;

    if (!RegisterClass(&wndClass))
    {
        return false;
    }

    RECT rect = { 0L, 0L, (LONG)windowSize.x, (LONG)windowSize.y };

    long windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;

    AdjustWindowRect(&rect, windowStyle, false);

    m_hwnd = CreateWindow(m_windowName, m_windowName, windowStyle, CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

    if (!m_hwnd)
    {
        return false;
    }

    ShowWindow(m_hwnd, SW_SHOW);

    return true;
}

bool Window::Update()
{
    MSG msg = {};

    if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (msg.message == WM_QUIT)
    {
        DestroyWindow(m_hwnd);
        return false;
    }

    return true;
}

void Window::SetWindowName(const wchar_t* windowName)
{
    m_windowName = windowName;
}

const HWND& Window::GetWindowHandle() const
{
    return m_hwnd;
}

const Int2& Window::GetWindowSize() const
{
    return m_windowSize;
}