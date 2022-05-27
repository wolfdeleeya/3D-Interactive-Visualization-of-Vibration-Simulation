#include "graph/graph_settings.h"
#include "imgui.h"

void GroupBarGraphSettings::draw()
{
	ImGui::DragFloat("Bar Width", &bar_width, 0.05, 0, 1);
}