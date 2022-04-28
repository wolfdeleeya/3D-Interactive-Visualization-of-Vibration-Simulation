#include <iostream>

#include "application_model.h"

const float ApplicationModel::min_camera_distance = 0.5, ApplicationModel::max_camera_distance = 5;

ApplicationModel::ApplicationModel()
{
	m_rotation_sensitivity = 1;
	m_scroll_sensitivity = 0.05;

	m_engine_data = new EngineData(glm::vec3(0.55, 0.55, 0.55));
	m_camera = new Camera(max_camera_distance, glm::vec3(0));
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
	m_engine_data->check_for_changes();
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

	glm::vec3 sum(0);
	
	glm::vec3 leftest, rightest, frontest, backest, highest, lowest;

	for (const auto& pair : vertices)
		sum += pair.second;

	glm::vec3 avg = sum / (float)vertices.size();
	m_camera->set_target(avg);

	std::cout << avg.x << ", " << avg.y << ", " << avg.z << std::endl;

	on_view_mat_changed.invoke(m_camera->view_mat());
}
