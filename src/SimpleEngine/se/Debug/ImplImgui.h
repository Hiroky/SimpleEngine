#pragma once 

#include "se/Graphics/Graphics.h"
#include "thirdparty/imgui/imgui.h"


namespace se
{
	bool ImGuiInitialize();
	void ImGuiShutdown();
	void ImGuiNewFrame();
	void ImGuiRender();
}