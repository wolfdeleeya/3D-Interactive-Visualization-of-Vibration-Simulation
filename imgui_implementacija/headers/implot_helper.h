#pragma once

#include "implot.h"
#include "implot_internal.h"
#include "graph/graph_data.h"
#include "gradient.h"

namespace MyImPlot {
	void PlotBarGroups(const GraphData& gd, float bar_width);
	void PlotBars(const GraphData& gd, unsigned int index, float bar_width);
	void PlotLine(const GraphData& gd, unsigned int line_index);
	void ColormapScale(const char* label, const Gradient& gradient, double scale_min, double scale_max, unsigned int num_of_color_points, const ImVec2& size, bool is_continuous);
}