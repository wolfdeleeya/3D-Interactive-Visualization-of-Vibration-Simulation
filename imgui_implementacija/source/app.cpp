#include <iostream>

#include "glad/glad.h"
#include "app.h"
#include "nfd.h"
#include "imgui_layer.h"

float App::last_timestamp = 0;
float App::delta_time = 0;

App::App(int init_width, int init_height, const char* vert_shader_path, const char* frag_shader_path) {
	NFD_Init();

	for (int i = 0; i <= GLFW_MOUSE_BUTTON_LAST; ++i)
		m_mouse_button_state[i] = false;

	m_window_width = init_width;
	m_window_height = init_height;

	m_last_mouse_pos = glm::vec2(0);

	init_glfw(init_width, init_height);
	init_opengl();

	m_appliction_model = new ApplicationModel();
	m_imgui_layer = new ImGUILayer(m_appliction_model, m_window, "version 330 core");
	m_engine_mesh = new EngineMesh(vert_shader_path, frag_shader_path);

	m_appliction_model->on_view_mat_changed.add_member_listener(&EngineMesh::set_view, m_engine_mesh);
	m_appliction_model->engine_data()->on_colors_recalculated.add_member_listener(&EngineMesh::set_colors, m_engine_mesh);

	on_window_size_changed.add_member_listener(&EngineMesh::window_size_changed, m_engine_mesh);

	m_imgui_layer->on_load_vertex_positions.add_member_listener(&EngineMesh::load_vertex_positions, m_engine_mesh);
	m_imgui_layer->on_load_vertex_positions.add_member_listener(&ApplicationModel::on_vertex_positions_loaded, m_appliction_model);

	m_imgui_layer->on_load_cell_vertices.add_member_listener(&EngineMesh::load_cell_vertices, m_engine_mesh);
	m_imgui_layer->on_load_cell_vertices.add_member_listener(&EngineData::on_cell_vertices_loaded, m_appliction_model->engine_data());

	m_imgui_layer->on_load_cell_stats.add_member_listener(&ApplicationModel::load_cell_stats, m_appliction_model);

	on_window_size_changed.invoke({ init_width, init_height });

	m_appliction_model->refresh_camera();
}

App::~App()
{
	delete m_imgui_layer;
	delete m_engine_mesh;
	glfwTerminate();
	NFD_Quit();
	delete m_appliction_model;
}

void App::init_glfw(int width, int height) {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	
	m_window = glfwCreateWindow(width, height, "Engine Viewer", nullptr, nullptr);

	if (!m_window) {
		std::cerr << "Couldn't create window!";
		glfwTerminate();
		//TODO: HANDLE ERROR
	}

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);
}

void App::init_opengl() {
	int value = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (!value) {
		std::cerr << "COULDN'T INITIALIZE GLAD!\n";
		glfwTerminate();
		//TODO: HANDLE ERROR
	}

	glEnable(GL_DEPTH_TEST);
}

void App::resize_callback(int width, int height)
{
	m_window_width = width;
	m_window_height = height;

	glViewport(0, 0, m_window_width, m_window_height);

	on_window_size_changed.invoke({ m_window_width, m_window_height });
}

void App::scroll_callback(double x_offset, double y_offset)
{
	bool is_handled = m_imgui_layer->handle_mouse_scroll(x_offset, y_offset);

	if (!is_handled)
		m_appliction_model->move_camera_distance(y_offset);
}

void App::mouse_moved_callback(double x_pos, double y_pos)
{
	glm::vec2 current_mouse_pos{ x_pos, y_pos };

	bool is_handled = m_imgui_layer->handle_mouse_pos(x_pos, y_pos);

	if (!is_handled) {
		if (m_mouse_button_state[GLFW_MOUSE_BUTTON_LEFT]) {
			glm::vec2 mouse_delta = current_mouse_pos - m_last_mouse_pos;
			m_appliction_model->rotate_camera(mouse_delta);
		} else if(m_mouse_button_state[GLFW_MOUSE_BUTTON_RIGHT])
			m_engine_mesh->select_index(x_pos, m_window_height - y_pos);
	}
	m_last_mouse_pos = current_mouse_pos;
}

void App::mouse_button_callback(int button, bool is_pressed)
{
	m_imgui_layer->handle_mouse_click(button, is_pressed);
	m_mouse_button_state[button] = is_pressed;
}

void App::update() {
	glfwPollEvents();
	
	update_time();

	glm::vec3 clear_color = m_appliction_model->clear_color;
	glClearColor(clear_color.r, clear_color.g, clear_color.b, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_appliction_model->update();
	m_engine_mesh->render();
	m_imgui_layer->update();

	glfwSwapBuffers(m_window);
}

void App::update_time()
{
	float current_timestamp = glfwGetTime();
	delta_time = current_timestamp - last_timestamp;
	last_timestamp = current_timestamp;
}
