#pragma once
#include <iostream>
#include <vector>

#include "application_model.h"
#include "GLFW/glfw3.h"
#include "nfd.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "graph_data.h"

class ImGUILayer {
private:
	GLFWwindow* m_window;
	std::vector<std::string> m_frequenzy_names;
	std::vector<std::string> m_frequencies_with_limits;

	ApplicationModel* m_application_model;

	ImVec2 m_scene_view_position;
	ImVec2 m_scene_view_scale;
	unsigned int m_scene_view_texture;

	bool m_is_hovering_scene_view;

	GraphData m_graph_data;

	void draw_color_selection_widget();

	void draw_normal_color_selection();

	void draw_limits_mode_color_selection();

	void draw_engine_view();

	void draw_general_info_widget();

	void draw_fps_and_delta_time();
	
	void draw_color_selection(const char* color_variable_name, glm::vec3& color) {
		ImGui::ColorEdit3(color_variable_name, glm::value_ptr(color));
	}

	void draw_main_bar();

	void draw_frequency_selection_widget();

	void draw_legend_bar_widget();

	void draw_frequency_selection_evaluation_settings_widget();

	void draw_limits_selection();

	void draw_gradient_selection(const char* gradient_name, Gradient& g);

	void draw_function_selection();

	void draw_graph_tooltip();

	bool is_window_resized(ImGuiWindow* window);

public:
	Event<const char*> on_load_vertex_positions,
		on_load_cell_vertices,
		on_load_cell_stats,
		on_load_frequency_limits;

	Event<const glm::ivec2&> on_scene_view_scale_changed;

	ImGUILayer(ApplicationModel* application_model, GLFWwindow* window, const char* version_string, unsigned int scene_view_texture, bool is_dark = true);
	
	~ImGUILayer();

	void update();

	bool handle_mouse_scroll(double x_offset, double y_offset);
	
	bool handle_mouse_pos(double x_pos, double y_pos);
	
	bool handle_mouse_click(int button, bool down);

	void cell_stats_loaded();

	glm::ivec2 get_scene_view_space_mouse_pos(const glm::ivec2& mouse_pos);

	void frequency_limits_loaded() { m_frequencies_with_limits = m_application_model->frequencies_with_limits(); }

	void on_graph_changed(const GraphData& gd) { m_graph_data = gd; }

	ImVec2 scene_view_scale() { return m_scene_view_scale; }
};

std::string get_file_path(std::initializer_list<nfdfilteritem_t> filter_items);

std::string fix_path(const std::string path);