#include "mesh/engine_lines_mesh.h"

const char* EngineLineMesh::VERTEX_SHADER = "./Shaders/line_shader.vert";
const char* EngineLineMesh::FRAGMENT_SHADER = "./Shaders/line_shader.frag";

void EngineLineMesh::setup_buffers()
{
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0); 

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EngineLineMesh::setup_indices()
{
	m_indeces_map.clear();

	unsigned int current_index = 0;
	
	for (auto& pair : m_cell_vertices) {
		std::vector<unsigned int>& vertices = pair.second;
		for (unsigned int index : vertices)
			if (m_indeces_map.find(index) == m_indeces_map.end())
				m_indeces_map[index] = current_index++;
	}

	m_indeces = data::create_line_faces(m_cell_vertices, m_indeces_map);
}

void EngineLineMesh::setup_vertex_data()
{
	if (m_cell_vertices.size() == 0 || m_vertex_positions.size() == 0)
		return;

	load_model_data();

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_model_data.size() * sizeof(glm::vec3), &m_model_data[0], GL_STATIC_DRAW); 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void EngineLineMesh::load_model_data()
{
	m_model_data.resize(m_vertex_positions.size());

	for (auto& pair : m_vertex_positions) {
		unsigned int index = m_indeces_map[pair.first];
		m_model_data[index] = pair.second;
	}
}

EngineLineMesh::EngineLineMesh(const glm::ivec2& window_dimensions, unsigned int target_FBO) : AbstractMesh(VERTEX_SHADER, FRAGMENT_SHADER, window_dimensions) 
{
	m_target_FBO = target_FBO;
	setup_buffers(); 
}

void EngineLineMesh::render()
{
	if (is_empty())
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, m_target_FBO);

	glViewport(0, 0, m_window_dimensions.x, m_window_dimensions.y);

	m_shader.use(); 

	glBindVertexArray(m_VAO);
	glDrawElements(GL_LINES, m_indeces.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}