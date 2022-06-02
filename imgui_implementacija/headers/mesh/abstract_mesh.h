#pragma once

#include "shader_helper.h"
#include <iostream>

class AbstractMesh {
private:
	static const char* MODEL_PAR_NAME;
	static const char* VIEW_PAR_NAME;
	static const char* PROJECTION_PAR_NAME;
protected:
	Shader m_shader;
	unsigned int m_VBO, m_VAO;

	glm::ivec2 m_window_dimensions;

	virtual void setup_buffers() = 0;
	virtual void setup_vertex_data() = 0;

public:
	AbstractMesh(const char* vertex_shader_dest, const char* fragment_shader_dest, const glm::ivec2& window_dimensions);

	virtual ~AbstractMesh();

	virtual void render() = 0;
	
	void update_window_size(const glm::ivec2& window_dimensions);

	template <typename T>
	bool set_value(const std::string& name, T value);

	void set_model(const glm::mat4& model_mat) { set_value(MODEL_PAR_NAME, model_mat); }

	void set_view(const glm::mat4& view_mat) { set_value(VIEW_PAR_NAME, view_mat); }

	void set_projection(const glm::mat4& projection_mat) { set_value(PROJECTION_PAR_NAME, projection_mat); }
};

template<typename T>
inline bool AbstractMesh::set_value(const std::string& name, T value)
{
	return m_shader.set_value(name, value);
}
