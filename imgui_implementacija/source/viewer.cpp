#include <iostream>

#include "glad/glad.h"
#include "viewer.h"
#include "nfd.h"
#include "imgui_layer.h"

Viewer::Viewer(int init_width, int init_height, const char* vert_shader_path, const char* frag_shader_path) {
	NFD_Init();

	for (int i = 0; i <= GLFW_MOUSE_BUTTON_LAST; ++i)
		m_mouse_button_state[i] = false;

	m_window_width = init_width;
	m_window_height = init_height;

	m_last_mouse_pos = glm::vec2(0);

	init_glfw(init_width, init_height);
	init_opengl();

	m_appliction_model = new ApplicationModel(vert_shader_path, frag_shader_path, init_width, init_height);
	m_imgui_layer = new ImGUILayer(m_appliction_model, m_window, "version 330 core");
}

Viewer::~Viewer()
{
	delete m_imgui_layer;
	glfwTerminate();
	NFD_Quit();
	delete m_appliction_model;
}

void Viewer::init_glfw(int width, int height) {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(width, height, "Engine Viewer", nullptr, nullptr);

	if (!m_window) {
		std::cerr << "Couldn't create window!";
		glfwTerminate();
		//TODO: HANDLE ERROR
	}

	glfwMakeContextCurrent(m_window);
}

void Viewer::init_opengl() {
	int value = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (!value) {
		std::cerr << "COULDN'T INITIALIZE GLAD!\n";
		glfwTerminate();
		//TODO: HANDLE ERROR
	}

	glEnable(GL_DEPTH_TEST);
}

void Viewer::resize_callback(int width, int height)
{
	m_window_width = width;
	m_window_height = height;

	glViewport(0, 0, m_window_width, m_window_height);
	m_appliction_model->set_window_size(m_window_width, m_window_height);

	on_window_size_changed.invoke({ m_window_width, m_window_height });
}

void Viewer::scroll_callback(double x_offset, double y_offset)
{
	bool is_handled = m_imgui_layer->handle_mouse_scroll(x_offset, y_offset);

	if (!is_handled)
		m_appliction_model->move_camera_distance(y_offset);
}

void Viewer::mouse_moved_callback(double x_pos, double y_pos)
{
	glm::vec2 current_mouse_pos{ x_pos, y_pos };

	bool is_handled = m_imgui_layer->handle_mouse_pos(x_pos, y_pos);

	if (!is_handled) {
		if (m_mouse_button_state[GLFW_MOUSE_BUTTON_LEFT]) {
			glm::vec2 mouse_delta = current_mouse_pos - m_last_mouse_pos;
			m_appliction_model->rotate_camera(mouse_delta);
		}
	}
	m_last_mouse_pos = current_mouse_pos;
}

void Viewer::mouse_button_callback(int button, bool is_pressed)
{
	m_imgui_layer->handle_mouse_click(button, is_pressed);
	m_mouse_button_state[button] = is_pressed;
}

void Viewer::update() {
	glfwPollEvents();

	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_appliction_model->model()->draw();

	m_imgui_layer->update();

	glfwSwapBuffers(m_window);
}
