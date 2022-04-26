#pragma once


#include "data_loading.h"
#include "shader_helper.h"

struct vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
};

class EngineMesh {
private:
	Shader m_shader, m_line_shader;
	unsigned int m_VBO, m_VAO, m_EBO;

	const static char* LINE_VERT_SHADER;
	const static char* LINE_FRAG_SHADER;

	std::vector<vertex> m_model_data;
	std::vector<unsigned int> m_indeces;

	std::map<unsigned int, std::vector<unsigned int>> m_cell_vertices;
	std::map<unsigned int, glm::vec3> m_cell_colors_map;
	std::map<unsigned int, glm::vec3> m_vertex_positions;
	std::map<unsigned int, glm::vec3> m_cell_normals;

	std::map<std::pair<unsigned int, unsigned int>, unsigned int> m_indeces_map;

	bool m_is_cw;

	const static char* model_par_name, * view_par_name, * projection_par_name;

	void setup_vertex_data();

	void setup_buffers();

	void calculate_normals();

	void load_model_data();

	void setup_indices();
public:
	EngineMesh(const char* vertex_shader_dest, const char* fragment_shader_dest, bool is_cw = false);

	~EngineMesh();

	void render();

	template <typename T>

	bool set_value(const std::string& name, T value);

	bool set_model(const glm::mat4& model_mat);

	bool set_view(const glm::mat4& view_mat);

	bool set_projection(const glm::mat4& projection_mat);

	void load_cell_vertices(const char* path);

	void set_colors(const std::map<unsigned int, glm::vec3>& cell_colors_map);

	void load_vertex_positions(const char* path);

	void clear();
};

template<typename T>
inline bool EngineMesh::set_value(const std::string& name, T value)
{
	m_shader.set_value(name, value);
	m_line_shader.set_value(name, value);

	return true;
}
