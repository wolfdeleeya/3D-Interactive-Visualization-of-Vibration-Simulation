#pragma once

#include <vector>

#include "mesh_manager.h"
#include "application_model.h"
#include "imgui_layer.h"
#include "GLFW/glfw3.h"

//APP
class App {
private:
	ApplicationModel* m_appliction_model;
	GLFWwindow* m_window;
	MeshManager* m_mesh_manager;

	ImGUILayer* m_imgui_layer;

	glm::vec2 m_last_mouse_pos;
	glm::vec2 m_current_mouse_pos;
	glm::vec2 m_mouse_delta;

	bool m_mouse_button_state[GLFW_MOUSE_BUTTON_LAST + 1];

	void init_glfw(int width, int height);

	void init_opengl();

public:
	static float last_timestamp;
	static float delta_time;

	App(int init_width, int init_height, const char* vert_shader_path, const char* frag_shader_path);

	~App();
	
	bool should_close() { return glfwWindowShouldClose(m_window); }

	void scroll_callback(double x_offset, double y_offset);

	void mouse_moved_callback(double x_pos, double y_pos);

	void mouse_button_callback(int button, bool is_pressed);
	
	void close() { glfwSetWindowShouldClose(m_window, true); }

	void update();

	static void update_time();

	GLFWwindow* window() { return m_window; }
};