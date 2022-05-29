#include "implot_helper.h"
#include "implot.h"
#include "implot_internal.h"

void MyImPlot::PlotBarGroups(const GraphData& gd, float bar_width)
{
	std::vector<ImU32> colors;//quick-fix ->TODO: REPLACE glm::vec3 for colors in GraphData with ImU32
	for (const auto& c : gd.colors)
		colors.push_back(ImGui::GetColorU32({ c.r, c.g, c.b, 1 }));
    ImPlot::PlotBarGroupsCustomColors(&gd.item_labels[0], &gd.plot_data[0], &colors[0], gd.item_labels.size(), gd.group_labels.size(), bar_width);
}

void MyImPlot::PlotBars(const GraphData& gd, unsigned int index, float bar_width)
{
	unsigned int num_of_groups = gd.group_labels.size();
	const glm::vec3& color = gd.colors[index];
	ImU32 packed_color = ImGui::GetColorU32({ color.r, color.g, color.b, 1 });
	ImPlot::PlotBarsCustomColor(gd.item_labels[index], &gd.plot_data[0 + index * num_of_groups], packed_color, num_of_groups, bar_width);
}


void MyImPlot::PlotLine(const GraphData& gd, unsigned int line_index)
{
	unsigned int num_of_groups = gd.group_labels.size();
	const glm::vec3& color = gd.colors[line_index];
	ImU32 packed_color = ImGui::GetColorU32({ color.r, color.g, color.b, 1 });
	ImPlot::PlotLineCustomColor(gd.item_labels[line_index], &gd.positions[0], &gd.plot_data[0 + line_index * num_of_groups], packed_color, num_of_groups);
}