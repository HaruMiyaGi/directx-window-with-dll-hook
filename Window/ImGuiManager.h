#pragma once
#include "imgui\imgui.h"

class ImguiManager
{
public:
	ImguiManager()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
	}

	~ImguiManager()
	{
		ImGui::DestroyContext();
	}
};
