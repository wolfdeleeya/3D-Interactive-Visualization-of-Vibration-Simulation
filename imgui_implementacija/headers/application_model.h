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
	enum class GraphRenderMode { BARS, LINES, END };
	enum class GraphComparisonMode { DEFAULT, SUBPLOTS, RELATIVE, END };

	enum class ColorVariables { CLEAR_COLOR, END};
private:
	static const std::vector<const char*> GRAPH_RENDER_MODE_LABELS;
	static const std::vector<const char*> GRAPH_COMPARISON_MODE_LABELS;

	Camera* m_camera;

	float m_scroll_sensitivity;
	
	float m_rotation_sensitivity;

	unsigned int m_current_graph_referent_cell_index;

	VariableMap<ColorVariables, glm::vec3> m_color_variables;

	const static float min_camera_distance, max_camera_distance;

	GraphRenderMode m_current_graph_render_mode;
	GraphComparisonMode m_current_graph_comparison_mode;

	void set_graph_render_mode(GraphRenderMode mode) { m_current_graph_render_mode = mode; }

	void set_graph_comparison_mode(GraphComparisonMode mode) { m_current_graph_comparison_mode = mode; }

public:
	glm::vec3 hovered_cell_graph_color;
	float graph_bar_width;
	int graph_num_of_columns;

	Event<const glm::mat4&> on_view_mat_changed;
	Signal on_graph_referent_cell_changed;

	ApplicationModel();

	~ApplicationModel();

	void update();
	
	void rotate_camera(glm::vec2 mouse_delta);

	void move_camera_distance(float y_offset);

	void handle_mouse_dragged(const glm::ivec2& mouse_delta);

	void set_graph_referent_cell(unsigned int new_referent_cell_index);

	void switch_graph_render_mode();

	void switch_graph_comparison_mode();

	void refresh_camera() { on_view_mat_changed.invoke(m_camera->view_mat()); }

	glm::vec3* get_color(ColorVariables e) { return m_color_variables.get(e); }

	void set_color(ColorVariables e, const glm::vec3& c) { m_color_variables.set(e, c); }

	unsigned int current_graph_referant_cell_index() const { return m_current_graph_referent_cell_index; }

	GraphRenderMode current_graph_render_mode() { return m_current_graph_render_mode; }

	GraphComparisonMode current_graph_comparison_mode() { return m_current_graph_comparison_mode; }

	const char* current_graph_render_mode_label() { return GRAPH_RENDER_MODE_LABELS[(unsigned int)m_current_graph_render_mode]; }

	const char* current_graph_comparison_mode_label() { return GRAPH_COMPARISON_MODE_LABELS[(unsigned int)m_current_graph_comparison_mode]; }
};