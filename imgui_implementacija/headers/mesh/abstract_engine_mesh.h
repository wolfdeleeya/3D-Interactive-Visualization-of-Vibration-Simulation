#pragma once

#include "abstract_mesh.h"

class AbstractEngineMesh : public AbstractMesh {
protected:
	unsigned int m_EBO;

	std::map<unsigned int, std::vector<unsigned int>> m_cell_vertices;
	std::map<unsigned int, glm::vec3> m_vertex_positions;

	std::vector<unsigned int> m_indeces;

	virtual void setup_indices() = 0;

public:
	AbstractEngineMesh(const char* vertex_shader_dest, const char* fragment_shader_dest, const glm::ivec2& window_dimensions);

	virtual ~AbstractEngineMesh();

	void set_cell_vertices(const std::map<unsigned int, std::vector<unsigned int>>& cell_vertices);

	void set_vertex_positions(const std::map<unsigned int, glm::vec3>& vertex_positions);

	bool is_empty() { return m_cell_vertices.size() == 0 || m_vertex_positions.size() == 0; }
};