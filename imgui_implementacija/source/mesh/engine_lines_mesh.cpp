#include "mesh/engine_lines_mesh.h"

#include <iostream>

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
	unsigned int current_index = 0;
	for (auto& pair : m_vertex_positions)
		m_indeces_map[pair.first] = current_index++;

	m_indeces = loader::create_line_faces(m_cell_vertices, m_indeces_map);
}

void EngineLineMesh::setup_vertex_data()
{
	if (m_cell_vertices.size() == 0 || m_vertex_positions.size() == 0)
		return;

	load_model_data();

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_model_data.size() * sizeof(m_model_data[0]), &m_model_data[0], GL_STATIC_DRAW); 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void EngineLineMesh::load_model_data()
{
	for (auto& pair : m_vertex_positions)
		m_model_data.push_back(pair.second);
}

EngineLineMesh::EngineLineMesh(const glm::ivec2& window_dimensions): AbstractMesh(VERTEX_SHADER, FRAGMENT_SHADER, window_dimensions) {}

void EngineLineMesh::render()
{
	glViewport(0, 0, m_window_dimensions.x, m_window_dimensions.y);
	m_shader.use(); 
	glBindVertexArray(m_VAO);
	glDrawElements(GL_LINE_LOOP, m_indeces.size(), GL_UNSIGNED_INT, 0); 
	glBindVertexArray(0);
}