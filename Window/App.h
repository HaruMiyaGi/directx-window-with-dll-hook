#pragma once
#include "Window.h"

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
		wnd.gfx().StartFrame(1.0f, 0.0f, 0.0f);
		int i = 0;
		wnd.gfx().EndFrame();
	}
private:
	Window wnd;
};