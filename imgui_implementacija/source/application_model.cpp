#include <iostream>

#include "application_model.h"
#include <glm_vec_helper.h>

const std::vector<const char*> ApplicationModel::GRAPH_RENDER_MODE_LABELS = {
	"BARS",
	"LINES"
};

const std::vector<const char*> ApplicationModel::GRAPH_COMPARISON_MODE_LABELS = {
	"DEFAULT",
	"SUBPLOTS",
	"RELATIVE"
};

const float ApplicationModel::min_camera_distance = 0.5, ApplicationModel::max_camera_distance = 5;

ApplicationModel::ApplicationModel():
	m_color_variables([](const glm::vec3& c1, const glm::vec3& c2) { return are_equal(c1, c2); }, ColorVariables::END, { 0,0,0 })
{
	m_rotation_sensitivity = 1;
	m_scroll_sensitivity = 0.05;

	set_graph_render_mode(GraphRenderMode::BARS);
	set_graph_comparison_mode(GraphComparisonMode::DEFAULT);

	hovered_cell_graph_color = { 0.835, 0.662, 0.427 };
	graph_bar_width = 0.5;
	graph_num_of_columns = 1;
	m_current_graph_referent_cell_index = 0;

	m_camera = new Camera(max_camera_distance, glm::vec3(0));
}

ApplicationModel::~ApplicationModel()
{
	delete m_camera;
}

void ApplicationModel::update()
{
	bool are_changes_pending = false;
	
	are_changes_pending |= m_color_variables.check_for_changes();
}

void ApplicationModel::rotate_camera(glm::vec2 mouse_delta)
{
	m_camera->rotate(-glm::vec2(mouse_delta.y, mouse_delta.x) * m_rotation_sensitivity);

	on_view_mat_changed.invoke(m_camera->view_mat());
}

void ApplicationModel::move_camera_distance(float y_offset)
{
	float distance_offset = -m_camera->distance() * m_scroll_sensitivity * y_offset;		//negative due to the nature of scrolling, I'm using camera distance so that scrolling speed is linearly decreased
	float distance = m_camera->distance() + distance_offset;
	
	if (distance > max_camera_distance)
		distance = max_camera_distance;
	else if (distance < min_camera_distance)
		distance = min_camera_distance;

	m_camera->set_distance(distance);

	on_view_mat_changed.invoke(m_camera->view_mat());
}

void ApplicationModel::handle_mouse_dragged(const glm::ivec2& mouse_delta)
{
	rotate_camera(mouse_delta);
}

void ApplicationModel::set_graph_referent_cell(unsigned int new_referent_cell_index)
{
	m_current_graph_referent_cell_index = new_referent_cell_index;
	on_graph_referent_cell_changed.invoke();
}

void ApplicationModel::switch_graph_render_mode()
{
	unsigned int next_mode = ((unsigned int)m_current_graph_render_mode + 1) % (unsigned int)GraphRenderMode::END;
	set_graph_render_mode((GraphRenderMode)next_mode);
}

void ApplicationModel::switch_graph_comparison_mode()
{
	unsigned int next_mode = ((unsigned int)m_current_graph_comparison_mode + 1) % (unsigned int)GraphComparisonMode::END;
	set_graph_comparison_mode((GraphComparisonMode)next_mode);
}