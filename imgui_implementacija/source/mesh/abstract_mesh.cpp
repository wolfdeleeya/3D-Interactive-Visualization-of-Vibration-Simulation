#include "mesh/abstract_mesh.h"
#include "data_loading.h"

const char* AbstractMesh::MODEL_PAR_NAME = "model";
const char* AbstractMesh::VIEW_PAR_NAME = "view";
const char* AbstractMesh::PROJECTION_PAR_NAME = "projection";

AbstractMesh::AbstractMesh(const char* vertex_shader_dest, const char* fragment_shader_dest):
	m_shader(vertex_shader_dest, fragment_shader_dest)
{
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
	glGenVertexArrays(1, &m_VAO);
}

AbstractMesh::~AbstractMesh()
{
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
	glDeleteVertexArrays(1, &m_VAO);

	m_shader.delete_shader();
}

void AbstractMesh::load_cell_vertices(const char* path)
{
	m_cell_vertices = loader::load_cells(path);

	setup_indices();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indeces.size() * sizeof(unsigned int), &m_indeces[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	setup_vertex_data();
}

void AbstractMesh::load_vertex_positions(const char* path)
{
	m_vertex_positions = loader::load_vertices(path);

	setup_vertex_data();
}