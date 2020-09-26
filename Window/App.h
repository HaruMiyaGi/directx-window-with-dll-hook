#pragma once
#include "Window.h"
#include "Timer.h"

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
			if (toggle && GetKeyState(VK_F1) & 1)
			{
				LoadLibrary("Hook.dll");
				toggle = false;
			}

			if (const auto ecode = Window::ProcessMessages())
				return *ecode;
			Frame();
		}
	}
private:
	void Frame();
private:
	Timer timer;
	Window wnd;
	bool toggle = true;
};