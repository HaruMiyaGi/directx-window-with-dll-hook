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
		int i = 0;
	}
private:
	Window wnd;
};