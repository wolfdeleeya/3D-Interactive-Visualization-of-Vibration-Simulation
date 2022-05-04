#include "mesh/engine_cell_selection_mesh.h"

#include <iostream>
#include "data_loading.h"
#include <glm/gtc/type_ptr.hpp>

const char* EngineCellSelectionMesh::CELL_SELECT_VERT_SHADER = "./Shaders/engine_cell_selection_shader.vert";
const char* EngineCellSelectionMesh::CELL_SELECT_FRAG_SHADER = "./Shaders/engine_cell_selection_shader.frag";

void EngineCellSelectionMesh::intialize_framebuffer(const glm::ivec2& framebuffer_dimensions)
{
	m_fbo_dimensions = framebuffer_dimensions;

	int width = m_fbo_dimensions.x;
	int height = m_fbo_dimensions.y;

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO_color);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_RBO_color);

	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RBO_depth);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void EngineCellSelectionMesh::load_model_data()
{
	m_model_data.resize(m_indeces.size());

	for (auto& pair : m_cell_vertices) {
		unsigned int cell_index = pair.first;
		for (unsigned int vert_index : pair.second) {
			glm::vec3 pos = m_vertex_positions[vert_index];

			unsigned int index = m_indeces_map[cell_index][vert_index];

			m_model_data[index] = { pos, cell_index };
		}
	}
}

glm::vec3 EngineCellSelectionMesh::get_cell_color_at_pos(GLint x, GLint y)
{
	glm::vec4 pixel(0);

	GLint true_x = ((float)x / m_window_dimensions.x) * m_fbo_dimensions.x;
	GLint true_y = ((float)y / m_window_dimensions.y) * m_fbo_dimensions.y;

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glReadPixels(true_x, true_y, 1, 1, GL_RGBA, GL_FLOAT, glm::value_ptr(pixel));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return pixel;
}

void EngineCellSelectionMesh::setup_buffers()
{
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cell_select_vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(cell_select_vertex), (void*)offsetof(cell_select_vertex, cell_index));

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EngineCellSelectionMesh::setup_indices()
{
	m_indeces_map.clear();

	unsigned int current_index = 0;
	for (auto& pair : m_cell_vertices) {
		unsigned int cell_index = pair.first;
		for (unsigned int vert_index : pair.second) {
			m_indeces_map[cell_index][vert_index] = current_index++;
		}
	}

	m_indeces = loader::triangulate_cell_indeces(m_cell_vertices, m_indeces_map);
}

void EngineCellSelectionMesh::setup_vertex_data()
{
	if (m_cell_vertices.size() == 0 || m_vertex_positions.size() == 0)
		return;

	load_model_data();

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_model_data.size() * sizeof(cell_select_vertex), &m_model_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

EngineCellSelectionMesh::EngineCellSelectionMesh(const glm::ivec2& window_dimensions, const glm::ivec2& framebuffer_dimensions):
	AbstractMesh(CELL_SELECT_VERT_SHADER, CELL_SELECT_FRAG_SHADER, window_dimensions)
{
	glGenFramebuffers(1, &m_FBO);

	glGenRenderbuffers(1, &m_RBO_color);
	glGenRenderbuffers(1, &m_RBO_depth);

	setup_buffers();
	intialize_framebuffer(framebuffer_dimensions);
}

EngineCellSelectionMesh::~EngineCellSelectionMesh()
{
	glDeleteFramebuffers(1, &m_FBO);
	glDeleteRenderbuffers(1, &m_RBO_color);
	glDeleteRenderbuffers(1, &m_RBO_depth);
}

void EngineCellSelectionMesh::render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glClearColor(0, 0, 0, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	glViewport(0, 0, m_fbo_dimensions.x, m_fbo_dimensions.y);
	
	m_shader.use();
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_indeces.size(), GL_UNSIGNED_INT, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int EngineCellSelectionMesh::get_index_at_pos(GLint x, GLint y)
{
	glm::vec3 color = get_cell_color_at_pos(x, y);

	int red_factor = round(color.r * 255) * 256 * 256;
	int green_factor = round(color.g * 255) * 256;
	int blue_factor = round(color.b * 255);

	return red_factor + green_factor + blue_factor;
}