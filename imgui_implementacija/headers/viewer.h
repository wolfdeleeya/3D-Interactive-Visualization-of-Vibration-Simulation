#pragma once

#include <vector>

#include "engine_mesh.h"
#include "application_model.h"
#include "imgui_layer.h"
#include "GLFW/glfw3.h"

//APP
class Viewer {
private:
	ApplicationModel* m_appliction_model;
	EngineMesh* m_engine_mesh;
	GLFWwindow* m_window;

	ImGUILayer* m_imgui_layer;

	glm::vec2 m_last_mouse_pos;
	int m_window_width, m_window_height;
	bool m_mouse_button_state[GLFW_MOUSE_BUTTON_LAST + 1];


	void init_glfw(int width, int height);

	void init_opengl();

public:
	Event<std::pair<int, int>> on_window_size_changed;

	Viewer(int init_width, int init_height, const char* vert_shader_path, const char* frag_shader_path);

	~Viewer();
	
	bool should_close() { return glfwWindowShouldClose(m_window); }

	void resize_callback(int width, int height);

	void scroll_callback(double x_offset, double y_offset);

	void mouse_moved_callback(double x_pos, double y_pos);

	void mouse_button_callback(int button, bool is_pressed);

	void close() { glfwSetWindowShouldClose(m_window, true); }

	void update();

	GLFWwindow* window() { return m_window; }
};