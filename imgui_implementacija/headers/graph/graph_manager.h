#pragma once

#include "application_model.h"
#include "engine_data.h"
#include "graph_data.h"
#include "graph_settings.h"

class GraphManager {
	enum class RenderMode { BARS, LINES };

private:
	ApplicationModel* m_application_model;
	EngineData* m_engine_data;

	GraphData m_graph_data;
	GraphData m_cached_relative_graph_data;

	RenderMode m_current_render_mode;

	std::vector<BaseGraphSettings*> m_graph_settings;

	glm::vec3 m_hovered_cell_graph_color;

	bool m_is_drawing_on_separate_plots;

	void draw_bar_graph();

	void draw_line_graph();

public:

	GraphManager(ApplicationModel* application_model, EngineData* engine_data);

	void update_cell_plot();		//for now on each plot change, we call this function

	void update_legend();

	void draw_cell_plot();

	void draw_legend();

	void draw_graph_settings_widget();

	void ShowDemo_FilledLinePlots();
};