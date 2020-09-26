#include "App.h"
Window::WindowClass Window::WindowClass::wndClass;

int WINAPI WinMain(HINSTANCE _In_ hInstance, HINSTANCE _In_opt_ hPrevInstance, LPSTR _In_ lpCmdLine, int _In_ nCmdShow)
{
	return App{}.Go();
}