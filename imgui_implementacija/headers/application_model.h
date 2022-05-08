#pragma once

#include "engine_data.h"
#include "camera.h"
#include "events.h"
#include "signals.h"
#include "glm/gtc/type_ptr.hpp"
#include "graph_data.h"

class ApplicationModel {
private:
	Camera* m_camera;
	
	EngineData* m_engine_data;
	
	float m_scroll_sensitivity;
	
	float m_rotation_sensitivity;

	bool m_is_hover_mode_active;

	std::vector<std::string> m_frequenzy_names;

	glm::vec3 m_cached_clear_color;

	const static float min_camera_distance, max_camera_distance;

public:
	glm::vec3 clear_color;

	Signal on_cell_stats_loaded;

	Event<const glm::mat4&> on_view_mat_changed;
	Event<const glm::vec3&> on_clear_color_changed;

	ApplicationModel();

	~ApplicationModel();
	
	void load_cell_stats(const char* path);
	
	void update();
	
	void rotate_camera(glm::vec2 mouse_delta);

	void move_camera_distance(float y_offset);

	void on_vertex_positions_loaded(const char* path);

	void handle_out_of_focus();

	void handle_mouse_dragged(glm::ivec2 mouse_delta);

	void handle_mouse_click();

	void set_is_hover_mode_active(bool value);

	GraphData get_graph_data_for_selected_cells();

	void set_hover_mode_active() { set_is_hover_mode_active(true); }

	void set_cell_selection_mode_active() { set_is_hover_mode_active(false); }

	EngineData* engine_data() { return m_engine_data; }

	void refresh_camera() { on_view_mat_changed.invoke(m_camera->view_mat()); }

	std::vector<std::string> frequenzy_names() { return m_engine_data->frequenzy_names(); }

	Limits* limits_mode() { return (Limits*)(m_engine_data->get_uint(UnsignedIntVariables::NORMAL_MODE_LIMITS)); }

	CellFunctions* selected_function() { return (CellFunctions *)(m_engine_data->get_uint(UnsignedIntVariables::NORMAL_MODE_FUNCTION)); }

	float* user_defined_limits() { return glm::value_ptr(m_engine_data->user_limits); }

	void select_frequency(const std::string& f_name, bool is_selected) { m_engine_data->select_frequency(f_name, is_selected); }

	bool is_frequency_selected(const std::string& f_name) { return m_engine_data->is_frequency_selected(f_name); }

	void clear_frequenzy_selection() { m_engine_data->clear_selection(); }

	std::vector<std::string> selected_frequencies() { return m_engine_data->selected_frequencies(); }

	unsigned int num_of_selected_frequencies() { return m_engine_data->selected_frequencies().size(); }

	std::vector<std::pair<std::string, float>> get_hovered_cell_values() { return m_engine_data->get_hovered_cell_values(); }

	bool is_hover_mode_active() { return m_is_hover_mode_active; }

	bool is_cell_selection_mode_active() { return !m_is_hover_mode_active; }
};