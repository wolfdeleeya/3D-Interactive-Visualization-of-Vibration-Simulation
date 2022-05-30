#include <memory>

#include "imgui_layer.h"
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

void ImGUILayer::draw_color_selection_widget()
{
	if (ImGui::Begin("Color Properties Selection")) {
		draw_color_selection("Background Color", *m_application_model->get_color(ApplicationModel::ColorVariables::CLEAR_COLOR));

		draw_color_selection("Default Color", *m_engine_data->get_color(EngineData::ColorVariables::DEFAULT_COLOR));

		unsigned int current_selected_cells_pallete_index = m_engine_data->current_selected_cells_pallete_index();

		auto& current_pallete_data = m_selected_cells_palletes_textures[current_selected_cells_pallete_index];

		EngineData* engine_data = m_engine_data;
		draw_textured_button(current_pallete_data.first.c_str(), current_pallete_data.second, { 32, 32 }, 
			[engine_data]() {engine_data->set_next_selected_cells_pallete(); }); CHECK_OPENGL;

		if (ImGui::Button("Set Next Selected Cells Color Pallete")) {
			m_engine_data->set_next_selected_cells_pallete();
		}

		if (m_engine_data->are_stats_loaded()) {
			bool is_limits_mode_active = m_application_model->is_limits_mode_active();

			if (m_engine_data->are_frequenzy_limits_loaded()) {

				if (ImGui::Button(is_limits_mode_active ? "Limits Mode" : "Normal Mode")) {
					m_application_model->set_is_limits_mode_active(!is_limits_mode_active);
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

void ImGUILayer::draw_normal_color_selection()
{
	draw_gradient_selection("Normal Mode Gradient", *m_engine_data->get_gradient(EngineData::GradientVariables::NORMAL_MODE_GRADIENT));
}

void ImGUILayer::draw_limits_mode_color_selection()
{
	draw_color_selection("Good Color", *m_engine_data->get_color(EngineData::ColorVariables::GOOD_LIMITS_COLOR));
	draw_gradient_selection("Mid Gradient", *m_engine_data->get_gradient(EngineData::GradientVariables::LIMITS_MODE_MID_GRADIENT));
	draw_gradient_selection("Bad Gradient", *m_engine_data->get_gradient(EngineData::GradientVariables::LIMITS_MODE_BAD_GRADIENT));
}

void ImGUILayer::draw_engine_view()
{
	if(ImGui::Begin("Engine View")) {
		if (ImGui::BeginChild("##Colormap")) {
			m_graph_manager->draw_legend();
		}
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
			ImGui::Image((ImTextureID)m_scene_view_texture, scene_scale, ImVec2(0, 1), ImVec2(1, 0));		// invert the V from the UV

			bool is_scene_view_in_focus = ImGui::IsItemHovered();

			if (is_scene_view_in_focus != m_is_hovering_scene_view)
				on_scene_view_focus_changed.invoke(is_scene_view_in_focus);

			m_is_hovering_scene_view = ImGui::IsItemHovered();

			ImGui::EndChild();
		}

		ImGui::End();
	}
}

void ImGUILayer::draw_general_info_widget()
{
	if (ImGui::Begin("General Info")) {
		draw_fps_and_delta_time();

		ImGui::End();
	}
}

void ImGUILayer::draw_fps_and_delta_time()
{
	static float timer = 0;
	static float fps_refresh_time = 1;
	static float last_avg_delta_time = 0;
	static int count = 0;

	ImGui::DragFloat("FPS Refresh Time", &fps_refresh_time, 0.05, 0, 5);

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

void ImGUILayer::draw_main_bar()
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

void ImGUILayer::draw_frequency_selection_widget()
{
	if (ImGui::Begin("Frequency Selection")) {
		if (ImGui::BeginTable("split1", 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
		{
			bool is_limits_mode_active = m_application_model->is_limits_mode_active();
			const std::vector<std::string>& frequency_names = is_limits_mode_active ? m_frequencies_with_limits : m_frequenzy_names;

			std::unique_ptr<bool> selected(new bool[frequency_names.size()]);

			for (int i = 0; i < frequency_names.size(); i++)
			{
				selected.get()[i] = m_engine_data->is_frequency_selected(frequency_names[i]);

				ImGui::TableNextColumn();
				ImGui::Selectable(frequency_names[i].c_str(), &selected.get()[i], 1);;

				if (m_engine_data->is_frequency_selected(frequency_names[i]) != selected.get()[i]) {
					m_engine_data->select_frequency(frequency_names[i], selected.get()[i]);
				}
			}

			ImGui::EndTable();
		}
		ImGui::End();
	}
}

void ImGUILayer::draw_frequency_selection_evaluation_settings_widget()
{
	unsigned int num_of_selected_frequencies = m_engine_data->num_of_selected_frequencies();

	if (num_of_selected_frequencies > 0) {
		if (ImGui::Begin("Frequency Selection Evaluation Settings")) {
			if (!m_application_model->is_limits_mode_active()) {
				draw_limits_selection();
				if (num_of_selected_frequencies > 1)
					draw_function_selection();
			}

			if (ImGui::Button("Clear Selection"))
				m_engine_data->clear_frequency_selection();

			ImGui::End();
		}
	}
}

void ImGUILayer::draw_limits_selection()
{
	int selected_mode = *m_engine_data->get_uint(EngineData::UnsignedIntVariables::NORMAL_MODE_LIMITS);
	unsigned int num_of_labels = sizeof(EngineData::LIMITS_NAMES) / sizeof(*EngineData::LIMITS_NAMES);

	ImGui::ListBox("Limits Selection", &(selected_mode), EngineData::LIMITS_NAMES, num_of_labels, 2);

	m_engine_data->set_uint(EngineData::UnsignedIntVariables::NORMAL_MODE_LIMITS, selected_mode);

	if (selected_mode == (int)EngineData::NormalModeLimitsVariables::USER_DEF) {
		glm::vec2* user_limits = m_engine_data->get_normal_mode_limits(EngineData::NormalModeLimitsVariables::USER_DEF);
		ImGui::DragFloat2("Custom Limits", glm::value_ptr(*user_limits), 1, -200, 200);
	}
}

void ImGUILayer::draw_gradient_selection(const char* gradient_name, Gradient& g)
{
	ImGui::PushItemWidth(-ImGui::GetFontSize() * 15);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImGui::Text(gradient_name);

	glm::vec3& c1 = g.color1;
	glm::vec3& c2 = g.color2;

	std::string c1_name(gradient_name);
	c1_name += " Color 1";

	std::string c2_name(gradient_name);
	c2_name += " Color 2";

	draw_color_selection(c1_name.c_str(), c1);
	draw_color_selection(c2_name.c_str(), c2);

	ImGui::PushItemWidth(-ImGui::GetFontSize() * 15);
	ImVec2 gradient_size = ImVec2(ImGui::CalcItemWidth(), ImGui::GetFrameHeight());
	{
		ImVec2 p0 = ImGui::GetCursorScreenPos();
		ImVec2 p1 = ImVec2(p0.x + gradient_size.x, p0.y + gradient_size.y);
		ImU32 col_a = ImGui::GetColorU32(IM_COL32(c1.r * 255, c1.g * 255, c1.b * 255, 255));
		ImU32 col_b = ImGui::GetColorU32(IM_COL32(c2.r * 255, c2.g * 255, c2.b * 255, 255));
		draw_list->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);
		ImGui::InvisibleButton("gradient", gradient_size);
	}
}

void ImGUILayer::draw_function_selection()
{
	int selected_function = (int) *m_engine_data->get_uint(EngineData::UnsignedIntVariables::NORMAL_MODE_FUNCTION);
	unsigned int num_of_labels = sizeof(EngineData::FUNCTION_NAMES) / sizeof(*EngineData::FUNCTION_NAMES);

	ImGui::ListBox("Function Selection", &(selected_function), EngineData::FUNCTION_NAMES, num_of_labels, 2);
	
	m_engine_data->set_uint(EngineData::UnsignedIntVariables::NORMAL_MODE_FUNCTION, selected_function);
}

void ImGUILayer::draw_graph_widget()
{
	ImGui::SetNextWindowSize({500, 330});

	ImPlotContext* implot_context = ImPlot::GetCurrentContext();
	ImGuiContext* imgui_context = ImGui::GetCurrentContext();
	
	if (ImGui::Begin("Graph")) {
		if (m_engine_data->num_of_selected_cells() > 0) {
			if (ImGui::Button("CLEAR SELECTED CELLS")) {
				m_engine_data->clear_selected_cells();
			}
		}

		if (ImGui::BeginChild("Graph Child")) {
			ImGuiWindow* current_window = imgui_context->CurrentWindow;

			current_window->HasCloseButton = false;
			current_window->WantCollapseToggle = false;

			m_graph_manager->draw_cell_plot();

			ImGui::EndChild();
		}

		ImGui::End();
	}
}

void ImGUILayer::draw_graph_settings_widget()
{
	if (ImGui::Begin("Graph Settings")) {
		m_graph_manager->draw_graph_settings();
		ImGui::End();
	}
}

void ImGUILayer::draw_colormap_legend_widget()
{
	if (ImGui::Begin("##Color Map Legend")) {
		m_graph_manager->draw_legend();
		ImGui::End();
	}
}

bool ImGUILayer::is_window_resized(ImGuiWindow* window)
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

void ImGUILayer::delete_selected_cells_palletes()
{
	for (auto& pair : m_selected_cells_palletes_textures)
		glDeleteTextures(1, &pair.second);

	m_selected_cells_palletes_textures.clear();
}

unsigned int ImGUILayer::generate_texture_from_pallete(const data::pallete& p)
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

void ImGUILayer::draw_textured_button(const char* button_text, unsigned int texture_id, const ImVec2& button_size, std::function<void(void)> button_callback)
{
	ImVec2 size = button_size;                     // Size of the image we want to make visible
	ImVec2 uv0 = ImVec2(0, 0);                        // UV coordinates for lower-left
	ImVec2 uv1 = ImVec2(1, 1);								// UV coordinates for (32,32) in our texture
	ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);         // Black background
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // No tint
	if (ImGui::ImageButton((void*) texture_id, size, uv0, uv1, -1, bg_col, tint_col))	//-1 padding - use default padding
		button_callback();

	ImGui::SameLine();
	ImGui::Text(button_text);
}

ImGUILayer::ImGUILayer(ApplicationModel* application_model, EngineData* engine_data, GLFWwindow* window, const char* version_string, unsigned int scene_view_texture, bool is_dark): 
	m_window(window)
{
	m_application_model = application_model;
	m_engine_data = engine_data;
	m_graph_manager = new GraphManager(application_model, engine_data);

	m_is_hovering_scene_view = true;
	m_scene_view_texture = scene_view_texture;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	m_engine_data->on_cell_stats_loaded.add_member_listener(&ImGUILayer::cell_stats_loaded, this);
	m_engine_data->on_frequency_limits_loaded.add_member_listener(&ImGUILayer::frequency_limits_loaded, this);
	m_engine_data->on_selected_cells_palletes_loaded.add_member_listener(&ImGUILayer::selected_cells_palletes_loaded, this);

	if (m_engine_data->are_selected_cells_palletes_loaded())
		selected_cells_palletes_loaded();
}

ImGUILayer::~ImGUILayer()
{
	delete m_graph_manager;
	delete_selected_cells_palletes();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGUILayer::update()
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

	if (m_frequenzy_names.size() > 0) {
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

bool ImGUILayer::handle_mouse_scroll(double x_offset, double y_offset)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent((float)x_offset, (float)y_offset);

	//won't check if mouse is captured because mouse will be captured over scene view and we want to handle that in backend
	return !m_is_hovering_scene_view;
}

bool ImGUILayer::handle_mouse_pos(double x_pos, double y_pos)
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

bool ImGUILayer::handle_mouse_click(int button, bool down)
{
	ImGuiIO& io = ImGui::GetIO();
	if (button >= 0 && button < ImGuiMouseButton_COUNT)
		io.AddMouseButtonEvent(button, down);

	//won't check if mouse is captured because mouse will be captured over scene view and we want to handle that in backend
	return !m_is_hovering_scene_view;
}

void ImGUILayer::cell_stats_loaded()
{
	m_frequenzy_names = m_engine_data->frequenzy_names();
}

glm::ivec2 ImGUILayer::get_scene_view_space_mouse_pos(const glm::ivec2& mouse_pos)
{
	int x = mouse_pos.x - m_scene_view_position.x;
	int y = mouse_pos.y - m_scene_view_position.y;

	return glm::ivec2(x, y);
}

void ImGUILayer::selected_cells_palletes_loaded()
{
	delete_selected_cells_palletes();

	const auto& palletes = m_engine_data->selected_cells_palletes();

	m_selected_cells_palletes_textures.resize(palletes.size());

	for (int i = 0; i < palletes.size(); ++i) {
		m_selected_cells_palletes_textures[i].first = palletes[i].first;
		m_selected_cells_palletes_textures[i].second = generate_texture_from_pallete(palletes[i]);
	}
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