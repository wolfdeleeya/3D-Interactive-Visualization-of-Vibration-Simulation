#pragma once

#include "abstract_mesh.h"

struct cell_select_vertex {
	glm::vec3 position;
	unsigned int cell_index;
};

class EngineCellSelectionMesh: public AbstractMesh {
private:
	unsigned int m_FBO;
	unsigned int m_RBO_color, m_RBO_depth;

	const static char* CELL_SELECT_VERT_SHADER;
	const static char* CELL_SELECT_FRAG_SHADER;

	std::vector<cell_select_vertex> m_model_data;

	std::map<unsigned int, std::map<unsigned int, unsigned int>> m_indeces_map;

	glm::ivec2 m_fbo_dimensions;

	void intialize_framebuffer(const glm::ivec2& framebuffer_dimensions);

	void load_model_data();

	glm::vec3 get_cell_color_at_pos(GLint x, GLint y);
protected:
	virtual void setup_buffers() override;

	virtual void setup_indices() override;
	
	virtual void setup_vertex_data() override;

public:
	EngineCellSelectionMesh(const glm::ivec2& window_dimensions, const glm::ivec2& framebuffer_dimensions);

	virtual ~EngineCellSelectionMesh() override;

	virtual void render() override;

	int get_index_at_pos(GLint x, GLint y);
};