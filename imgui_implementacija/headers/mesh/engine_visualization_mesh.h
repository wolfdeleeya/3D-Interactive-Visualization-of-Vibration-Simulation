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
	unsigned int m_VBO_color;

	glm::ivec2 m_window_dimensions;

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
	EngineVisualizationMesh(const char* vertex_shader_dest, const char* fragment_shader_dest, 
		const glm::ivec2& window_dimensions, bool is_cw = false);
	
	virtual ~EngineVisualizationMesh() override;

	virtual void render() override;

	virtual void update_window_size(const glm::ivec2& window_dimensions) override;
};