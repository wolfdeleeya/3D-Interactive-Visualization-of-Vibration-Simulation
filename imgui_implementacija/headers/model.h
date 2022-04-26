#pragma once

#include "data_loading.h"
#include "shader_helper.h"
#include "gradient.h"

struct vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
};

class Model {
private:
	Shader m_shader, m_line_shader;
	unsigned int m_VBO, m_VAO, m_EBO;
	
	const static char* LINE_VERT_SHADER;
	const static char* LINE_FRAG_SHADER;

	std::vector<vertex> m_model_data;
	std::vector<unsigned int> m_indeces;

	std::map<unsigned int, std::vector<unsigned int>> m_cell_vertices;
	std::map<unsigned int, glm::vec3> m_vertex_positions;
	std::map<unsigned int, cell_stats> m_cell_stats;
	std::map<unsigned int, glm::vec3> m_cell_color;
	std::map<unsigned int, glm::vec3> m_cell_normals;

	std::map<std::pair<unsigned int, unsigned int>, unsigned int> m_indeces_map;

	std::vector<std::string> m_frequenzy_names;

	float m_global_min, global_max;

	std::vector<std::string> m_selected_frequencies_names;

	glm::vec3 m_default_color;
	Gradient m_gradient;

	bool m_is_cw;

	const static char* model_par_name, *view_par_name, *projection_par_name;

	void setup_vertex_data();
	
	void setup_buffers();
	
	void calculate_color();

	void calculate_normals();

	void load_model_data();

	void setup_indices();

public:
	Model(const char* vertex_shader_dest, const char* fragment_shader_dest, bool is_cw = false, const glm::vec3& default_color = glm::vec3(1), const Gradient& gradient = Gradient());
	
	~Model();
	
	void draw();
	
	template <typename T>
	
	bool set_value(const std::string& name, T value);
	
	bool set_model(const glm::mat4& model_mat);

	bool set_view(const glm::mat4& view_mat);

	bool set_projection(const glm::mat4& projection_mat);

	void set_gradient(const Gradient& g);

	void load_cell_vertices(const char* path);
	
	void load_vertex_positions(const char* path);
	
	void load_cell_stats(const char* path);
	
	void set_attribute(const std::string& attribute_name, bool value);
	
	void clear();

	bool are_stats_loaded() { return m_cell_stats.size() > 0; }

	std::vector<std::string> frequenzy_names() { return m_frequenzy_names; }
};