#include "engine_mesh.h"
#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h"

const char* EngineMesh::model_par_name = "model";
const char* EngineMesh::view_par_name = "view";
const char* EngineMesh::projection_par_name = "projection";

const char* EngineMesh::LINE_VERT_SHADER = "./Shaders/line_shader.vert";
const char* EngineMesh::LINE_FRAG_SHADER = "./Shaders/line_shader.frag";


void EngineMesh::setup_vertex_data()
{
	if (m_cell_vertices.size() == 0 || m_vertex_positions.size() == 0)
		return;

	load_model_data();

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_model_data.size() * sizeof(vertex), &m_model_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	setup_buffers();
}

void EngineMesh::setup_buffers()
{
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EngineMesh::calculate_normals()
{
	for (auto& pair : m_cell_vertices) {
		std::vector<unsigned int>& indeces = pair.second;

		glm::vec3 v1 = glm::normalize(m_vertex_positions[indeces[1]] - m_vertex_positions[indeces[0]]);
		glm::vec3 v2 = glm::normalize(m_vertex_positions[indeces[2]] - m_vertex_positions[indeces[1]]);

		m_cell_normals[pair.first] = glm::normalize(m_is_cw ? glm::cross(v2, v1) : glm::cross(v1, v2));
	}
}

void EngineMesh::load_model_data()
{
	m_model_data.resize(m_indeces_map.size());

	for (auto& pair : m_cell_vertices) {
		for (unsigned int vert_index : pair.second) {
			glm::vec3 pos = m_vertex_positions[vert_index];
			glm::vec3 color = m_cell_colors_map[pair.first];
			glm::vec3 normal = m_cell_normals[pair.first];

			unsigned int index = m_indeces_map[{pair.first, vert_index}];

			m_model_data[index] = { pos, color, normal };
		}
	}
}

void EngineMesh::setup_indices()
{
	m_indeces_map.clear();
	for (auto& pair : m_cell_vertices) {
		for (unsigned int vert_index : pair.second) {
			unsigned int current_size = m_indeces_map.size();
			m_indeces_map[{pair.first, vert_index}] = current_size;
		}
	}
}

EngineMesh::EngineMesh(const char* vertex_shader_dest, const char* fragment_shader_dest, bool is_cw):
	m_shader(vertex_shader_dest, fragment_shader_dest), m_line_shader(LINE_VERT_SHADER, LINE_FRAG_SHADER) {
	m_is_cw = is_cw;
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
	glGenVertexArrays(1, &m_VAO);
}

EngineMesh::~EngineMesh()
{
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
	glDeleteVertexArrays(1, &m_VAO);
	m_shader.delete_shader();
}

void EngineMesh::render()
{
	if (m_indeces.size() == 0 || m_model_data.size() == 0)		//if some part of render data isn't loaded properly
		return;

	m_shader.use();
	glBindVertexArray(m_VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, m_indeces.size(), GL_UNSIGNED_INT, 0);
	
	m_line_shader.use();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, m_indeces.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void EngineMesh::set_model(const glm::mat4& model_mat)
{
	m_shader.set_value(model_par_name, model_mat);
	m_line_shader.set_value(model_par_name, model_mat);
}

void EngineMesh::set_view(const glm::mat4& view_mat)
{
	m_shader.set_value(view_par_name, view_mat);
	m_line_shader.set_value(view_par_name, view_mat);
}

void EngineMesh::set_projection(const glm::mat4& projection_mat)
{
	m_shader.set_value(projection_par_name, projection_mat);
	m_line_shader.set_value(projection_par_name, projection_mat);
}

void EngineMesh::set_colors(const std::map<unsigned int, glm::vec3>& cell_colors_map)
{
	m_cell_colors_map = cell_colors_map;
	setup_vertex_data();		//TODO: separate color loading from vertex lodaing: low priority currently
}

void EngineMesh::load_cell_vertices(const char* path)
{
	m_cell_vertices = loader::load_cells(path);

	setup_indices();

	if (m_cell_vertices.size() > 0 && m_vertex_positions.size() > 0)
		calculate_normals();

	m_indeces = loader::triangulate_cell_indeces(m_cell_vertices, m_indeces_map);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indeces.size() * sizeof(unsigned int), &m_indeces[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	setup_vertex_data();
}

void EngineMesh::load_vertex_positions(const char* path)
{
	m_vertex_positions = loader::load_vertices(path);
	if (m_cell_vertices.size() > 0 && m_vertex_positions.size() > 0)
		calculate_normals();
	setup_vertex_data();
}

void EngineMesh::window_size_changed(std::pair<int, int> window_dimensions) 
{ 
	set_projection(glm::perspective(45.f, (float)window_dimensions.first / window_dimensions.second, 0.1f, 100.f)); 
}

void EngineMesh::clear()
{
	glDeleteBuffers(1, &m_VBO);
	glDeleteVertexArrays(1, &m_VAO);
}