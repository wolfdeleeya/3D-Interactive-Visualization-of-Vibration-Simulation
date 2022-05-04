#pragma once

#include <vector>

#include "application_model.h"
#include "GLFW/glfw3.h"
#include "nfd.h"
#include "imgui.h"
#include "imgui_internal.h"

struct GraphData {
	static const char* item_labels[];
	std::vector<char*> frequenzy_labels;
	std::vector<float> plot_data;
	std::vector<double> positions;

	int groups = 0;
	float size;
	
	GraphData(const std::vector<std::pair<std::string, float>>& data);
	GraphData(const GraphData& gd);
	~GraphData();
	void operator=(const GraphData& gd);
};

class ImGUILayer {
private:
	GLFWwindow* m_window;
	std::vector<std::string> m_frequenzy_names;
	ApplicationModel* m_application_model;
	ImVec2 m_mouse_delta;
	ImVec2 m_plot_size;

	ImVec2 m_scene_view_position;
	ImVec2 m_scene_view_scale;
	unsigned int m_scene_view_texture;

	bool m_is_hovering_scene_view;

	GraphData m_graph_data;

	void draw_color_selection_widget();

	void draw_engine_view();

	void draw_general_info_widget();

	void draw_fps_and_delta_time();
	
	void draw_main_bar();

	void draw_frequency_selection_widget();

	void draw_limits_selection();

	void draw_gradient_selection();

	void draw_function_selection();

	void draw_graph_tooltip();

	bool is_window_resized(ImGuiWindow* window);

public:
	Event<const char*> on_load_vertex_positions,
		on_load_cell_vertices,
		on_load_cell_stats;

	Event<const glm::ivec2&> on_scene_view_scale_changed;

	ImGUILayer(ApplicationModel* application_model, GLFWwindow* window, const char* version_string, unsigned int scene_view_texture, bool is_dark = true);
	
	~ImGUILayer();

	void update();

	bool handle_mouse_scroll(double x_offset, double y_offset);
	
	bool handle_mouse_pos(double x_pos, double y_pos);
	
	bool handle_mouse_click(int button, bool down);

	void cell_stats_loaded();

	void on_cell_selected(unsigned int cell_index);

	glm::ivec2 get_scene_view_space_mouse_pos(const glm::ivec2& mouse_pos);

	ImVec2 scene_view_scale() { return m_scene_view_scale; }
};

std::string get_file_path(std::initializer_list<nfdfilteritem_t> filter_items);

std::string fix_path(const std::string path);