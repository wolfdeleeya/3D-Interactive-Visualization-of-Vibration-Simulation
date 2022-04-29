#include <iostream>
#include <fstream>
#include "glad/glad.h"
#include "shader_helper.h"
#include <glm/gtc/type_ptr.hpp>

#include "debug.h"

const int SHADER_INFO_LENGTH = 512;

unsigned int load_shader(const char* file_dest, unsigned int shader_type) {
	std::ifstream file(file_dest);
	if (!file)
		throw std::invalid_argument("SHADER DOES NOT EXIST");
	std::string line, text;

	while (std::getline(file, line))
		text += line + '\n';

	text += '\0';
	const char* text_c = text.c_str();

	unsigned int shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &text_c, nullptr);
	glCompileShader(shader);

	if (!check_shader_status(shader))
		return -1;

	return shader;
}

unsigned int load_program(const char* vertex_shader_dest, const char* frag_shader_dest) {
	unsigned int vertex_shader = load_shader(vertex_shader_dest, GL_VERTEX_SHADER);
	unsigned int frag_shader = load_shader(frag_shader_dest, GL_FRAGMENT_SHADER);
	
	unsigned int program = glCreateProgram();

	glAttachShader(program, vertex_shader);
	glAttachShader(program, frag_shader);
	glLinkProgram(program);

	glDeleteShader(vertex_shader);
	glDeleteShader(frag_shader);

	if (!check_program_status(program))
		return -1;

	return program;
}

bool check_shader_status(unsigned int shader_id) {
	int is_success;
	int shader_type;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &is_success);
	glGetShaderiv(shader_id, GL_SHADER_TYPE, &shader_type);

	if (!is_success) {
		char info[SHADER_INFO_LENGTH];
		glGetShaderInfoLog(shader_id, SHADER_INFO_LENGTH, nullptr, info);
		std::cerr << (shader_type == GL_FRAGMENT_SHADER ? "FRAGMENT: " : "VERTEX: ") << info << '\n';
	}

	return is_success;
}

bool check_program_status(unsigned int program_id) {
	int is_success;
	glGetProgramiv(program_id, GL_LINK_STATUS, &is_success);

	if (!is_success) {
		char info[SHADER_INFO_LENGTH];
		glGetProgramInfoLog(program_id, SHADER_INFO_LENGTH, nullptr, info);
		std::cerr << "PROGRAM: " << info << '\n';
	}

	return is_success;
}

Shader::Shader(const char* vert_shader_dest, const char* frag_shader_dest) {

	m_program_id = load_program(vert_shader_dest, frag_shader_dest);
	m_is_active = true;

	m_vertex_shader_path = vert_shader_dest;
	m_fragment_shader_path = frag_shader_dest;
}

void Shader::set_value(int location, float value) {
	this->use();
	glUniform1f(location, value);
}

void Shader::set_value(int location, int value) {
	this->use();
	glUniform1i(location, value);
}

void Shader::set_value(int location, const glm::vec2& value) {
	this->use();
	glUniform2fv(location, 1, glm::value_ptr(value));
}

void Shader::set_value(int location, const glm::mat4x4& value) {
	this->use();
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(value));
}

void Shader::set_value(int location, const glm::vec4& value) {
	this->use();
	glUniform4fv(location, 1, glm::value_ptr(value));
}

void Shader::set_value(int location, const glm::vec3& value) {
	this->use();
	glUniform3fv(location, 1, glm::value_ptr(value));
}

template <typename T>
bool Shader::set_value(const std::string& name, T value) {
	if (m_uniform_map.count(name) > 0)
		set_value(m_uniform_map[name], value);
	else {
		int location = glGetUniformLocation(m_program_id, name.c_str());

		if (location == -1)
			return false;

		m_uniform_map[name] = location;
		set_value(location, value);
	}

	return true;
}


void Shader::set_def(const char* def_name, int value, unsigned int shader_type) {
	switch (shader_type) {
	case GL_VERTEX_SHADER:
		m_vert_shader_defs[def_name] = value;
		break;
	case GL_FRAGMENT_SHADER:
		m_frag_shader_defs[def_name] = value;
		break;
	}

	delete_shader();

	m_program_id = load_program(m_vertex_shader_path.c_str(), m_fragment_shader_path.c_str());
	m_is_active = true;
}

void Shader::use() {
	if (m_is_active)
		glUseProgram(m_program_id);
	else
		throw new std::exception("INACTIVE SHADER!\n");
}

void Shader::delete_shader() {
	if (m_is_active) {
		glDeleteProgram(m_program_id);
		m_is_active = false;
	}
}

template bool Shader::set_value<>(const std::string& name, float value);
template bool Shader::set_value<>(const std::string& name, int value);
template bool Shader::set_value<>(const std::string& name, glm::vec2 value);
template bool Shader::set_value<>(const std::string& name, glm::vec4 value);
template bool Shader::set_value<>(const std::string& name, glm::vec3 value);
template bool Shader::set_value<>(const std::string& name, glm::mat4x4 value);