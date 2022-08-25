#include "mesh/abstract_engine_mesh.h"

AbstractEngineMesh::AbstractEngineMesh(const char* vertex_shader_dest, const char* fragment_shader_dest, EngineModel* engine_model, const glm::ivec2& window_dimensions):
	AbstractMesh(vertex_shader_dest, fragment_shader_dest, window_dimensions)
{
	m_engine_model = engine_model;

	m_engine_model->on_vertex_positions_loaded.add_member_listener(&AbstractEngineMesh::setup_vertex_positions, this);
	m_engine_model->on_cell_vertices_loaded.add_member_listener(&AbstractEngineMesh::setup_cell_vertices, this);

	glGenBuffers(1, &m_EBO);
}

AbstractEngineMesh::~AbstractEngineMesh()
{
	glDeleteBuffers(1, &m_EBO);
}

void AbstractEngineMesh::setup_cell_vertices()
{
	setup_indices();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indeces.size() * sizeof(unsigned int), &m_indeces[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (m_engine_model->is_model_data_loaded())
		setup_vertex_data();
}

void AbstractEngineMesh::setup_vertex_positions()
{
	if (m_engine_model->is_model_data_loaded())
		setup_vertex_data();
}
