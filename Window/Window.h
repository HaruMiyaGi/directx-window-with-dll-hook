#pragma once
#include <Windows.h>
#include <optional>
#include "Graphics.h"
#include "imgui\imgui_impl_win32.h"

class Window
{
public:
	class WindowClass
	{
	public:
		static const char* GetName()
		{
			return wndClassName;
		}
		static HINSTANCE GetInstance()
		{
			return wndClass.hInst;
		}
	private:
		WindowClass()
			: hInst(GetModuleHandle(nullptr))
		{
			WNDCLASSEX wc = { 0 };
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = HandleMsgSetup;
			wc.hInstance = GetInstance();
			wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
			wc.lpszClassName = GetName();
			RegisterClassEx(&wc);
		}
		~WindowClass()
		{
			UnregisterClass(GetName(), GetInstance());
		}
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator = (const WindowClass&) = delete;
	private:
		static constexpr const char* wndClassName = "class";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const char* title)
		: width(width), height(height)
	{
		RECT rect = { 0, 0, width, height };
		AdjustWindowRectEx(&rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE, 0);

		hWnd = CreateWindowEx(NULL, WindowClass::GetName(), title,
			WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
			CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
			NULL, NULL, WindowClass::GetInstance(), this);

		ShowWindow(hWnd, SW_SHOWDEFAULT);

		ImGui_ImplWin32_Init(hWnd);

		pGfx = std::make_unique<Graphics>(hWnd, width, height);
	}
	Graphics& gfx()
	{
		return *pGfx;
	}
	~Window()
	{
		DestroyWindow(hWnd);
	}
	Window(const Window&) = delete;
	Window& operator = (const Window&) = delete;
	static std::optional<int> ProcessMessages()
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return msg.wParam;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return {};
	}
	HWND GetHWND()
	{
		return hWnd;
	}
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_NCCREATE)
		{
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
			return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		switch (msg)
		{
		case WM_CLOSE:
		{
			PostQuitMessage(69);
		} break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
private:
	int width;
	int height;
	HWND hWnd;
public:
	std::unique_ptr<Graphics> pGfx;
};