#include "implot_helper.h"
#include "implot.h"
#include "implot_internal.h"


template IMPLOT_API void ImPlot::PlotBarGroupsCustomColors<double, glm::vec<3, float>>
(const char* const label_ids[], const double* values, const glm::vec3* colors, int items,
    int groups, double width, double shift, ImPlotBarGroupsFlags flags);

void MyImPlot::PlotBarGroups(const GraphData& gd)
{
    ImPlot::PlotBarGroupsCustomColors(&gd.item_labels[0], &gd.plot_data[0], &gd.colors[0], gd.item_labels.size(), gd.group_labels.size(), gd.size);
}


void MyImPlot::PlotLine(const GraphData& gd, unsigned int line_index)
{
}