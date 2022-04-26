#pragma once

#include "engine_data.h"
#include "camera.h"
#include "events.h"
#include "signals.h"

class ApplicationModel {
private:
	Camera* m_camera;
	EngineData* m_engine_data;
	float m_scroll_sensitivity;
	float m_rotation_sensitivity;

	std::vector<std::string> m_frequenzy_names;

	const static float min_camera_distance, max_camera_distance;
public:
	std::vector<bool> selected_attributes;

	Signal on_cell_stats_loaded;

	Event<const glm::mat4&> on_view_mat_changed;
	
	ApplicationModel();

	~ApplicationModel();
	
	void load_cell_stats(const char* path);
	
	void update();

	EngineData* engine_data() { return m_engine_data; }
	
	void rotate_camera(glm::vec2 mouse_delta);

	void move_camera_distance(float y_offset);

	void refresh_camera() { on_view_mat_changed.invoke(m_camera->view_mat()); }

	std::vector<std::string> frequenzy_names() { return m_engine_data->frequenzy_names(); }
};