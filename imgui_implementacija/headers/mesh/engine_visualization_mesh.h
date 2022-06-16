#pragma once

#include "abstract_engine_mesh.h"
#include "data_loading.h"
#include "engine_model.h"

struct vertex {
	glm::vec3 position;
	unsigned int cell_index;
};

class EngineVisualizationMesh : public AbstractEngineMesh {
private:
	static const char* VERTEX_SHADER, * FRAGMENT_SHADER;

	const static char* SELECTED_CELL_PAR_NAME;

	EngineModel* m_engine_model;

	unsigned int m_VBO_color;

	unsigned int m_target_FBO;

	std::vector<vertex> m_model_data;
	std::vector<glm::vec3> m_color_data;

	std::map<unsigned int, std::map<unsigned int, unsigned int>> m_indeces_map;

	virtual void setup_buffers() override;

	virtual void setup_indices() override;
	
	virtual void setup_vertex_data() override;

	void setup_color_data();

	void load_model_data();

	void load_color_data();
public:
	EngineVisualizationMesh(EngineModel* engine_model, const glm::ivec2& window_dimensions, unsigned int m_target_FBO = 0);
	
	virtual ~EngineVisualizationMesh() override;

	virtual void render() override;

	void on_colors_updated() { setup_color_data(); }

	void set_target_FBO(unsigned int target_FBO) { m_target_FBO = target_FBO; }

	void cell_hovered(unsigned int cell_index) { m_shader.set_value(SELECTED_CELL_PAR_NAME, cell_index); }
};