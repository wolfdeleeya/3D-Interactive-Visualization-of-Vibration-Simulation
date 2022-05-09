#include <iostream>

#include "application_model.h"
#include <glm_vec_helper.h>

const float ApplicationModel::min_camera_distance = 0.5, ApplicationModel::max_camera_distance = 5;

void ApplicationModel::set_is_hover_mode_active(bool value)
{
	m_is_hover_mode_active = value;
	m_engine_data->update_graph_on_hover(value);
}

void ApplicationModel::set_is_limits_mode_active(bool value)
{
	m_is_limits_mode_active = value;
	m_engine_data->set_is_limits_mode_active(value);
}

ApplicationModel::ApplicationModel():
	m_color_variables([](const glm::vec3& c1, const glm::vec3& c2) { return are_equal(c1, c2); }, ColorVariables::END, { 0,0,0 }, [this]() { this->invoke_clear_color_changed(); })
{
	m_rotation_sensitivity = 1;
	m_scroll_sensitivity = 0.05;

	m_engine_data = new EngineData(glm::vec3(0.55, 0.55, 0.55));
	m_camera = new Camera(max_camera_distance, glm::vec3(0));

	m_is_hover_mode_active = true;
	m_is_limits_mode_active = false;
}

ApplicationModel::~ApplicationModel()
{
	delete m_engine_data;
	delete m_camera;
}

void ApplicationModel::load_cell_stats(const char* path)
{
	m_engine_data->load_cell_stats(path);
	on_cell_stats_loaded.invoke();
}

void ApplicationModel::update()
{
	engine_data()->check_for_changes();

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

void ApplicationModel::on_vertex_positions_loaded(const char* path)				//set camera to point at average position of vertices
{
	const auto& vertices = loader::load_vertices(path);
	
	float min_x, max_x, min_y, max_y, min_z, max_z;		//calculate bounding box

	min_x = min_y = min_z = std::numeric_limits<float>::max();
	max_x = max_y = max_z = std::numeric_limits<float>::min();

	for (const auto& pair : vertices)
	{
		glm::vec3 v = pair.second;

		min_x = v.x < min_x ? v.x : min_x;
		max_x = v.x > max_x ? v.x : max_x;

		min_y = v.y < min_y ? v.y : min_y;
		max_y = v.y > max_y ? v.y : max_y;

		min_z = v.z < min_z ? v.z : min_z;
		max_z = v.z > max_z ? v.z : max_z;
	}

	glm::vec3 box_center((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);

	m_camera->set_target(box_center);

	on_view_mat_changed.invoke(m_camera->view_mat());
}

void ApplicationModel::handle_out_of_focus()
{
	m_engine_data->clear_hovered_cell();
}

void ApplicationModel::handle_mouse_dragged(glm::ivec2 mouse_delta)
{
	rotate_camera(mouse_delta);
	m_engine_data->clear_hovered_cell();
}

void ApplicationModel::handle_mouse_click()
{
	if (is_cell_selection_mode_active())
		engine_data()->handle_cell_selection();
}