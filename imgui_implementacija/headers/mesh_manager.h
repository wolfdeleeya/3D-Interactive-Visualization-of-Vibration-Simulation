#pragma once

#include "events.h"
#include "signals.h"
#include "mesh/abstract_mesh.h"
#include "mesh/abstract_engine_mesh.h"

#include "mesh/axis_mesh.h"

class MeshManager {
private:
	std::vector<AbstractEngineMesh*> m_engine_meshes;

	AxisMesh* m_axis_mesh;

	Event<unsigned int> on_cell_hovered;

	Event<const std::map<unsigned int, glm::vec3>&> on_colors_recalculated;

	std::function<unsigned int(GLint, GLint)> m_index_selection_function;

	unsigned int m_scene_view_FBO, m_scene_view_MS_FBO;
	unsigned int m_scene_view_texture, m_scene_view_MS_texture;
	unsigned int m_scene_view_MS_RBO_depth;

	glm::ivec2 m_window_dimensions;
	glm::vec3 m_current_clear_color;

public:
	Signal on_vertices_loaded,
		on_cell_vertices_loaded;

	MeshManager(const glm::ivec2& window_dimensions);

	~MeshManager();

	void setup_scene_view_framebuffer(const glm::ivec2& scene_view_dimensions);

	void view_mat_changed(const glm::mat4& view);

	void colors_recalculated(const std::map<unsigned int, glm::vec3>& colors) { on_colors_recalculated.invoke(colors); }

	void cell_hovered(unsigned int cell_index) { on_cell_hovered.invoke(cell_index); }

	void load_vertex_positions(const char* path);

	void load_cell_vertices(const char* path);

	void window_size_changed(const glm::ivec2& window_dimensions);

	unsigned int get_index_at_pos(GLint x, GLint y);

	void render();

	void set_current_clear_color(const glm::vec3& clear_color) { m_current_clear_color = clear_color; }

	unsigned int scene_texture() { return m_scene_view_texture; }
};