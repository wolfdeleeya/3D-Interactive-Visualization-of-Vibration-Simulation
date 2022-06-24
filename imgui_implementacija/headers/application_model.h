#pragma once

#include "engine_model.h"
#include "camera.h"
#include "events.h"
#include "signals.h"
#include "glm/gtc/type_ptr.hpp"
#include "graph/graph_data.h"
#include "variable_map.h"

class ApplicationModel {
public:
	enum class ColorVariables { CLEAR_COLOR, END};
private:
	Camera* m_camera;

	float m_scroll_sensitivity;
	
	float m_rotation_sensitivity;

	VariableMap<ColorVariables, glm::vec3> m_color_variables;

	const static float min_camera_distance, max_camera_distance;

public:
	Event<const glm::mat4&> on_view_mat_changed;

	ApplicationModel();

	~ApplicationModel();

	void update();
	
	void rotate_camera(glm::vec2 mouse_delta);

	void move_camera_distance(float y_offset);

	void on_vertex_positions_loaded(const char* path);

	void handle_mouse_dragged(const glm::ivec2& mouse_delta);

	void refresh_camera() { on_view_mat_changed.invoke(m_camera->view_mat()); }

	glm::vec3* get_color(ColorVariables e) { return m_color_variables.get(e); }

	void set_color(ColorVariables e, const glm::vec3& c) { m_color_variables.set(e, c); }
};