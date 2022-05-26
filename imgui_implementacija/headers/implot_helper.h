#pragma once

#include "implot.h"
#include "implot_internal.h"
#include "graph_data.h"

namespace MyImPlot {

	void PlotBarGroups(const GraphData& gd);
	void PlotLine(const GraphData& gd, unsigned int line_index);
}