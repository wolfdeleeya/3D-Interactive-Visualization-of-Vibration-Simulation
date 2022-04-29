#pragma once

#include "engine_data.h"
#include "camera.h"
#include "events.h"
#include "signals.h"
#include "glm/gtc/type_ptr.hpp"

class ApplicationModel {
private:
	Camera* m_camera;
	EngineData* m_engine_data;
	float m_scroll_sensitivity;
	float m_rotation_sensitivity;

	unsigned int m_selected_cell;

	std::vector<std::string> m_frequenzy_names;

	const static float min_camera_distance, max_camera_distance;
public:
	glm::vec3 clear_color;

	Signal on_cell_stats_loaded;

	Event<const glm::mat4&> on_view_mat_changed;
	Event<unsigned int> on_cell_selected;

	ApplicationModel();

	~ApplicationModel();
	
	void load_cell_stats(const char* path);
	
	void update();
	
	void rotate_camera(glm::vec2 mouse_delta);

	void move_camera_distance(float y_offset);

	void on_vertex_positions_loaded(const char* path);

	void select_cell(unsigned int cell_index);

	EngineData* engine_data() { return m_engine_data; }

	void refresh_camera() { on_view_mat_changed.invoke(m_camera->view_mat()); }

	std::vector<std::string> frequenzy_names() { return m_engine_data->frequenzy_names(); }

	Limits* limits_mode() { return &m_engine_data->limits_mode; }

	CellFunctions* selected_function() { return &m_engine_data->selected_function; }

	float* user_defined_limits() { return glm::value_ptr(m_engine_data->user_limits); }

	void select_frequency(const std::string& f_name, bool is_selected) { m_engine_data->select_frequency(f_name, is_selected); }

	bool is_frequency_selected(const std::string& f_name) { return m_engine_data->is_frequency_selected(f_name); }

	void clear_selection() { m_engine_data->clear_selection(); }

	std::vector<std::string> selected_frequencies() { return m_engine_data->selected_frequencies(); }

	bool is_valid_cell_selected() { return m_selected_cell != 0; }

	void clear_selected_cell() { select_cell(0); }

	unsigned int num_of_selected_frequencies() { return m_engine_data->selected_frequencies().size(); }

	std::vector<std::pair<std::string, float>> get_selected_cell_values() { return m_engine_data->get_values_for_cell(m_selected_cell); }
};