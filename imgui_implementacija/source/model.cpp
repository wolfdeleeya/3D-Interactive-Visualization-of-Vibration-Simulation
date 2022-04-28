#include <iostream>

#include "model.h"
#include "GLFW/glfw3.h"

const char* Model::model_par_name = "model";
const char* Model::view_par_name = "view";
const char* Model::projection_par_name = "projection";

const char* Model::LINE_VERT_SHADER = "./Shaders/line_shader.vert";
const char* Model::LINE_FRAG_SHADER = "./Shaders/line_shader.frag";


void Model::setup_vertex_data()
{
	if (m_cell_vertices.size() == 0 || m_vertex_positions.size() == 0)
		return;

	load_model_data();

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_model_data.size() * sizeof(vertex), &m_model_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	setup_buffers();
}

void Model::setup_buffers()
{
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, normal));
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::calculate_color()
{
	if(m_selected_frequencies_names.size() == 0)
		for (auto& pair : m_cell_vertices)
			m_cell_color[pair.first] = m_default_color;
	else {
		for (auto& pair : m_cell_stats)
			m_cell_color[pair.first] = m_default_color;
	}

}

void Model::calculate_normals()
{
	for (auto& pair : m_cell_vertices) {
		std::vector<unsigned int>& indeces = pair.second;

		glm::vec3 v1 = glm::normalize(m_vertex_positions[indeces[1]] - m_vertex_positions[indeces[0]]);
		glm::vec3 v2 = glm::normalize(m_vertex_positions[indeces[2]] - m_vertex_positions[indeces[1]]);

		m_cell_normals[pair.first] = glm::normalize(m_is_cw ? glm::cross(v2, v1) : glm::cross(v1, v2));
	}
}

void Model::load_model_data()
{
	m_model_data.resize(m_indeces_map.size());

	for (auto& pair : m_cell_vertices) {
		for (unsigned int vert_index : pair.second) {
			glm::vec3 pos = m_vertex_positions[vert_index];
			glm::vec3 color = m_cell_color[pair.first];
			glm::vec3 normal = m_cell_normals[pair.first];

			unsigned int index = m_indeces_map[{pair.first, vert_index}];

			m_model_data[index] = { pos, color, normal };
		}
	}
}

void Model::setup_indices()
{
	m_indeces_map.clear();
	for(auto& pair: m_cell_vertices) {
		for (unsigned int vert_index : pair.second) {
			unsigned int current_size = m_indeces_map.size();
			m_indeces_map[{pair.first, vert_index}] = current_size;
		}
	}
}

Model::~Model() {
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
	glDeleteVertexArrays(1, &m_VAO);
	m_shader.delete_shader();
}

Model::Model(const char* vertex_shader_dest, const char* fragment_shader_dest, bool is_cw, const glm::vec3& default_color, const Gradient& gradient) : 
	m_shader(vertex_shader_dest, fragment_shader_dest), m_line_shader(LINE_VERT_SHADER, LINE_FRAG_SHADER) {
	m_gradient = gradient;
	m_default_color = default_color;
	m_is_cw = is_cw;
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
	glGenVertexArrays(1, &m_VAO);
}

void Model::draw()
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

void Model::set_gradient(const Gradient& g)
{
	m_gradient = g;
	calculate_color();
	setup_vertex_data();		//TODO: separate color loading and vertex loading
}

void Model::load_cell_vertices(const char* path)
{
	m_cell_vertices = loader::load_cells(path);
	
	setup_indices();

	calculate_color();		//setup color to default color

	if (m_cell_vertices.size() > 0 && m_vertex_positions.size() > 0)
		calculate_normals();

	m_indeces = loader::triangulate_cell_indeces(m_cell_vertices, m_indeces_map);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indeces.size() * sizeof(unsigned int), &m_indeces[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	setup_vertex_data();
}
template <typename T>
bool Model::set_value(const std::string& name, T value) {
	m_shader.set_value(name, value);
	m_line_shader.set_value(name, value);

	return true;
}

bool Model::set_model(const glm::mat4& model_mat) {
	m_shader.set_value(model_par_name, model_mat);
	m_line_shader.set_value(model_par_name, model_mat);

	return true;
}

bool Model::set_view(const glm::mat4& view_mat) {
	m_shader.set_value(view_par_name, view_mat);
	m_line_shader.set_value(view_par_name, view_mat);

	return true;
}

bool Model::set_projection(const glm::mat4& projection_mat) {
	m_shader.set_value(projection_par_name, projection_mat);
	m_line_shader.set_value(projection_par_name, projection_mat);

	return true;
}

void Model::load_vertex_positions(const char* path)
{
	m_vertex_positions = loader::load_vertices(path);
	if (m_cell_vertices.size() > 0 && m_vertex_positions.size() > 0)
		calculate_normals();
	setup_vertex_data();
}

void Model::load_cell_stats(const char* path)
{
	m_cell_stats = loader::load_cell_stats(path, m_frequenzy_names);
	setup_vertex_data();
}

void Model::set_attribute(const std::string& attribute_name, bool value)
{
	if (value)				//if true add name
		m_selected_frequencies_names.push_back(attribute_name);
	else {					//if false delete name
		const auto& last = std::remove(m_selected_frequencies_names.begin(), m_selected_frequencies_names.end(), attribute_name);
		m_selected_frequencies_names.erase(last);
	}

	calculate_color();
	setup_vertex_data();
}

void Model::clear()
{
	glDeleteBuffers(1, &m_VBO);
	glDeleteVertexArrays(1, &m_VAO);
}