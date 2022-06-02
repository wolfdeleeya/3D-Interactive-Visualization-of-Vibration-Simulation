#include "mesh/engine_visualization_mesh.h"
#include "glm/gtc/matrix_transform.hpp"

const char* EngineVisualizationMesh::VERTEX_SHADER = "./Shaders/engine_shader.vert";
const char* EngineVisualizationMesh::FRAGMENT_SHADER = "./Shaders/engine_shader.frag";

const char* EngineVisualizationMesh::SELECTED_CELL_PAR_NAME = "selected_index";

void EngineVisualizationMesh::setup_buffers()
{
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(vertex), (void*)offsetof(vertex, cell_index));

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_color);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EngineVisualizationMesh::setup_indices()
{
	m_indeces_map.clear();

	unsigned int current_index = 0;

	for (auto& pair : m_cell_vertices) {
		unsigned int cell_index = pair.first;
		for (unsigned int vert_index : pair.second) {
			m_indeces_map[cell_index][vert_index] = current_index++;
		}
	}

	m_indeces = data::triangulate_cell_indeces(m_cell_vertices, m_indeces_map);
}

void EngineVisualizationMesh::setup_vertex_data()
{
	if (m_cell_vertices.size() == 0 || m_vertex_positions.size() == 0)
		return;

	load_model_data();

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_model_data.size() * sizeof(vertex), &m_model_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void EngineVisualizationMesh::setup_color_data()
{
	if (m_indeces.size() == 0)
		return;

	load_color_data();

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_color);
	glBufferData(GL_ARRAY_BUFFER, m_color_data.size() * sizeof(glm::vec3), &m_color_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void EngineVisualizationMesh::load_model_data()
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

void EngineVisualizationMesh::load_color_data()
{
	m_color_data.resize(m_indeces.size());

	for (auto& pair : m_cell_vertices) {
		unsigned int cell_index = pair.first;

		for (unsigned int vert_index : pair.second) {
			glm::vec3 color = m_cell_colors_map[cell_index];

			unsigned int index = m_indeces_map[cell_index][vert_index];

			m_color_data[index] = color;
		}
	}
}

EngineVisualizationMesh::EngineVisualizationMesh(const glm::ivec2& window_dimensions, unsigned int target_FBO) :
	AbstractEngineMesh(VERTEX_SHADER, FRAGMENT_SHADER, window_dimensions)
{
	glGenBuffers(1, &m_VBO_color);

	m_target_FBO = target_FBO;

	setup_buffers();
}

EngineVisualizationMesh::~EngineVisualizationMesh()
{
	glDeleteBuffers(1, &m_VBO_color);
}

void EngineVisualizationMesh::set_colors(const std::map<unsigned int, glm::vec3>& cell_colors_map)
{
	m_cell_colors_map = cell_colors_map;

	setup_color_data();
}

void EngineVisualizationMesh::render()
{
	if (is_empty())
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, m_target_FBO);

	glViewport(0, 0, m_window_dimensions.x, m_window_dimensions.y);

	m_shader.use();

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_indeces.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}