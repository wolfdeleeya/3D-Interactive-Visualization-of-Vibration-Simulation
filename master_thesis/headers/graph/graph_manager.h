#pragma once

#include "application_model.h"
#include "engine_model.h"
#include "graph_data.h"

class GraphManager {

	typedef std::function<void(const GraphData&, int)> render_plot_function;
	typedef std::function<void(void)> comparison_plot_function;
	typedef std::function<void(void)> colormap_legend_function;

private:
	ApplicationModel* m_application_model;
	EngineModel* m_engine_model;

	GraphData m_graph_data;
	GraphData m_cached_relative_graph_data;

	std::vector<render_plot_function> m_render_plot_functions;
	std::vector <comparison_plot_function> m_comparison_plot_functions;
	std::vector<colormap_legend_function> m_colormap_legend_functions;

	//draw bar graph of the selected cell, if cell_index is equal to -1, draw graphs for every cell on the current plot
	void draw_bar_graph(const GraphData& gd, int cell_index = -1);

	//draw line graph of the selected cell, if cell_index is equal to -1, draw graphs for every cell on the current plot
	void draw_line_graph(const GraphData& gd, int cell_index = -1);

	void draw_default_comparison();

	void draw_subplot_comparison();

	void draw_relative_comparison();

	void draw_limits_mode_colormap_legend();

	void draw_normal_mode_colormap_legend();
public:
	GraphManager(ApplicationModel* application_model, EngineModel* engine_model);

	void update_cell_plot();

	void update_relative_plot();

	void draw_cell_plot();

	void draw_legend();
};