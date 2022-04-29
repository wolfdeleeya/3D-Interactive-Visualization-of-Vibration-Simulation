#pragma once

#include "data_loading.h"
#include "shader_helper.h"

struct vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
};

struct cell_select_vertex {
	glm::vec3 position;
	unsigned int cell_index;
};

class EngineMesh {
private:
	Shader m_shader, m_line_shader, m_cell_select_shader;
	unsigned int m_VBO, m_VAO, m_EBO;
	unsigned int m_CS_VBO, m_CS_VAO;	//cell selection buffer and vertex attribute array

	unsigned int m_CS_FBO;
	unsigned int m_CS_RBO_color, m_CS_RBO_depth;

	const static char* LINE_VERT_SHADER;
	const static char* LINE_FRAG_SHADER;

	const static char* CELL_SELECT_VERT_SHADER;
	const static char* CELL_SELECT_FRAG_SHADER;

	std::vector<vertex> m_model_data;
	std::vector<cell_select_vertex> m_model_cell_selection_data;

	std::vector<unsigned int> m_indeces;

	std::map<unsigned int, std::vector<unsigned int>> m_cell_vertices;
	std::map<unsigned int, glm::vec3> m_cell_colors_map;
	std::map<unsigned int, glm::vec3> m_vertex_positions;
	std::map<unsigned int, glm::vec3> m_cell_normals;

	std::map<std::pair<unsigned int, unsigned int>, unsigned int> m_indeces_map;

	int m_selected_index;

	bool m_is_cw;

	const static char* model_par_name, * view_par_name, * projection_par_name;

	void setup_vertex_data();

	void setup_cell_selection_vertex_data();

	void setup_buffers();

	void calculate_normals();

	void load_model_data();

	void load_cell_selection_data();

	void setup_indices();

	void update_cell_selection_framebuffer(std::pair<int, int> window_dimensions);

	glm::vec3 get_color_at_pos(GLint x, GLint y);

public:
	EngineMesh(const char* vertex_shader_dest, const char* fragment_shader_dest, std::pair<int, int> window_dimensions, bool is_cw = false);

	~EngineMesh();

	void render();

	template <typename T>

	bool set_value(const std::string& name, T value);

	void set_model(const glm::mat4& model_mat) { set_value(model_par_name, model_mat); }

	void set_view(const glm::mat4& view_mat) { set_value(view_par_name, view_mat); }

	void set_projection(const glm::mat4& projection_mat) { set_value(projection_par_name, projection_mat); }

	void set_colors(const std::map<unsigned int, glm::vec3>& cell_colors_map);

	void load_cell_vertices(const char* path);

	void load_vertex_positions(const char* path);

	void window_size_changed(std::pair<int, int> window_dimensions);

	int get_index_at_pos(GLint x, GLint y);

	void select_index(GLint x, GLint y);

	bool is_empty() { return m_cell_vertices.size() == 0 || m_vertex_positions.size() == 0; }
};

template<typename T>
inline bool EngineMesh::set_value(const std::string& name, T value)
{
	m_shader.set_value(name, value);
	m_line_shader.set_value(name, value);
	m_cell_select_shader.set_value(name, value);

	return true;
}
