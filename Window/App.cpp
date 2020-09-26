#include "App.h"

void App::Frame()
{
	auto dt = sin(timer.Peek()) / 2.0f + 0.5f;
	auto sec = timer.Peek();

	wnd.gfx().StartFrame(0.1f, 0.05f, 0.15f);
	wnd.gfx().Cube(sec);
	wnd.gfx().EndFrame();
}