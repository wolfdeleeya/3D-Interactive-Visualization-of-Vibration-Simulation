#pragma once

#include "engine_data.h"
#include "camera.h"
#include "events.h"
#include "signals.h"
#include "glm/gtc/type_ptr.hpp"
#include "graph_data.h"
#include "variable_map.h"

class ApplicationModel {
public:
	enum class ColorVariables { CLEAR_COLOR, END};
private:
	Camera* m_camera;
	
	EngineData* m_engine_data;
	
	float m_scroll_sensitivity;
	
	float m_rotation_sensitivity;

	bool m_is_hover_mode_active;

	bool m_is_limits_mode_active;

	std::vector<std::string> m_frequenzy_names;

	VariableMap<ColorVariables, glm::vec3> m_color_variables;

	const static float min_camera_distance, max_camera_distance;

public:
	Signal on_cell_stats_loaded, 
		on_frequency_limits_loaded;

	Event<const glm::mat4&> on_view_mat_changed;
	Event<const glm::vec3&> on_clear_color_changed;

	Event<unsigned int> on_cell_hovered;
	Event<const std::map<unsigned int, glm::vec3>&> on_colors_recalculated;

	Event<const GraphData&> on_graph_data_changed;

	ApplicationModel();

	~ApplicationModel();
	
	void load_cell_stats(const char* path);

	void load_frequency_limits(const char* path);

	void update();
	
	void rotate_camera(glm::vec2 mouse_delta);

	void move_camera_distance(float y_offset);

	void on_vertex_positions_loaded(const char* path);

	void handle_scene_view_out_of_focus();

	void handle_mouse_dragged(glm::ivec2 mouse_delta);

	void handle_mouse_click();

	void set_is_hover_mode_active(bool value);

	void set_is_limits_mode_active(bool value);

	void load_cell_vertices(const char* path) { m_engine_data->load_cell_vertices(path); }

	void set_hover_mode_active() { set_is_hover_mode_active(true); }

	void set_cell_selection_mode_active() { set_is_hover_mode_active(false); }

	void refresh_camera() { on_view_mat_changed.invoke(m_camera->view_mat()); }

	std::vector<std::string> frequenzy_names() { return m_engine_data->frequenzy_names(); }

	void select_frequency(const std::string& f_name, bool is_selected) { m_engine_data->select_frequency(f_name, is_selected); }

	bool is_frequency_selected(const std::string& f_name) { return m_engine_data->is_frequency_selected(f_name); }

	void clear_frequenzy_selection() { m_engine_data->clear_frequency_selection(); }

	void clear_selected_cells() { m_engine_data->clear_selected_cells(); }

	void set_hovered_cell(unsigned int cell_index) { m_engine_data->set_hovered_cell(cell_index); }

	std::vector<std::string> selected_frequencies() { return m_engine_data->selected_frequencies(); }

	std::vector<std::string> frequencies_with_limits() { return m_engine_data->frequencies_with_limits(); }

	unsigned int num_of_selected_frequencies() { return m_engine_data->selected_frequencies().size(); }

	std::vector<std::pair<std::string, float>> get_hovered_cell_values() { return m_engine_data->get_hovered_cell_values(); }

	bool is_hover_mode_active() { return m_is_hover_mode_active; }

	bool is_cell_selection_mode_active() { return !m_is_hover_mode_active; }

	bool is_limits_mode_active() { return m_is_limits_mode_active; }

	bool are_stats_loaded() { return m_engine_data->are_stats_loaded(); }

	bool are_frequency_limits_loaded() { return m_engine_data->are_frequenzy_limits_loaded(); }

	glm::vec3* get_color(ColorVariables e) { return m_color_variables.get(e); }

	void set_color(ColorVariables e, const glm::vec3& c) { m_color_variables.set(e, c); }

	glm::vec3* get_engine_data_color(EngineData::ColorVariables e) { return m_engine_data->get_color(e); }

	Gradient* get_engine_data_gradient(EngineData::GradientVariables e) { return m_engine_data->get_gradient(e); }

	unsigned int* get_engine_data_uint(EngineData::UnsignedIntVariables e) { return m_engine_data->get_uint(e); }

	glm::vec2* get_engine_data_normal_mode_limits(EngineData::NormalModeLimitsVariables e) 
	{
		return m_engine_data->get_normal_mode_limits(e);
	}

	void set_engine_data_uint(EngineData::UnsignedIntVariables e, unsigned int value) { return m_engine_data->set_uint(e, value); }

	void refresh_engine_cell_colors() { m_engine_data->refresh_color(); }

	void invoke_clear_color_changed() { on_clear_color_changed.invoke(*get_color(ColorVariables::CLEAR_COLOR)); }

	void invoke_colors_recalculated(const std::map<unsigned int, glm::vec3>& color_map) { on_colors_recalculated.invoke(color_map); }

	void invoke_cell_hovered(unsigned int cell_index) { on_cell_hovered.invoke(cell_index); }

	void invoke_graph_data_changed(const GraphData& gd) { on_graph_data_changed.invoke(gd); }
};