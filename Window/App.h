#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImGuiManager.h"

class App
{
public:
	App()
		: wnd(640, 480, "uwu")
	{}
	int Go()
	{

		while (true)
		{
			if (GetKeyState(VK_F1) && toggle)
			{
				lib = LoadLibrary("Hook.dll");
				toggle = false;
			}

			POINT p;
			GetCursorPos(&p);
			ScreenToClient(wnd.GetHWND(), &p);

			//if (GetKeyState(VK_F3) && !toggle)
			//{
			//	FreeLibrary(lib);
			//	toggle = true;
			//}

			if (const auto ecode = Window::ProcessMessages())
				return *ecode;
			Frame(p);
		}
	}
private:
	void Frame(POINT p);
private:
	Timer timer;
	ImguiManager imgui;
	Window wnd;
	HMODULE lib = 0;
	bool toggle = true;
	float uwu = 1.0f;
	float owo = 0.0f;
};