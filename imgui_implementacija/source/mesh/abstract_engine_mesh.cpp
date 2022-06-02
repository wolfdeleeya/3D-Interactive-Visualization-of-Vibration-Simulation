#include "mesh/abstract_engine_mesh.h"

AbstractEngineMesh::AbstractEngineMesh(const char* vertex_shader_dest, const char* fragment_shader_dest, const glm::ivec2& window_dimensions):
	AbstractMesh(vertex_shader_dest, fragment_shader_dest, window_dimensions)
{
	glGenBuffers(1, &m_EBO);
}

AbstractEngineMesh::~AbstractEngineMesh()
{
	glDeleteBuffers(1, &m_EBO);
}

void AbstractEngineMesh::set_cell_vertices(const std::map<unsigned int, std::vector<unsigned int>>& cell_vertices)
{
	m_cell_vertices = cell_vertices;

	setup_indices();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indeces.size() * sizeof(unsigned int), &m_indeces[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (!is_empty())
		setup_vertex_data();
}

void AbstractEngineMesh::set_vertex_positions(const std::map<unsigned int, glm::vec3>& vertex_positions)
{
	m_vertex_positions = vertex_positions;

	if (!is_empty())
		setup_vertex_data();
}
