#include "application_model.h"

const float ApplicationModel::min_camera_distance = 1, ApplicationModel::max_camera_distance = 30;

ApplicationModel::ApplicationModel(const char* vert_shader_path, const char* frag_shader_path, int init_width, int init_height)
{
	m_rotation_sensitivity = 1;
	m_scroll_sensitivity = 0.05;

	m_model = new Model(vert_shader_path, frag_shader_path, false, glm::vec3(0.55, 0.55, 0.55));
	m_camera = new Camera(10, glm::vec3(0));
	
	m_model->set_view(m_camera->view_mat());
	set_window_size(init_width, init_height);
}

ApplicationModel::~ApplicationModel()
{
	delete m_model;
	delete m_camera;
}

void ApplicationModel::load_vertices(const char* path)
{
	m_model->load_vertex_positions(path);
	on_vertices_loaded.invoke();
}

void ApplicationModel::load_cells(const char* path)
{
	m_model->load_cell_vertices(path);
	on_cells_loaded.invoke();
}

void ApplicationModel::load_cell_stats(const char* path)
{
	m_model->load_cell_stats(path);
	selected_attributes.clear();
	selected_attributes.resize(frequenzy_names().size());
	on_cell_stats_loaded.invoke();
}

void ApplicationModel::rotate_camera(glm::vec2 mouse_delta)
{
	m_camera->rotate(-glm::vec2(mouse_delta.y, mouse_delta.x) * m_rotation_sensitivity);
	m_model->set_view(m_camera->view_mat());

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
	m_model->set_view(m_camera->view_mat());

	on_view_mat_changed.invoke(m_camera->view_mat());
}

void ApplicationModel::set_window_size(int width, int height)
{
	m_model->set_projection(glm::perspective(glm::radians(45.f), (float)width / height, 0.1f, 100.f));
}
