#include <memory>

#include "imgui_manager.h"
#include "app.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "implot_internal.h"
#include "nfd.h"
#include "debug.h"
#include "implot_helper.h"
#include "custom_imgui_widgets.h"

void ImGuiManager::draw_color_selection_widget()
{
	if (ImGui::Begin("Color Properties Selection")) {
		draw_color_selection("Background Color", *m_application_model->get_color(ApplicationModel::ColorVariables::CLEAR_COLOR));

		draw_color_selection("Default Color", *m_engine_model->get_color(EngineModel::ColorVariables::DEFAULT_COLOR));

		unsigned int current_selected_cells_pallete_index = m_engine_model->current_selected_cells_pallete_index();

		auto& current_pallete_data = m_selected_cells_palletes_textures[current_selected_cells_pallete_index];

		EngineModel* engine_model = m_engine_model;
		ImGui::Image((void*)current_pallete_data.second, { 60, 60 });
		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGui::Text(("Pallete Name: " + current_pallete_data.first).c_str());
		if (ImGui::Button("Set Next Color Pallete \nFor Selected Cells")) {
			m_engine_model->set_next_selected_cells_pallete();
		}
		ImGui::EndGroup();

		if (m_engine_model->are_stats_loaded()) {
			EngineModel::VisualizationMode current_visualization_mode = m_engine_model->current_visualization_mode();
			bool is_limits_mode_active = current_visualization_mode == EngineModel::VisualizationMode::LIMITS;

			if (m_engine_model->are_frequency_limits_loaded()) {
				
				ImGui::Text("Current Mode:");
				ImGui::SameLine();

				if (ImGui::Button(is_limits_mode_active ? "Limits Mode" : "General Mode")) {
					m_engine_model->set_next_visualization_mode();
				}
			}

			if (is_limits_mode_active)
				draw_limits_mode_color_selection();
			else
				draw_normal_color_selection();
		}

		ImGui::End();
	}
}

void ImGuiManager::draw_normal_color_selection()
{
	draw_gradient_selection("General Mode Gradient", *m_engine_model->get_gradient(EngineModel::GradientVariables::NORMAL_MODE_GRADIENT));
}

void ImGuiManager::draw_limits_mode_color_selection()
{
	draw_color_selection("Safe Zone Color", *m_engine_model->get_color(EngineModel::ColorVariables::GOOD_LIMITS_COLOR));
	draw_gradient_selection("Risky Zone Gradient", *m_engine_model->get_gradient(EngineModel::GradientVariables::LIMITS_MODE_RISKY_GRADIENT));
	draw_gradient_selection("Dangerous Zone Gradient", *m_engine_model->get_gradient(EngineModel::GradientVariables::LIMITS_MODE_DANGEROUS_GRADIENT));
}

void ImGuiManager::draw_engine_view()
{
	if(ImGui::Begin("Engine View")) {
		m_graph_manager->draw_legend();

		ImGui::SameLine();
		if (ImGui::BeginChild("Engine Render")) {
			m_scene_view_position = ImGui::GetWindowPos();
			ImVec2 scene_scale = ImGui::GetWindowSize();

			bool is_width_changed = abs(scene_scale.x - m_scene_view_scale.x) > 0;
			bool is_height_changed = abs(scene_scale.y - m_scene_view_scale.y) > 0;

			if ((is_width_changed || is_height_changed) && !is_window_resized(ImGui::GetCurrentWindow())) {
				m_scene_view_scale = scene_scale;
				on_scene_view_scale_changed.invoke({ scene_scale.x, scene_scale.y });
			}
			ImGui::Image((ImTextureID)m_mesh_manager->scene_texture(), scene_scale, ImVec2(0, 1), ImVec2(1, 0));		// invert the V from the UV

			bool is_scene_view_in_focus = ImGui::IsItemHovered();

			if (is_scene_view_in_focus != m_is_hovering_scene_view)
				on_scene_view_focus_changed.invoke(is_scene_view_in_focus);

			m_is_hovering_scene_view = ImGui::IsItemHovered();

			ImVec2 p = m_scene_view_position;
			glm::vec3 text_color = glm::vec3(1) - *m_application_model->get_color(ApplicationModel::ColorVariables::CLEAR_COLOR);
			ImGui::GetWindowDrawList()->AddText(ImGui::GetDefaultFont(), 15, m_scene_view_position, ImGui::GetColorU32({text_color.r, text_color.g, text_color.b, 1}), m_selected_frequencies_text.c_str());
		}
		ImGui::EndChild();

		ImGui::End();
	}
}

void ImGuiManager::draw_general_info_widget()
{
	if (ImGui::Begin("General Info")) {
		draw_fps_and_delta_time();

		ImGui::End();
	}
}

void ImGuiManager::draw_fps_and_delta_time()
{
	static float timer = 0;
	static float fps_refresh_time = 1;
	static float last_avg_delta_time = 0;
	static int count = 0;

	ImGui::DragFloat("Info Refresh Interval", &fps_refresh_time, 0.05, 0, 5);

	float delta_time = App::delta_time;

	timer += delta_time;
	++count;

	if (timer > fps_refresh_time) {
		last_avg_delta_time = timer / count;
		timer = 0;
		count = 0;
	}

	std::string fps_label = "FPS: " + std::to_string((unsigned int)round(1 / last_avg_delta_time));
	std::string delta_time_label = "Delta Time: " + std::to_string(last_avg_delta_time);

	ImGui::Text(fps_label.c_str());
	ImGui::SameLine();
	ImGui::Text(delta_time_label.c_str());
}

void ImGuiManager::draw_main_bar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::Button("Load Vertices")) {
				std::string s = get_file_path({ {".csv files", "csv"} });
				if (s.size() > 0)
					on_load_vertex_positions.invoke(s.c_str());
			}

			if (ImGui::Button("Load Cells")) {
				std::string s = get_file_path({ {".csv files", "csv"} });
				if (s.size() > 0)
					on_load_cell_vertices.invoke(s.c_str());
			}

			if (ImGui::Button("Load Cell Data")) {
				std::string s = get_file_path({ {".csv files", "csv"} });
				if (s.size() > 0)
					on_load_cell_stats.invoke(s.c_str());
			}

			if (ImGui::Button("Load Limits")) {
				std::string s = get_file_path({ {".csv files", "csv"} });
				if (s.size() > 0)
					on_load_frequency_limits.invoke(s.c_str());
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void ImGuiManager::draw_frequency_selection_widget()
{
	if (ImGui::Begin("Frequency Selection")) {
		if (ImGui::BeginTable("split1", 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
		{
			EngineModel::VisualizationMode current_visualization_mode = m_engine_model->current_visualization_mode();
			bool is_limits_mode_active = current_visualization_mode == EngineModel::VisualizationMode::LIMITS;

			const std::vector<std::string>& frequency_names = is_limits_mode_active ? m_frequencies_with_limits : m_frequency_names;

			std::unique_ptr<bool> selected(new bool[frequency_names.size()]);

			for (int i = 0; i < frequency_names.size(); i++)
			{
				selected.get()[i] = m_engine_model->is_frequency_selected(frequency_names[i]);

				ImGui::TableNextColumn();
				ImGui::Selectable(frequency_names[i].c_str(), &selected.get()[i], 1);;

				if (m_engine_model->is_frequency_selected(frequency_names[i]) != selected.get()[i]) {
					m_engine_model->select_frequency(frequency_names[i], selected.get()[i]);
				}
			}

			ImGui::EndTable();
		}
		ImGui::End();
	}
}

void ImGuiManager::draw_frequency_selection_evaluation_settings_widget()
{
	unsigned int num_of_selected_frequencies = m_engine_model->num_of_selected_frequencies();

	if (num_of_selected_frequencies > 0) {
		if (ImGui::Begin("Frequency Selection Evaluation Settings")) {
			
			EngineModel::VisualizationMode current_visualization_mode = m_engine_model->current_visualization_mode();
			bool is_limits_mode_active = current_visualization_mode == EngineModel::VisualizationMode::LIMITS;

			if (!is_limits_mode_active) {
				draw_limits_selection();
				if (num_of_selected_frequencies > 1)
					draw_function_selection();
			}

			if (ImGui::Button("Clear Selected Frequencies"))
				m_engine_model->clear_frequency_selection();

			ImGui::End();
		}
	}
}

void ImGuiManager::draw_limits_selection()
{
	int selected_mode = *m_engine_model->get_uint(EngineModel::UnsignedIntVariables::VIBRATION_LIMITS);
	unsigned int num_of_labels = sizeof(EngineModel::LIMITS_NAMES) / sizeof(*EngineModel::LIMITS_NAMES);

	ImGui::ListBox("Limits Selection", &(selected_mode), EngineModel::LIMITS_NAMES, num_of_labels);

	m_engine_model->set_uint(EngineModel::UnsignedIntVariables::VIBRATION_LIMITS, selected_mode);

	if (selected_mode == (int)EngineModel::VibrationLimitsVariables::USER_DEF) {
		glm::vec2* user_limits = m_engine_model->get_vibration_limits(EngineModel::VibrationLimitsVariables::USER_DEF);
		ImGui::DragFloat2("Custom Limits", glm::value_ptr(*user_limits), 1, -200, 200);
	}
}

void ImGuiManager::draw_gradient_selection(const char* gradient_name, Gradient& g)
{
	ImGui::BeginGroup();
	ImGui::PushItemWidth(-ImGui::GetFontSize() * 15);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImGui::Text(gradient_name);

	glm::vec3* c1 = g.color1_ptr();
	glm::vec3* c2 = g.color2_ptr();

	std::string c1_name("Color 1");
	c1_name += "##" + std::string(gradient_name);

	std::string c2_name("Color 2");
	c2_name += "##" + std::string(gradient_name);

	draw_color_selection(c1_name.c_str(), *c1);
	ImGui::SameLine();
	draw_color_selection(c2_name.c_str(), *c2);

	std::string interpolation_name = g.get_current_interpolation_mode_name();
	interpolation_name += "##";
	interpolation_name += gradient_name;	//have to add this in order to get identical button labels

	ImGui::Text("Gradient sampling method:");
	ImGui::SameLine();
	if (ImGui::Button(interpolation_name.c_str()))
		g.set_next_interpolation_mode();
	ImGui::EndGroup();
}

void ImGuiManager::draw_function_selection()
{
	int selected_function = (int) *m_engine_model->get_uint(EngineModel::UnsignedIntVariables::NORMAL_MODE_FUNCTION);
	unsigned int num_of_labels = sizeof(EngineModel::FUNCTION_NAMES) / sizeof(*EngineModel::FUNCTION_NAMES);

	ImGui::ListBox("Function Selection", &(selected_function), EngineModel::FUNCTION_NAMES, num_of_labels);
	
	m_engine_model->set_uint(EngineModel::UnsignedIntVariables::NORMAL_MODE_FUNCTION, selected_function);
}

void ImGuiManager::draw_graph_widget()
{
	ImGui::SetNextWindowSize({500, 330});

	ImPlotContext* implot_context = ImPlot::GetCurrentContext();
	ImGuiContext* imgui_context = ImGui::GetCurrentContext();
	
	if (ImGui::Begin("Graph View")) {
		if (m_engine_model->num_of_selected_cells() > 0) {
			if (ImGui::Button("CLEAR SELECTED CELLS")) {
				m_engine_model->clear_selected_cells();
			}
		}

		if (ImGui::BeginChild("Graph Child")) {
			ImGuiWindow* current_window = imgui_context->CurrentWindow;

			current_window->HasCloseButton = false;
			current_window->WantCollapseToggle = false;

			m_graph_manager->draw_cell_plot();

			ImGui::EndChild();
		}
	}

	ImGui::End();
}

void ImGuiManager::draw_graph_settings_widget()
{
	if (ImGui::Begin("Graph Settings")) {

		ImGui::Text("Graph Rendering Mode:");
		ImGui::SameLine();
		if (ImGui::Button(m_application_model->current_graph_render_mode_label())) {
			m_application_model->switch_graph_render_mode();
		}

		draw_render_graph_settings();

		ImGui::Text("Comparison Mode:");
		ImGui::SameLine();
		if (ImGui::Button(m_application_model->current_graph_comparison_mode_label())) {
			m_application_model->switch_graph_comparison_mode();
		}

		draw_comparison_graph_settings();

		draw_color_selection("Hovered Cell Graph Color", m_application_model->hovered_cell_graph_color);
	}
	ImGui::End();
}

void ImGuiManager::draw_subplots_comparison_graph_settings()
{
	int num_of_selected_cells = m_engine_model->num_of_selected_cells();

	//if the number of selected cells is greater than 1, than the grid of subplots will have more than 1 subplot
	if (num_of_selected_cells > 1)
		ImGui::DragInt("Number Of Colums", &m_application_model->graph_num_of_columns, 0.1, 1, num_of_selected_cells);
}

void ImGuiManager::draw_relative_comparison_graph_settings()
{
	std::vector<unsigned int> selected_cells_indeces(m_engine_model->selected_cells());
	unsigned int num_of_selected_cells = selected_cells_indeces.size();

	if (ImGui::TreeNode("Pick Referant Selected Cell"))
	{
		static unsigned int selected = 0;
		for (unsigned int i = 0; i < num_of_selected_cells; ++i)
		{
			unsigned int current_cell_index = selected_cells_indeces[i];
			std::string cell_label = "Cell ";
			cell_label += std::to_string(current_cell_index);

			glm::vec3 color = m_engine_model->get_color_for_selected_cell(i);
			ImU32 packed_color = ImGui::GetColorU32({ color.r, color.g, color.b, 1 });

			if (MyImGui::SelectableCustomColor(cell_label.c_str(), selected == current_cell_index, packed_color, packed_color)) {
				//if it's already selected - deselect it!
				if (selected == current_cell_index)
					selected = 0;
				else
					selected = current_cell_index;
				m_application_model->set_graph_referent_cell(selected);
			}
		}
		ImGui::TreePop();
	}
}

void ImGuiManager::draw_bar_graph_settings()
{
	ImGui::DragFloat("Bar Width", &m_application_model->graph_bar_width, 0.05, 0, 1);
}

void ImGuiManager::draw_colormap_legend_widget()
{
	if (ImGui::Begin("##Color Map Legend")) {
		m_graph_manager->draw_legend();
	}
	ImGui::End();
}

bool ImGuiManager::is_window_resized(ImGuiWindow* window)
{
	ImGuiID active_id = ImGui::GetActiveID();

	for (int i = 0; i < 4; ++i) {
		ImGuiID border_id = ImGui::GetWindowResizeBorderID(window, i);
		if (active_id == border_id)
			return true;
		ImGuiID corner_id = ImGui::GetWindowResizeCornerID(window, i);
		if (active_id == corner_id)
			return true;
	}
	
	return false;
}

void ImGuiManager::delete_selected_cells_palletes()
{
	for (auto& pair : m_selected_cells_palletes_textures)
		glDeleteTextures(1, &pair.second);

	m_selected_cells_palletes_textures.clear();
}

unsigned int ImGuiManager::generate_texture_from_pallete(const data::pallete& p)
{
	unsigned int texture_id;
	//generate texture for pixel buffering
	glGenTextures(1, &texture_id);

	std::vector<unsigned char> data;

	//save pallete colors as vector of unsigned bytes
	for (const glm::vec3& color : p.second)
		for (int i = 0; i < 3; ++i)
			data.push_back(255 * color[i]);

	glBindTexture(GL_TEXTURE_2D, texture_id); CHECK_OPENGL;

	//set min and mag fiters to nearest to not look blurry in ui
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); CHECK_OPENGL;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); CHECK_OPENGL;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, p.second.size(), 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &data[0]); CHECK_OPENGL;

	glBindTexture(GL_TEXTURE_2D, 0); CHECK_OPENGL;

	return texture_id;
}

void ImGuiManager::draw_textured_button(const char* button_text, unsigned int texture_id, const ImVec2& button_size, std::function<void(void)> button_callback)
{
	ImVec2 size = button_size;                     // Size of the image we want to make visible

	if (ImGui::ImageButton((void*) texture_id, size))	//-1 padding - use default padding
		button_callback();
	ImGui::SameLine();
	ImGui::Text(button_text);
}

ImGuiManager::ImGuiManager(ApplicationModel* application_model, EngineModel* engine_model, MeshManager* mesh_manager, GraphManager* graph_manager, GLFWwindow* window, bool is_dark)
{
	m_window = window;

	m_application_model = application_model;
	m_engine_model = engine_model;
	
	m_mesh_manager = mesh_manager;
	m_graph_manager = graph_manager;

	m_is_hovering_scene_view = true;

	m_engine_model->on_cell_stats_loaded.add_member_listener(&ImGuiManager::cell_stats_loaded, this);
	m_engine_model->on_frequency_limits_loaded.add_member_listener(&ImGuiManager::frequency_limits_loaded, this);
	m_engine_model->on_selected_cells_palletes_loaded.add_member_listener(&ImGuiManager::selected_cells_palletes_loaded, this);
	m_engine_model->on_selected_frequencies_changed.add_member_listener(&ImGuiManager::selected_frequencies_changed, this);

	if (m_engine_model->are_selected_cells_palletes_loaded())
		selected_cells_palletes_loaded();

	//Initialize functions arrays
	m_render_graph_settings_functions = {
		std::bind(&ImGuiManager::draw_bar_graph_settings, this),	//BARS
		[]() {}													//LINES - doesn't have any settings to draw, so we pass in an empty lambda
	};

	m_comparison_graph_settings_functions = {
		[]() {},																//DEFAULT - doesn't have any settings to draw, so we pass in empty lambda
		std::bind(&ImGuiManager::draw_subplots_comparison_graph_settings, this),	//SUBPLOTS
		std::bind(&ImGuiManager::draw_relative_comparison_graph_settings, this)	//RELATIVE
	};
}

ImGuiManager::~ImGuiManager()
{
	delete_selected_cells_palletes();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::update()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	draw_graph_widget();
	draw_graph_settings_widget();

	draw_general_info_widget();

	draw_color_selection_widget();

	if (m_frequency_names.size() > 0) {
		draw_frequency_selection_evaluation_settings_widget();
		draw_frequency_selection_widget();
	}

	draw_main_bar();

	draw_engine_view();

	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

bool ImGuiManager::handle_mouse_scroll(double x_offset, double y_offset)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent((float)x_offset, (float)y_offset);

	//won't check if mouse is captured because mouse will be captured over scene view and we want to handle that in backend
	return !m_is_hovering_scene_view;
}

bool ImGuiManager::handle_mouse_pos(double x_pos, double y_pos)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		int window_x, window_y;
		glfwGetWindowPos(m_window, &window_x, &window_y);
		x_pos += window_x;
		y_pos += window_y;
	}
	io.AddMousePosEvent((float)x_pos, (float)y_pos);

	//won't check if mouse is captured because mouse will be captured over scene view and we want to handle that in backend
	return !m_is_hovering_scene_view;
}

bool ImGuiManager::handle_mouse_click(int button, bool down)
{
	ImGuiIO& io = ImGui::GetIO();
	if (button >= 0 && button < ImGuiMouseButton_COUNT)
		io.AddMouseButtonEvent(button, down);

	//won't check if mouse is captured because mouse will be captured over scene view and we want to handle that in backend
	return !m_is_hovering_scene_view;
}

void ImGuiManager::cell_stats_loaded()
{
	m_frequency_names = m_engine_model->frequency_names();
}

glm::ivec2 ImGuiManager::get_scene_view_space_mouse_pos(const glm::ivec2& mouse_pos)
{
	int x = mouse_pos.x - m_scene_view_position.x;
	int y = mouse_pos.y - m_scene_view_position.y;

	return glm::ivec2(x, y);
}

void ImGuiManager::selected_cells_palletes_loaded()
{
	delete_selected_cells_palletes();

	const auto& palletes = m_engine_model->selected_cells_palletes();

	m_selected_cells_palletes_textures.resize(palletes.size());

	for (int i = 0; i < palletes.size(); ++i) {
		m_selected_cells_palletes_textures[i].first = palletes[i].first;
		m_selected_cells_palletes_textures[i].second = generate_texture_from_pallete(palletes[i]);
	}
}

void ImGuiManager::selected_frequencies_changed()
{
	m_selected_frequencies_text = "SELECTED FREQUENCIES:\n";

	for (unsigned int f_index : m_engine_model->selected_frequencies_indeces())
		m_selected_frequencies_text += (m_frequency_names[f_index] + "\n");
}

void ImGuiManager::handle_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

std::string get_file_path(std::initializer_list<nfdfilteritem_t> filter_items)
{
	nfdchar_t* model_path;
	nfdresult_t result = NFD_OpenDialog(&model_path, filter_items.begin(), filter_items.size(), NULL);

	if (result == NFD_OKAY) {
		std::string correct_path = fix_path(model_path);
		NFD_FreePath(model_path);
		return correct_path;
	}
	else {
		return "";
	}
}

std::string fix_path(const std::string path) {
	std::string result(path);
	size_t pos;
	while ((pos = result.find("\\")) != std::string::npos)
		result.replace(pos, 1, "/");

	return result;
}