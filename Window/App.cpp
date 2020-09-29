#include "App.h"
#include "Math.h"

float speed_factor = 1.0f;

void App::Frame(POINT p)
{
	auto dt = sin(timer.Peek()) / 2.0f + 0.5f;
	auto sec = timer.Peek() * uwu;




	wnd.gfx().StartFrame(0.1f, 0.1f, 0.2f);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (ImGui::Begin("Mos Pos"))
	{
		ImGui::Text("(%d, %d)", p.x, p.y);
		ImGui::Text("(%.1f, %.1f)", view_x(p.x), view_y(p.y));
		ImGui::SliderFloat("speed", &uwu, 0.0f, 2.0f);
		ImGui::SliderFloat("height", &owo, -10.0f, 10.0f);
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());



		
	wnd.gfx().Cube(sec, owo);

	wnd.gfx().Line(Vec2(0.0f, 0.0f), Vec2(p.x, p.y));






	wnd.gfx().EndFrame();
}