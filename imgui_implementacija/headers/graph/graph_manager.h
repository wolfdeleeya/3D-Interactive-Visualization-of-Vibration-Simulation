#pragma once

#include "application_model.h"
#include "engine_data.h"
#include "graph_data.h"
#include "graph_settings.h"

class GraphManager {
	enum class RenderMode { BARS, LINES, END };
	enum class ComparisonMode { DEFAULT, SUBPLOTS, RELATIVE, END };

	typedef std::function<void(const GraphData&, int)> render_plot_function;
	typedef std::function<void(void)> comparison_plot_function;

private:
	ApplicationModel* m_application_model;
	EngineData* m_engine_data;

	GraphData m_graph_data;
	GraphData m_cached_relative_graph_data;

	RenderMode m_current_render_mode;
	ComparisonMode m_current_comparison_mode;

	std::vector<BaseGraphSettings*> m_render_graph_settings;
	std::vector<BaseGraphSettings*> m_comparison_graph_settings;

	std::vector<render_plot_function> m_render_plot_functions;
	std::vector <comparison_plot_function> m_comparison_plot_functions;

	glm::vec3 m_hovered_cell_graph_color;

	unsigned int m_current_referent_cell_index;

	static const std::vector<const char*> RENDER_MODE_LABELS;
	static const std::vector<const char*> COMPARISON_MODE_LABELS;

	//draw bar graph of the selected cell, if cell_index is equal to -1, draw graphs for every cell on the current plot
	void draw_bar_graph(const GraphData& gd, int cell_index = -1);

	//draw line graph of the selected cell, if cell_index is equal to -1, draw graphs for every cell on the current plot
	void draw_line_graph(const GraphData& gd, int cell_index = -1);

	void draw_default_comparison();

	void draw_subplot_comparison();

	void draw_relative_comparison();


	void set_render_mode(RenderMode mode) { m_current_render_mode = mode; }

	void set_comparison_mode(ComparisonMode mode) { m_current_comparison_mode = mode; }
public:

	GraphManager(ApplicationModel* application_model, EngineData* engine_data);

	~GraphManager();

	void update_cell_plot();		//for now on each plot change, we call this function

	void update_relative_plot();

	void referant_cell_changed(unsigned int new_referant_cell_index);

	void update_legend();

	void draw_cell_plot();

	void draw_legend();

	void switch_render_mode();

	void switch_comparison_mode();

	void draw_graph_settings();

	void ShowDemo_FilledLinePlots();
};