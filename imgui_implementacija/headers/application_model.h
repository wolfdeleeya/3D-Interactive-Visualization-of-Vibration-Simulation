#pragma once
#include "model.h"
#include "camera.h"
#include "events.h"
#include "signals.h"

class ApplicationModel {
private:
	Model* m_model;
	Camera* m_camera;
	float m_scroll_sensitivity;
	float m_rotation_sensitivity;

	std::vector<std::string> m_frequenzy_names;

	const static float min_camera_distance, max_camera_distance;
public:
	std::vector<bool> selected_attributes;

	Signal on_vertices_loaded,
		on_cells_loaded,
		on_cell_stats_loaded;

	Event<glm::mat4> on_view_mat_changed;
	
	ApplicationModel(const char* vert_shader_path, const char* frag_shader_path, int init_width, int init_height);

	~ApplicationModel();
	
	void load_vertices(const char* path);
	
	void load_cells(const char* path);
	
	void load_cell_stats(const char* path);
	
	void update();

	Model* model() { return m_model; }
	
	void rotate_camera(glm::vec2 mouse_delta);

	void move_camera_distance(float y_offset);
	
	void set_window_size(int width, int height);

	std::vector<std::string> frequenzy_names() { return m_model->frequenzy_names(); }
};