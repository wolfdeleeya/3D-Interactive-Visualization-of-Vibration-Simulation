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

	float m_scroll_sensitivity;
	
	float m_rotation_sensitivity;

	bool m_is_limits_mode_active;

	VariableMap<ColorVariables, glm::vec3> m_color_variables;

	const static float min_camera_distance, max_camera_distance;

public:
	Event<const glm::mat4&> on_view_mat_changed;
	Event<const glm::vec3&> on_clear_color_changed;
	Event<bool> on_limits_mode_toggled;

	ApplicationModel();

	~ApplicationModel();

	void update();
	
	void rotate_camera(glm::vec2 mouse_delta);

	void move_camera_distance(float y_offset);

	void on_vertex_positions_loaded(const char* path);

	void handle_mouse_dragged(const glm::ivec2& mouse_delta);

	void set_is_limits_mode_active(bool value);

	void refresh_camera() { on_view_mat_changed.invoke(m_camera->view_mat()); }

	bool is_limits_mode_active() { return m_is_limits_mode_active; }

	glm::vec3* get_color(ColorVariables e) { return m_color_variables.get(e); }

	void set_color(ColorVariables e, const glm::vec3& c) { m_color_variables.set(e, c); }

	void invoke_clear_color_changed() { on_clear_color_changed.invoke(*get_color(ColorVariables::CLEAR_COLOR)); }
};