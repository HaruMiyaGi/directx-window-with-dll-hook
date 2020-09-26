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
			if (const auto ecode = Window::ProcessMessages())
				return *ecode;
			Frame();
		}
	}
private:
	void Frame()
	{
		auto dt = sin(timer.Peek()) / 2.0f + 0.5f;

		wnd.gfx().StartFrame(dt, 0.0f, 0.0f);
		wnd.gfx().EndFrame();
	}
private:
	Timer timer;
	Window wnd;
};