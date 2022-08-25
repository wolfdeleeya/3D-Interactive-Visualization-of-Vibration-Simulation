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
	const std::map<unsigned int, std::vector<unsigned int>>& cell_vertices = m_engine_model->cell_vertices();

	m_indeces_map.clear();

	unsigned int current_index = 0;

	for (auto& pair : cell_vertices) {
		unsigned int cell_index = pair.first;
		for (unsigned int vert_index : pair.second) {
			m_indeces_map[cell_index][vert_index] = current_index++;
		}
	}

	m_indeces = data::triangulate_cell_indeces(cell_vertices, m_indeces_map);
}

void EngineVisualizationMesh::setup_vertex_data()
{
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
	const std::map<unsigned int, glm::vec3>& vertex_positions = m_engine_model->vertex_positions();
	const std::map<unsigned int, std::vector<unsigned int>>& cell_vertices = m_engine_model->cell_vertices();
	m_model_data.resize(m_indeces.size());

	for (const auto& pair : cell_vertices) {
		unsigned int cell_index = pair.first;

		for (unsigned int vert_index : pair.second) {
			glm::vec3 pos = vertex_positions.at(vert_index);
			unsigned int index = m_indeces_map[cell_index][vert_index];

			m_model_data[index] = { pos, cell_index };
		}
	}
}

void EngineVisualizationMesh::load_color_data()
{
	const std::map<unsigned int, std::vector<unsigned int>>& cell_vertices = m_engine_model->cell_vertices();
	const std::map<unsigned int, glm::vec3>& color_map = m_color_calculator.current_color_map();

	if (m_color_data.size() != m_indeces.size())
		m_color_data.resize(m_indeces.size());

	for (const auto& pair : cell_vertices) {
		unsigned int cell_index = pair.first;

		for (unsigned int vert_index : pair.second) {
			glm::vec3 color = color_map.at(cell_index);

			unsigned int index = m_indeces_map[cell_index][vert_index];

			m_color_data[index] = color;
		}
	}
}

EngineVisualizationMesh::EngineVisualizationMesh(EngineModel* engine_model, const glm::ivec2& window_dimensions, unsigned int target_FBO) :
	AbstractEngineMesh(VERTEX_SHADER, FRAGMENT_SHADER, engine_model, window_dimensions), m_color_calculator(engine_model)
{
	m_engine_model = engine_model;

	m_color_calculator.on_colors_recalculated.add_member_listener(&EngineVisualizationMesh::setup_color_data, this);

	glGenBuffers(1, &m_VBO_color);

	m_target_FBO = target_FBO;

	setup_buffers();
}

EngineVisualizationMesh::~EngineVisualizationMesh()
{
	glDeleteBuffers(1, &m_VBO_color);
}

void EngineVisualizationMesh::render()
{
	if (!m_engine_model->is_model_data_loaded())
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, m_target_FBO);

	glViewport(0, 0, m_window_dimensions.x, m_window_dimensions.y);

	m_shader.use();

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_indeces.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}