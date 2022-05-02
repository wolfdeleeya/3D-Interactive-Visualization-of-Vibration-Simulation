#include "mesh/abstract_mesh.h"
#include "data_loading.h"
#include <glm/ext/matrix_clip_space.hpp>

const char* AbstractMesh::MODEL_PAR_NAME = "model";
const char* AbstractMesh::VIEW_PAR_NAME = "view";
const char* AbstractMesh::PROJECTION_PAR_NAME = "projection";

AbstractMesh::AbstractMesh(const char* vertex_shader_dest, const char* fragment_shader_dest, const glm::ivec2& window_dimensions):
	m_shader(vertex_shader_dest, fragment_shader_dest)
{
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
	glGenVertexArrays(1, &m_VAO);

	update_window_size(window_dimensions);
}

AbstractMesh::~AbstractMesh()
{
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
	glDeleteVertexArrays(1, &m_VAO);

	m_shader.delete_shader();
}

void AbstractMesh::update_window_size(const glm::ivec2& window_dimensions)
{
	m_window_dimensions = window_dimensions;
	set_projection(glm::perspective(45.f, (float)window_dimensions.x / window_dimensions.y, 0.1f, 100.f));
}

void AbstractMesh::set_cell_vertices(const std::map<unsigned int, std::vector<unsigned int>>& cell_vertices)
{
	m_cell_vertices = cell_vertices;

	setup_indices();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indeces.size() * sizeof(unsigned int), &m_indeces[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	setup_vertex_data();
}

void AbstractMesh::set_vertex_positions(const std::map<unsigned int, glm::vec3>& vertex_positions)
{
	m_vertex_positions = vertex_positions;

	setup_vertex_data();
}