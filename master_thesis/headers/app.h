#pragma once

#include <vector>

#include "mesh_manager.h"
#include "application_model.h"
#include "engine_model.h"
#include "imgui_manager.h"
#include "GLFW/glfw3.h"

//APP
class App {
private:
	GLFWwindow* m_window;

	ApplicationModel* m_appliction_model;
	EngineModel* m_engine_model;
	
	MeshManager* m_mesh_manager;
	GraphManager* m_graph_manager;

	ImGuiManager* m_imgui_manager;

	glm::vec2 m_last_mouse_pos;
	glm::vec2 m_current_mouse_pos;
	glm::vec2 m_mouse_delta;

	bool m_mouse_button_state[GLFW_MOUSE_BUTTON_LAST + 1];

	App(int init_width, int init_height);

	void init_glfw(int width, int height);

	void init_opengl();

	void init_imgui();

	void init_implot();

	void set_callbacks();
public:
	static App* app_instance;
	static float last_timestamp;
	static float delta_time;

	Event<const glm::ivec2&> on_mouse_dragged;
	
	Signal on_mouse_clicked;

	static App* create_app(int init_width, int init_height);

	App(const App& a) = delete;

	App operator=(const App& a) = delete;

	~App();
	
	bool should_close() { return glfwWindowShouldClose(m_window); }

	void handle_scroll_callback(double x_offset, double y_offset);

	void handle_mouse_moved_callback(double x_pos, double y_pos);

	void handle_mouse_button_callback(int button, bool is_pressed);

	void handle_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	
	void close() { glfwSetWindowShouldClose(m_window, true); }

	void update();

	static void update_time();

	GLFWwindow* window() { return m_window; }

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) { app_instance->handle_key_callback(window, key, scancode, action, mods); }

	static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) { app_instance->handle_scroll_callback(x_offset, y_offset); }

	static void mouse_moved_callback(GLFWwindow* window, double x_pos, double y_pos) { app_instance->handle_mouse_moved_callback(x_pos, y_pos); }

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) { app_instance->handle_mouse_button_callback(button, action == GLFW_PRESS); }
};