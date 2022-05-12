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
	ImGui::Begin("Color Properties Selection");

	draw_color_selection("Background Color", *m_application_model->get_color(ApplicationModel::ColorVariables::CLEAR_COLOR));
	
	draw_color_selection("Default Color", *m_application_model->get_engine_data_color(EngineData::ColorVariables::DEFAULT_COLOR));
	
	if (m_application_model->is_hover_mode_active())
		draw_color_selection("Hovered Cell Stats Color", *m_application_model->get_engine_data_color(EngineData::ColorVariables::HOVERED_CELL_STATS_COLOR));

	if (m_application_model->are_stats_loaded()) {
		bool is_limits_mode_active = m_application_model->is_limits_mode_active();

		if (m_application_model->are_frequency_limits_loaded()) {

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

void ImGUILayer::draw_normal_color_selection()
{
	draw_gradient_selection("Normal Mode Gradient", *m_application_model->get_engine_data_gradient(EngineData::GradientVariables::NORMAL_MODE_GRADIENT));
}

void ImGUILayer::draw_limits_mode_color_selection()
{
	draw_color_selection("Good Color", *m_application_model->get_engine_data_color(EngineData::ColorVariables::GOOD_LIMITS_COLOR));
	draw_gradient_selection("Mid Gradient", *m_application_model->get_engine_data_gradient(EngineData::GradientVariables::LIMITS_MODE_MID_GRADIENT));
	draw_gradient_selection("Bad Gradient", *m_application_model->get_engine_data_gradient(EngineData::GradientVariables::LIMITS_MODE_BAD_GRADIENT));
}

void ImGUILayer::draw_engine_view()
{
	ImGui::Begin("Engine View");
	{
		ImGui::BeginChild("Engine Render");
		m_scene_view_position = ImGui::GetWindowPos();
		ImVec2 scene_scale = ImGui::GetWindowSize();

		bool is_width_changed = abs(scene_scale.x - m_scene_view_scale.x) > 0;
		bool is_height_changed = abs(scene_scale.y - m_scene_view_scale.y) > 0;

		if ((is_width_changed || is_height_changed) && !is_window_resized(ImGui::GetCurrentWindow())) {
			m_scene_view_scale = scene_scale;
			on_scene_view_scale_changed.invoke({ scene_scale.x, scene_scale.y });
		}
		ImGui::Image((ImTextureID)m_scene_view_texture, scene_scale, ImVec2(0, 1), ImVec2(1, 0));		// invert the V from the UV

		m_is_hovering_scene_view = ImGui::IsItemHovered();
		ImGui::EndChild();
	}
	ImGui::End();
}

void ImGUILayer::draw_general_info_widget()
{
	ImGui::Begin("General Info");
	draw_fps_and_delta_time();

	ImGui::End();
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
	ImGui::Begin("Frequency Selection");

	if (ImGui::BeginTable("split1", 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
	{
		bool is_limits_mode_active = m_application_model->is_limits_mode_active();
		const std::vector<std::string>& frequency_names = is_limits_mode_active ? m_frequencies_with_limits : m_frequenzy_names;

		std::unique_ptr<bool> selected(new bool[frequency_names.size()]);

		for (int i = 0; i < frequency_names.size(); i++)
		{
			selected.get()[i] = m_application_model->is_frequency_selected(frequency_names[i]);

			ImGui::TableNextColumn();
			ImGui::Selectable(frequency_names[i].c_str(), &selected.get()[i], 1);;

			if (m_application_model->is_frequency_selected(frequency_names[i]) != selected.get()[i]) {
				m_application_model->select_frequency(frequency_names[i], selected.get()[i]);
			}
		}

		ImGui::EndTable();
	}
	ImGui::End();
}

void ImGUILayer::draw_legend_bar_widget()
{
}

void ImGUILayer::draw_frequency_selection_evaluation_settings_widget()
{
	unsigned int num_of_selected_frequencies = m_application_model->num_of_selected_frequencies();

	if (num_of_selected_frequencies > 0) {
		if (ImGui::Begin("Frequency Selection Evaluation Settings")) {
			if (!m_application_model->is_limits_mode_active()) {
				draw_limits_selection();
				if (num_of_selected_frequencies > 1)
					draw_function_selection();
			}

			if (ImGui::Button("Clear Selection"))
				m_application_model->clear_frequenzy_selection();

			ImGui::End();
		}
	}
}

void ImGUILayer::draw_limits_selection()
{
	int selected_mode = *m_application_model->get_engine_data_uint(EngineData::UnsignedIntVariables::NORMAL_MODE_LIMITS);
	unsigned int num_of_labels = sizeof(EngineData::LIMITS_NAMES) / sizeof(*EngineData::LIMITS_NAMES);

	ImGui::ListBox("Limits Selection", &(selected_mode), EngineData::LIMITS_NAMES, num_of_labels, 2);

	m_application_model->set_engine_data_uint(EngineData::UnsignedIntVariables::NORMAL_MODE_LIMITS, selected_mode);

	if (selected_mode == (int)EngineData::NormalModeLimitsVariables::USER_DEF) {
		glm::vec2* user_limits = m_application_model->get_engine_data_normal_mode_limits(EngineData::NormalModeLimitsVariables::USER_DEF);
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
		ImGui::InvisibleButton("##gradient1", gradient_size);
	}
}

void ImGUILayer::draw_function_selection()
{
	int selected_function = (int) * m_application_model->get_engine_data_uint(EngineData::UnsignedIntVariables::NORMAL_MODE_FUNCTION);
	unsigned int num_of_labels = sizeof(EngineData::FUNCTION_NAMES) / sizeof(*EngineData::FUNCTION_NAMES);

	ImGui::ListBox("Function Selection", &(selected_function), EngineData::FUNCTION_NAMES, num_of_labels, 2);
	
	m_application_model->set_engine_data_uint(EngineData::UnsignedIntVariables::NORMAL_MODE_FUNCTION, selected_function);
}

void ImGUILayer::draw_graph_tooltip()
{
	ImGui::SetNextWindowSize({500, 330});

	ImPlotContext* implot_context = ImPlot::GetCurrentContext();
	ImGuiContext* imgui_context = ImGui::GetCurrentContext();
	
	ImGui::Begin("Graph");
	
	bool is_hover_mode_active = m_application_model->is_hover_mode_active();
	if (ImGui::Button((is_hover_mode_active ? "HOVER MODE" : "CELL SELECTION MODE")))		//if button is clicked flip the flag
	{
		m_application_model->set_is_hover_mode_active(!is_hover_mode_active);
		is_hover_mode_active = !is_hover_mode_active;
	}

	if (!is_hover_mode_active) {
		ImGui::SameLine();
		if(ImGui::Button("CLEAR SELECTED CELLS")) {
			m_application_model->clear_selected_cells();
		}
	}

	ImGui::BeginChild("Graph Child");
	ImGuiWindow* current_window = imgui_context->CurrentWindow;

	current_window->HasCloseButton = false;
	current_window->WantCollapseToggle = false;

	if (ImPlot::BeginPlot("Selected Cell Frequencies")) {
		ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
		ImPlot::SetupAxes("Frequency", "Vibrations", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);

		if (m_graph_data.positions.size() > 0) {
			ImPlot::SetupAxisTicks(ImAxis_X1, &m_graph_data.positions[0], m_graph_data.groups, &m_graph_data.group_labels[0]);
			ImPlot::PlotBarGroups(&m_graph_data.item_labels[0], &m_graph_data.plot_data[0], m_graph_data.items, m_graph_data.groups, m_graph_data.size, 0, 0);
			MyImPlot::PlotBarGroups(m_graph_data);
		}

		ImPlot::EndPlot();
	}
	ImGui::EndChild();
	ImGui::End();
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

ImGUILayer::ImGUILayer(ApplicationModel* application_model, GLFWwindow* window, const char* version_string, unsigned int scene_view_texture, bool is_dark): 
	m_window(window), m_graph_data({}, {})
{
	m_application_model = application_model;
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

	m_application_model->on_cell_stats_loaded.add_member_listener(&ImGUILayer::cell_stats_loaded, this);
	m_application_model->on_frequency_limits_loaded.add_member_listener(&ImGUILayer::frequency_limits_loaded, this);
	m_application_model->on_graph_data_changed.add_member_listener(&ImGUILayer::on_graph_changed, this);
}

ImGUILayer::~ImGUILayer()
{
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

	draw_graph_tooltip();

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
	m_frequenzy_names = m_application_model->frequenzy_names();
}

glm::ivec2 ImGUILayer::get_scene_view_space_mouse_pos(const glm::ivec2& mouse_pos)
{
	int x = mouse_pos.x - m_scene_view_position.x;
	int y = mouse_pos.y - m_scene_view_position.y;

	return glm::ivec2(x, y);
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