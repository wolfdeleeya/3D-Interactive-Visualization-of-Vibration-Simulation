#include <iostream>

#include "imgui_layer.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "nfd.h"

void ImGUILayer::draw_color_selection_widget()
{
	ImGui::Begin("Color Properties Selection");

	ImGui::ColorEdit3("Default Color", glm::value_ptr(m_application_model->engine_data()->default_color));

	ImGui::End();
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
	const char* arr[] = { "a", "b"};
	if (num_of_frequencies_selected > 0) {
		draw_limits_selection();
	}
	
	if (num_of_frequencies_selected > 1) {
		ImGui::Text("More than 1 frequency is selected!");
	}
	num_of_frequencies_selected = 0;

	if (ImGui::BeginTable("split1", 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
	{
		bool* selected = new bool[m_frequenzy_names.size()];
		std::vector<bool>& selected_attributes = m_application_model->selected_attributes;
		for (int i = 0; i < m_frequenzy_names.size(); i++)
		{
			selected[i] = selected_attributes[i];

			ImGui::TableNextColumn();
			ImGui::Selectable(m_frequenzy_names[i].c_str(), &selected[i]);
			if (selected_attributes[i] != selected[i])
				std::cout << m_frequenzy_names[i] << " je " << selected[i] << std::endl;

			if (selected_attributes[i] != selected[i]) {
				m_application_model->engine_data()->select_frequency(m_frequenzy_names[i], selected[i]);
			}
			selected_attributes[i] = selected[i];

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

ImGUILayer::ImGUILayer(ApplicationModel* application_model, GLFWwindow* window, const char* version_string, bool is_dark): m_window(window)
{
	m_application_model = application_model;
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	m_application_model->on_cell_stats_loaded.add_member_listener(&ImGUILayer::cell_stats_loaded, this);
}

void ImGUILayer::update()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
	
	draw_main_bar();

	draw_color_selection_widget();

	if (m_frequenzy_names.size() > 0)
		draw_frequency_selection_widget();

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