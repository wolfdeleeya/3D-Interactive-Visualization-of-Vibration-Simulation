#pragma once

#include "implot.h"
#include "implot_internal.h"
#include "graph_data.h"


namespace MyImPlot {
	void PlotBarGroups(const GraphData& gd);

	void PlotBarGroups(const char* const label_ids[], const int* values, int items, int groups, double width, const glm::vec3* item_colors);

	void PlotBars(const char* label_id, const int* values, int count, ImU32 color, double bar_width = 0.67, double x0 = 0, int offset = 0, int stride = sizeof(int));
}