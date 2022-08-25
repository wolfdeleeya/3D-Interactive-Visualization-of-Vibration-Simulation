#include "mesh/axis_mesh.h"

const std::vector<AxisMesh::axis_vertex> AxisMesh::VERTEX_DATA = {
	{0, 0},
	{1, 0},
	{0, 1},
	{2, 1},
	{0, 2},
	{3, 2},
};

const char* AxisMesh::VERTEX_SHADER = "./Shaders/axis_shader.vert";
const char* AxisMesh::FRAGMENT_SHADER = "./Shaders/axis_shader.frag";

void AxisMesh::setup_buffers()
{
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_BYTE, sizeof(axis_vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, sizeof(axis_vertex), (void*)offsetof(axis_vertex, axis_id));

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AxisMesh::setup_vertex_data()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, VERTEX_DATA.size() * sizeof(axis_vertex), &VERTEX_DATA[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

AxisMesh::AxisMesh(const glm::ivec2& window_dimensions, unsigned int target_FBO): AbstractMesh(VERTEX_SHADER, FRAGMENT_SHADER, window_dimensions)
{
	m_target_FBO = target_FBO;
	
	setup_vertex_data();
	setup_buffers();
}

void AxisMesh::render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_target_FBO);

	glViewport(0, 0, m_window_dimensions.x, m_window_dimensions.y);

	m_shader.use();

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_LINES, 0, VERTEX_DATA.size());

	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
