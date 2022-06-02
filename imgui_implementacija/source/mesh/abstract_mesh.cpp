#include "mesh/abstract_mesh.h"
#include "data_loading.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>

const char* AbstractMesh::MODEL_PAR_NAME = "model";
const char* AbstractMesh::VIEW_PAR_NAME = "view";
const char* AbstractMesh::PROJECTION_PAR_NAME = "projection";

AbstractMesh::AbstractMesh(const char* vertex_shader_dest, const char* fragment_shader_dest, const glm::ivec2& window_dimensions):
	m_shader(vertex_shader_dest, fragment_shader_dest)
{
	glGenBuffers(1, &m_VBO);
	glGenVertexArrays(1, &m_VAO);

	update_window_size(window_dimensions);
}

AbstractMesh::~AbstractMesh()
{
	glDeleteBuffers(1, &m_VBO);
	glDeleteVertexArrays(1, &m_VAO);

	m_shader.delete_shader();
}

void AbstractMesh::update_window_size(const glm::ivec2& window_dimensions)
{
	m_window_dimensions = window_dimensions;
	set_projection(glm::perspective(45.f, (float)window_dimensions.x / window_dimensions.y, 0.1f, 100.f));
}