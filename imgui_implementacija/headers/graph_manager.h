#pragma once

#include "application_model.h"
#include "engine_data.h"
#include "graph_data.h"

class GraphManager {
private:
	ApplicationModel* m_application_model;
	EngineData* m_engine_data;
	GraphData m_graph_data;
	glm::vec3 m_hovered_cell_graph_color;

public:

	GraphManager(ApplicationModel* application_model, EngineData* engine_data);

	void update_cell_plot();		//for now on each plot change, we call this function

	void update_legend();

	void draw_cell_plot();

	void draw_legend();
};