#pragma once
#include <vector>
#include <string>
#include <map>
#include "glm/mat4x4.hpp"
#include "glad/glad.h"
#include "glm/vec2.hpp"

unsigned int load_shader(const char* file_dest, unsigned int shader_type);

unsigned int load_program(const char* vertex_shader_dest, const char* frag_shader_dest);

bool check_shader_status(unsigned int shader_id);

bool check_program_status(unsigned int program_id);

class Shader {
private:
	unsigned int m_program_id;

	bool m_is_active;

	std::map<std::string, int> m_vert_shader_defs;
	std::map<std::string, int> m_frag_shader_defs;

	std::string m_vertex_shader_path;
	std::string m_fragment_shader_path;

	std::map<std::string, int> m_uniform_map;

	void set_value(int location, float value);
	void set_value(int location, int value);
	void set_value(int location, const glm::vec2& value);
	void set_value(int location, const glm::vec4& value);
	void set_value(int location, const glm::vec3& value);
	void set_value(int location, const glm::mat4x4& value);

public:
	Shader(const char* vert_shader_dest, const char* frag_shader_dest);

	void set_def(const char* def_name, int value, unsigned int shader_type);

	void use();

	void delete_shader();

	template <typename T>
	bool set_value(const std::string& name, T value);
};