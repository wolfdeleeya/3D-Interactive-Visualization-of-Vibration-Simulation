#include "engine_mesh.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "GLFW/glfw3.h"
#include "debug.h"

#include <iostream>

const char* EngineMesh::model_par_name = "model";
const char* EngineMesh::view_par_name = "view";
const char* EngineMesh::projection_par_name = "projection";

const char* EngineMesh::LINE_VERT_SHADER = "./Shaders/line_shader.vert";
const char* EngineMesh::LINE_FRAG_SHADER = "./Shaders/line_shader.frag";

const char* EngineMesh::CELL_SELECT_VERT_SHADER = "./Shaders/engine_cell_selection_shader.vert";
const char* EngineMesh::CELL_SELECT_FRAG_SHADER = "./Shaders/engine_cell_selection_shader.frag";


void EngineMesh::setup_vertex_data()
{
	if (m_cell_vertices.size() == 0 || m_vertex_positions.size() == 0)
		return;

	load_model_data();

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_model_data.size() * sizeof(vertex), &m_model_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void EngineMesh::setup_cell_selection_vertex_data()
{
	if (m_cell_vertices.size() == 0 || m_vertex_positions.size() == 0)
		return;

	load_cell_selection_data();

	glBindBuffer(GL_ARRAY_BUFFER, m_CS_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_model_cell_selection_data.size() * sizeof(cell_select_vertex), &m_model_cell_selection_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

	glBindVertexArray(m_CS_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_CS_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cell_select_vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(cell_select_vertex), (void*)offsetof(cell_select_vertex, cell_index));

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
		unsigned int cell_index = pair.first;

		for (unsigned int vert_index : pair.second) {
			glm::vec3 pos = m_vertex_positions[vert_index];
			glm::vec3 color = cell_index == m_selected_index ? glm::vec3(1) : m_cell_colors_map[cell_index];

			glm::vec3 normal = m_cell_normals[cell_index];

			unsigned int index = m_indeces_map[{cell_index, vert_index}];

			m_model_data[index] = { pos, color, normal };
		}
	}
}

void EngineMesh::load_cell_selection_data()
{
	m_model_cell_selection_data.resize(m_indeces_map.size());

	for (auto& pair : m_cell_vertices) {
		unsigned int cell_index = pair.first;
		for (unsigned int vert_index : pair.second) {
			glm::vec3 pos = m_vertex_positions[vert_index];

			unsigned int index = m_indeces_map[{cell_index, vert_index}];

			m_model_cell_selection_data[index] = { pos, cell_index };
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

void EngineMesh::update_cell_selection_framebuffer(std::pair<int, int> window_dimensions)
{
	int width = window_dimensions.first, height = window_dimensions.second;

	glBindFramebuffer(GL_FRAMEBUFFER, m_CS_FBO); 

	glBindRenderbuffer(GL_RENDERBUFFER, m_CS_RBO_color); 
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, width, height); 
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_CS_RBO_color); 

	glBindRenderbuffer(GL_RENDERBUFFER, m_CS_RBO_depth); 
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_CS_RBO_depth);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

EngineMesh::EngineMesh(const char* vertex_shader_dest, const char* fragment_shader_dest, 
	std::pair<int, int> window_dimensions, bool is_cw) :m_shader(vertex_shader_dest, fragment_shader_dest), 
	m_line_shader(LINE_VERT_SHADER, LINE_FRAG_SHADER), m_cell_select_shader(CELL_SELECT_VERT_SHADER, CELL_SELECT_FRAG_SHADER) {

	m_is_cw = is_cw;

	glGenBuffers(1, &m_VBO);
	glGenVertexArrays(1, &m_VAO);

	glGenBuffers(1, &m_CS_VBO);
	glGenVertexArrays(1, &m_CS_VAO);
	
	glGenBuffers(1, &m_EBO);

	glGenFramebuffers(1, &m_CS_FBO);

	glGenRenderbuffers(1, &m_CS_RBO_color);
	glGenRenderbuffers(1, &m_CS_RBO_depth);

	update_cell_selection_framebuffer(window_dimensions);

	setup_buffers();
}

EngineMesh::~EngineMesh()
{
	glDeleteBuffers(1, &m_VBO);
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteFramebuffers(1, &m_CS_FBO);
	glDeleteRenderbuffers(1, &m_CS_RBO_color);
	glDeleteRenderbuffers(1, &m_CS_RBO_depth);

	m_shader.delete_shader();
	m_line_shader.delete_shader();
	m_cell_select_shader.delete_shader();
}

void EngineMesh::render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_CS_FBO);

	glClearColor(0, 0, 0, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	m_cell_select_shader.use();
	glBindVertexArray(m_CS_VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, m_indeces.size(), GL_UNSIGNED_INT, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_shader.use();
	glBindVertexArray(m_VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, m_indeces.size(), GL_UNSIGNED_INT, 0);
	
	m_line_shader.use();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, m_indeces.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void EngineMesh::set_colors(const std::map<unsigned int, glm::vec3>& cell_colors_map)
{
	m_cell_colors_map = cell_colors_map;
	setup_vertex_data();		//TODO: separate color loading from vertex lodaing: low priority currently
}

void EngineMesh::load_cell_vertices(const char* path)
{
	m_cell_vertices = loader::load_cells(path);
	
	m_selected_index = 0;

	setup_indices();

	if (m_cell_vertices.size() > 0 && m_vertex_positions.size() > 0)
		calculate_normals();

	m_indeces = loader::triangulate_cell_indeces(m_cell_vertices, m_indeces_map);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indeces.size() * sizeof(unsigned int), &m_indeces[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	setup_vertex_data();
	setup_cell_selection_vertex_data();
}

void EngineMesh::load_vertex_positions(const char* path)
{
	m_vertex_positions = loader::load_vertices(path);
	if (m_cell_vertices.size() > 0 && m_vertex_positions.size() > 0)
		calculate_normals();
	setup_vertex_data();
	setup_cell_selection_vertex_data();
}

void EngineMesh::window_size_changed(std::pair<int, int> window_dimensions) 
{ 
	set_projection(glm::perspective(45.f, (float)window_dimensions.first / window_dimensions.second, 0.1f, 100.f)); 
	update_cell_selection_framebuffer(window_dimensions);
}

int EngineMesh::get_index_at_pos(GLint x, GLint y)
{
	glm::vec3 color = get_color_at_pos(x, y);
	int red_factor = round(color.r * 255) * 256 * 256;
	int green_factor = round(color.g * 255) * 256;
	int blue_factor = round(color.b * 255);
	return red_factor + green_factor + blue_factor;
}

void EngineMesh::select_index(GLint x, GLint y)
{
	m_selected_index = get_index_at_pos(x, y);
	setup_vertex_data();
}

glm::vec3 EngineMesh::get_color_at_pos(GLint x, GLint y)
{
	glm::vec4 pixel(0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_CS_FBO);

	glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, glm::value_ptr(pixel));
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return pixel;
}