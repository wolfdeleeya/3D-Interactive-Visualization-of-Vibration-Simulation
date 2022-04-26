#pragma once

#include <vector>

#include "application_model.h"
#include "GLFW/glfw3.h"
#include "nfd.h"

class ImGUILayer {
private:
	GLFWwindow* m_window;
	std::vector<std::string> m_frequenzy_names;
	ApplicationModel* m_application_model;

public:
	Event<const char*> on_load_vertex_positions,
		on_load_cell_vertices,
		on_load_cell_stats;


	ImGUILayer(ApplicationModel* application_model, GLFWwindow* window, const char* version_string, bool is_dark = true);
	
	void update();

	bool handle_mouse_scroll(double x_offset, double y_offset);
	
	bool handle_mouse_pos(double x_pos, double y_pos);
	
	bool handle_mouse_click(int button, bool down);

	void cell_stats_loaded();
};

std::string get_file_path(std::initializer_list<nfdfilteritem_t> filter_items);

std::string fix_path(const std::string path);