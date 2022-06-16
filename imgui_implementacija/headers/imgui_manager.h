#pragma once
#include <iostream>
#include <vector>

#include "application_model.h"
#include "GLFW/glfw3.h"
#include "nfd.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "graph/graph_manager.h"
#include "mesh_manager.h"

class ImGuiManager {
private:
	GLFWwindow* m_window; 

	std::vector<std::string> m_frequenzy_names;
	std::vector<std::string> m_frequencies_with_limits;

	std::string m_selected_frequencies_text;

	//MODEL
	ApplicationModel* m_application_model;
	EngineModel* m_engine_model;

	//VIEW
	MeshManager* m_mesh_manager;
	GraphManager* m_graph_manager;

	ImVec2 m_scene_view_position;
	ImVec2 m_scene_view_scale;

	std::vector<std::pair<std::string, unsigned int>> m_selected_cells_palletes_textures;

	bool m_is_hovering_scene_view;

	std::vector<std::function<void(void)>> m_comparison_graph_settings_functions;

	std::vector<std::function<void(void)>> m_render_graph_settings_functions;

	void draw_color_selection_widget();

	void draw_normal_color_selection();

	void draw_limits_mode_color_selection();

	void draw_engine_view();

	void draw_general_info_widget();

	void draw_fps_and_delta_time();

	void draw_main_bar();

	void draw_frequency_selection_widget();

	void draw_frequency_selection_evaluation_settings_widget();

	void draw_limits_selection();

	void draw_gradient_selection(const char* gradient_name, Gradient& g);

	void draw_function_selection();

	void draw_graph_widget();

	void draw_graph_settings_widget();

	void draw_comparison_graph_settings() { m_comparison_graph_settings_functions[unsigned int(m_graph_manager->current_comparison_mode())](); }

	//specific comparison graph settings

	void draw_subplots_comparison_graph_settings();

	void draw_relative_comparison_graph_settings();

	void draw_render_graph_settings() { m_render_graph_settings_functions[unsigned int(m_graph_manager->current_render_mode())](); }

	//specific graph render settings methods

	void draw_bar_graph_settings();

	void draw_colormap_legend_widget();

	bool is_window_resized(ImGuiWindow* window);

	void delete_selected_cells_palletes();

	static unsigned int generate_texture_from_pallete(const data::pallete& p);

	void draw_textured_button(const char* button_text, unsigned int texture_id, const ImVec2& button_size, std::function<void(void)> button_callback);

	void draw_color_selection(const char* color_variable_name, glm::vec3& color) {
		ImGui::ColorEdit3(color_variable_name, glm::value_ptr(color), ImGuiColorEditFlags_NoInputs);
	}

public:
	Event<bool> on_scene_view_focus_changed;

	Event<const char*> on_load_vertex_positions,
		on_load_cell_vertices,
		on_load_cell_stats,
		on_load_frequency_limits;

	Event<const glm::ivec2&> on_scene_view_scale_changed;

	ImGuiManager(ApplicationModel* application_model, EngineModel* engine_model, MeshManager* mesh_manager, GraphManager* graph_manager, GLFWwindow* window, bool is_dark = true);
	
	~ImGuiManager();

	void update();

	bool handle_mouse_scroll(double x_offset, double y_offset);
	
	bool handle_mouse_pos(double x_pos, double y_pos);
	
	bool handle_mouse_click(int button, bool down);

	void handle_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void cell_stats_loaded();

	glm::ivec2 get_scene_view_space_mouse_pos(const glm::ivec2& mouse_pos);

	void selected_cells_palletes_loaded();

	void selected_frequencies_changed();

	void frequency_limits_loaded() { m_frequencies_with_limits = m_engine_model->frequencies_with_limits(); }

	ImVec2 scene_view_scale() { return m_scene_view_scale; }
};

std::string get_file_path(std::initializer_list<nfdfilteritem_t> filter_items);

std::string fix_path(const std::string path);