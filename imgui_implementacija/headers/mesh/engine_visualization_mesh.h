#pragma once

#include "abstract_mesh.h"
#include "data_loading.h"

struct vertex {
	glm::vec3 position;
	glm::vec3 normal;
	unsigned int cell_index;
};

class EngineVisualizationMesh : public AbstractMesh {
private:
	static const char* VERTEX_SHADER, * FRAGMENT_SHADER;

	const static char* SELECTED_CELL_PAR_NAME;

	unsigned int m_VBO_color;

	unsigned int m_target_FBO;

	std::vector<vertex> m_model_data;
	std::vector<glm::vec3> m_color_data;

	std::map<unsigned int, glm::vec3> m_cell_colors_map;
	std::map<unsigned int, glm::vec3> m_cell_normals;

	std::map<unsigned int, std::map<unsigned int, unsigned int>> m_indeces_map;

	bool m_is_cw;

	virtual void setup_buffers() override;

	virtual void setup_indices() override;
	
	virtual void setup_vertex_data() override;

	void setup_color_data();

	void calculate_normals();

	void load_model_data();

	void load_color_data();
public:
	EngineVisualizationMesh(const glm::ivec2& window_dimensions, unsigned int m_target_FBO = 0, bool is_cw = false);
	
	virtual ~EngineVisualizationMesh() override;

	void set_colors(const std::map<unsigned int, glm::vec3>& cell_colors_map);

	virtual void render() override;

	void set_target_FBO(unsigned int target_FBO) { m_target_FBO = target_FBO; }

	void on_cell_selected(unsigned int cell_index) { m_shader.set_value(SELECTED_CELL_PAR_NAME, cell_index); }
};