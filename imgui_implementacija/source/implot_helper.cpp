#include "implot_helper.h"
#include "implot.h"
#include "implot_internal.h"


template IMPLOT_API void ImPlot::PlotBarGroupsCustomColors<double, glm::vec<3, float>>
(const char* const label_ids[], const double* values, const glm::vec3* colors, int items,
    int groups, double width, double shift, ImPlotBarGroupsFlags flags);

void MyImPlot::PlotBarGroups(const GraphData& gd, float bar_width)
{
    ImPlot::PlotBarGroupsCustomColors(&gd.item_labels[0], &gd.plot_data[0], &gd.colors[0], gd.item_labels.size(), gd.group_labels.size(), bar_width);
}


void MyImPlot::PlotLine(const GraphData& gd, unsigned int line_index)
{
	unsigned int num_of_groups = gd.group_labels.size();
	const glm::vec3& color = gd.colors[line_index];
	ImU32 packed_color = ImGui::GetColorU32({ color.r, color.g, color.b, 1 });
	ImPlot::PlotLineCustomColor(gd.item_labels[line_index], &gd.positions[0], &gd.plot_data[0 + line_index * num_of_groups], packed_color, num_of_groups);
}