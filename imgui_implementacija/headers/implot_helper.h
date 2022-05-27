#pragma once

#include "implot.h"
#include "implot_internal.h"
#include "graph/graph_data.h"

namespace MyImPlot {
	void PlotBarGroups(const GraphData& gd, float bar_width);
	void PlotBars(const GraphData& gd, unsigned int index);
	void PlotLine(const GraphData& gd, unsigned int line_index);
}