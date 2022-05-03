#include "imgui_layer.h"
#include "app.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "implot_internal.h"
#include "nfd.h"

const char* GraphData::item_labels[] = {"Frequencies"};

void ImGUILayer::draw_color_selection_widget()
{
	ImGui::Begin("Color Properties Selection");

	ImGui::ColorEdit3("Default Color", glm::value_ptr(m_application_model->engine_data()->default_color));

	draw_gradient_selection();

	ImGui::ColorEdit3("Background Color", glm::value_ptr(m_application_model->clear_color));

	ImGui::End();
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

		if (is_width_changed || is_height_changed) {
			m_scene_view_scale = scene_scale;

			on_scene_view_scale_changed.invoke({ scene_scale.x, scene_scale.y });
		}
		ImGui::Image((ImTextureID)m_scene_view_texture, scene_scale, ImVec2(0, 1), ImVec2(1, 0));			// invert the V from the UV
		
		m_is_hovering_scene_view = ImGui::IsItemHovered();
		ImGui::EndChild();
	}
	ImGui::End();
}

void ImGUILayer::draw_general_info_widget()
{
	ImGui::Begin("General Info");
	draw_fps_and_delta_time();

	ImGui::DragFloat2("Cell Graph Mouse Delta", &m_mouse_delta.x, 0.5, 10, 50);

	ImGui::DragFloat2("Graph Window Size", &m_plot_size.x, 0.5, 50, 1000);

	ImGui::End();
}

void ImGUILayer::draw_fps_and_delta_time()
{
	static float timer = 0;
	static float fps_refresh_time = 0;
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

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void ImGUILayer::draw_frequency_selection_widget()
{
	static unsigned num_of_frequencies_selected = 0;

	ImGui::Begin("Frequency Selection");
	if (num_of_frequencies_selected > 0) {
		draw_limits_selection();

		if (ImGui::Button("Clear Selection"))
			m_application_model->clear_selection();
	}
	
	if (num_of_frequencies_selected > 1)
		draw_function_selection();

	num_of_frequencies_selected = 0;

	if (ImGui::BeginTable("split1", 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
	{
		bool* selected = new bool[m_frequenzy_names.size()];

		for (int i = 0; i < m_frequenzy_names.size(); i++)
		{
			selected[i] = m_application_model->is_frequency_selected(m_frequenzy_names[i]);

			ImGui::TableNextColumn();
			ImGui::Selectable(m_frequenzy_names[i].c_str(), &selected[i], 1);;
			if (m_application_model->is_frequency_selected(m_frequenzy_names[i]) != selected[i])
				m_application_model->select_frequency(m_frequenzy_names[i], selected[i]);

			num_of_frequencies_selected += selected[i];
		}
		delete[] selected;

		ImGui::EndTable();
	}
	ImGui::End();
}

void ImGUILayer::draw_limits_selection()
{
	int selected_mode = *m_application_model->limits_mode();
	unsigned int num_of_labels = sizeof(EngineData::LIMITS_NAMES) / sizeof(*EngineData::LIMITS_NAMES);

	ImGui::ListBox("Limits Selection", &(selected_mode), EngineData::LIMITS_NAMES, num_of_labels, 2);

	*(m_application_model->limits_mode()) = (Limits)selected_mode;

	if (selected_mode == USER_DEFINED)
		ImGui::DragFloat2("Custom Limits", m_application_model->user_defined_limits(), 1, -200, 200);
}

void ImGUILayer::draw_gradient_selection()
{
	ImGui::PushItemWidth(-ImGui::GetFontSize() * 15);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImGui::Text("Engine Gradient");
	ImGui::ColorEdit3("Low Intensity Color", glm::value_ptr(m_application_model->engine_data()->gradient.color1));
	ImGui::ColorEdit3("High Intensity Color", glm::value_ptr(m_application_model->engine_data()->gradient.color2));

	glm::vec3 c1 = m_application_model->engine_data()->gradient.color1;
	glm::vec3 c2 = m_application_model->engine_data()->gradient.color2;

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
	int selected_function = *m_application_model->selected_function();
	unsigned int num_of_labels = sizeof(EngineData::FUNCTION_NAMES) / sizeof(*EngineData::FUNCTION_NAMES);

	ImGui::ListBox("Function Selection", &(selected_function), EngineData::FUNCTION_NAMES, num_of_labels, 2);

	*(m_application_model->selected_function()) = (CellFunctions)selected_function;
}

void ImGUILayer::draw_graph_tooltip()
{
	ImGui::SetNextWindowPos(ImGui::GetMousePos() + m_mouse_delta);
	ImGui::SetNextWindowSize({500, 330});

	ImPlotContext* implot_context = ImPlot::GetCurrentContext();
	ImGuiContext* imgui_context = ImGui::GetCurrentContext();
	
	ImGui::Begin("Graph");
	ImGui::BeginChild("");
	ImGuiWindow* current_window = imgui_context->CurrentWindow;

	current_window->HasCloseButton = false;
	current_window->WantCollapseToggle = false;

	if (ImPlot::BeginPlot("Selected Cell Frequencies")) {
		ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);

		ImPlot::SetupAxes("Frequency", "Vibrations", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
		ImPlot::SetupAxisTicks(ImAxis_X1, &m_graph_data.positions[0], m_graph_data.groups, &m_graph_data.frequenzy_labels[0]);
		ImPlot::PlotBarGroups(GraphData::item_labels, &m_graph_data.plot_data[0], 1, m_graph_data.groups, m_graph_data.size, 0, 0);

		ImPlot::EndPlot();
	}
	ImGui::EndChild();
	ImGui::End();
}

ImGUILayer::ImGUILayer(ApplicationModel* application_model, GLFWwindow* window, const char* version_string, unsigned int scene_view_texture, bool is_dark): 
	m_window(window), m_graph_data({})
{
	m_application_model = application_model;

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

	m_mouse_delta = { 10, 10 };
	m_plot_size = { 500, 300 };

	m_application_model->on_cell_stats_loaded.add_member_listener(&ImGUILayer::cell_stats_loaded, this);
	m_application_model->on_cell_selected.add_member_listener(&ImGUILayer::on_cell_selected, this);
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

	if (m_application_model->is_valid_cell_selected() && m_application_model->num_of_selected_frequencies() >= 2)
		draw_graph_tooltip();

	draw_general_info_widget();

	draw_color_selection_widget();

	if (m_frequenzy_names.size() > 0)
		draw_frequency_selection_widget();

	draw_main_bar();

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

	return io.WantCaptureMouse;
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

	return io.WantCaptureMouse;
}

bool ImGUILayer::handle_mouse_click(int button, bool down)
{
	ImGuiIO& io = ImGui::GetIO();
	if (button >= 0 && button < ImGuiMouseButton_COUNT)
		io.AddMouseButtonEvent(button, down);

	return io.WantCaptureMouse;
}

void ImGUILayer::cell_stats_loaded()
{
	m_frequenzy_names = m_application_model->frequenzy_names();
}

void ImGUILayer::on_cell_selected(unsigned int cell_index)
{
	if (m_application_model->is_valid_cell_selected())
		m_graph_data = m_application_model->get_selected_cell_values();
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

GraphData::GraphData(const std::vector<std::pair<std::string, float>>& data)
{
	groups = data.size();

	for (int i = 0; i < groups; ++i) {
		const std::string& name = data[i].first;
		
		frequenzy_labels.push_back(new char[name.size() + 1]);
		std::copy(name.begin(), name.end(), frequenzy_labels[i]);
		frequenzy_labels[i][name.size()] = '\0';

		plot_data.push_back(data[i].second);
		positions.push_back(i);
	}

	size = 0.5;
}

GraphData::GraphData(const GraphData& gd)
{
	groups = gd.groups;

	for (int i = 0; i < groups; ++i) {
		const std::string name = gd.frequenzy_labels[i];

		frequenzy_labels.push_back(new char[name.size() + 1]);
		std::copy(name.begin(), name.end(), frequenzy_labels[i]);
		frequenzy_labels[i][name.size()] = '\0';

		plot_data.push_back(gd.plot_data[i]);
		positions.push_back(i);
	}

	size = gd.size;
}

GraphData::~GraphData()
{
	for (char* label : frequenzy_labels)
		delete[] label;
}

void GraphData::operator=(const GraphData& gd)
{
	for (char* label : frequenzy_labels)
		delete[] label;

	frequenzy_labels.clear();
	plot_data.clear();
	positions.clear();

	groups = gd.groups;

	for (int i = 0; i < groups; ++i) {
		const std::string name = gd.frequenzy_labels[i];

		frequenzy_labels.push_back(new char[name.size() + 1]);
		std::copy(name.begin(), name.end(), frequenzy_labels[i]);
		frequenzy_labels[i][name.size()] = '\0';

		plot_data.push_back(gd.plot_data[i]);
		positions.push_back(i);
	}

	size = gd.size;
}