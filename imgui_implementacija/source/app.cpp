#include <iostream>

#include "glad/glad.h"
#include "app.h"
#include "nfd.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_manager.h"

App* App::app_instance = nullptr;
float App::last_timestamp = 0;
float App::delta_time = 0;

App::App(int init_width, int init_height) {
	NFD_Init();

	for (int i = 0; i <= GLFW_MOUSE_BUTTON_LAST; ++i)
		m_mouse_button_state[i] = false;

	m_last_mouse_pos = glm::vec2(0);

	init_glfw(init_width, init_height);
	init_opengl();

	init_imgui();
	init_implot();

	set_callbacks();

	m_appliction_model = new ApplicationModel();
	m_engine_model = new EngineModel(glm::vec3(0.55));

	m_mesh_manager = new MeshManager(m_appliction_model, m_engine_model, { init_width, init_height });
	m_graph_manager = new GraphManager(m_appliction_model, m_engine_model);

	m_imgui_manager = new ImGuiManager(m_appliction_model, m_engine_model, m_mesh_manager, m_graph_manager, m_window, "version 330 core");

	on_mouse_dragged.add_member_listener(&ApplicationModel::handle_mouse_dragged, m_appliction_model);
	on_mouse_dragged.add_member_listener(&EngineModel::handle_mouse_dragged, m_engine_model);

	on_mouse_clicked.add_member_listener(&EngineModel::handle_mouse_click, m_engine_model);

	m_appliction_model->on_view_mat_changed.add_member_listener(&MeshManager::view_mat_changed, m_mesh_manager);

	m_appliction_model->on_limits_mode_toggled.add_member_listener(&EngineModel::on_limits_mode_toggled, m_engine_model);

	m_engine_model->on_colors_recalculated.add_member_listener(&MeshManager::colors_recalculated, m_mesh_manager);
	m_engine_model->on_cell_hovered.add_member_listener(&MeshManager::cell_hovered, m_mesh_manager);

	m_imgui_manager->on_scene_view_scale_changed.add_member_listener(&MeshManager::window_size_changed, m_mesh_manager);

	m_imgui_manager->on_load_vertex_positions.add_member_listener(&MeshManager::load_vertex_positions, m_mesh_manager);
	m_imgui_manager->on_load_vertex_positions.add_member_listener(&ApplicationModel::on_vertex_positions_loaded, m_appliction_model);

	m_imgui_manager->on_load_cell_vertices.add_member_listener(&EngineModel::load_cell_vertices, m_engine_model);
	m_imgui_manager->on_load_cell_vertices.add_member_listener(&MeshManager::load_cell_vertices, m_mesh_manager);

	m_imgui_manager->on_load_cell_stats.add_member_listener(&EngineModel::load_cell_stats, m_engine_model);

	m_imgui_manager->on_load_frequency_limits.add_member_listener(&EngineModel::load_frequency_limits, m_engine_model);

	m_imgui_manager->on_scene_view_focus_changed.add_member_listener(&EngineModel::on_scene_view_focus_changed, m_engine_model);

	m_mesh_manager->on_vertices_loaded.add_member_listener(&EngineModel::calculate_color, m_engine_model);		//notify engine data to recalculate colors if it's loaded before cells and vertices
	m_mesh_manager->on_cell_vertices_loaded.add_member_listener(&EngineModel::calculate_color, m_engine_model);

	m_appliction_model->refresh_camera();
}

App* App::create_app(int init_width, int init_height)
{
	if (app_instance == nullptr)
		app_instance = new App(init_width, init_height);

	return app_instance;
}

App::~App()
{
	delete m_appliction_model;
	delete m_engine_model;

	delete m_mesh_manager;
	delete m_graph_manager;

	delete m_imgui_manager;

	glfwTerminate();
	NFD_Quit();
}

void App::init_glfw(int width, int height) {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
	glfwWindowHint(GLFW_SAMPLES, 4);
	
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
	glEnable(GL_MULTISAMPLE);

	//offset added so that the wireframe is drawn in front
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
}

void App::init_imgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void App::init_implot()
{
	ImPlot::CreateContext();
	ImPlot::GetStyle().AntiAliasedLines = true;
}

void App::set_callbacks()
{
	glfwSetKeyCallback(m_window, key_callback);
	glfwSetScrollCallback(m_window, scroll_callback);
	glfwSetCursorPosCallback(m_window, mouse_moved_callback);
	glfwSetMouseButtonCallback(m_window, mouse_button_callback);
}

void App::handle_scroll_callback(double x_offset, double y_offset)
{
	bool is_handled = m_imgui_manager->handle_mouse_scroll(x_offset, y_offset);

	if (!is_handled)
		m_appliction_model->move_camera_distance(y_offset);
}

void App::handle_mouse_moved_callback(double x_pos, double y_pos)
{
	m_current_mouse_pos = { x_pos, y_pos };
	m_mouse_delta = m_current_mouse_pos - m_last_mouse_pos;

	bool is_handled = m_imgui_manager->handle_mouse_pos(x_pos, y_pos);
	if (!is_handled)
	{
		if (m_mouse_button_state[GLFW_MOUSE_BUTTON_LEFT]) {
			on_mouse_dragged.invoke(m_mouse_delta);
		}
		else {
			glm::ivec2 scene_view_space_mouse_pos = m_imgui_manager->get_scene_view_space_mouse_pos(m_current_mouse_pos);
			int scene_view_height = m_imgui_manager->scene_view_scale().y;
			unsigned int selected_cell_index = m_mesh_manager->get_index_at_pos(scene_view_space_mouse_pos.x, scene_view_height - scene_view_space_mouse_pos.y);
			m_engine_model->set_hovered_cell(selected_cell_index);
		}
	}
}

void App::handle_mouse_button_callback(int button, bool is_pressed)
{
	bool is_handled = m_imgui_manager->handle_mouse_click(button, is_pressed);
	m_mouse_button_state[button] = is_pressed;

	if (!is_handled) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && is_pressed == false && int(glm::length(m_mouse_delta)) == 0)
			on_mouse_clicked.invoke();
	}
}

void App::handle_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	m_imgui_manager->handle_key_callback(window, key, scancode, action, mods);
}

void App::update() {
	m_last_mouse_pos = m_current_mouse_pos;

	glfwPollEvents();

	m_mouse_delta = m_current_mouse_pos - m_last_mouse_pos;

	update_time();

	m_appliction_model->update();
	m_engine_model->check_for_changes();
	m_mesh_manager->render();
	m_imgui_manager->update();

	glfwSwapBuffers(m_window);
}

void App::update_time()
{
	float current_timestamp = glfwGetTime();
	delta_time = current_timestamp - last_timestamp;
	last_timestamp = current_timestamp;
}