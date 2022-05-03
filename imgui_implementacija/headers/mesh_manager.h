#pragma once

#include "events.h"
#include "mesh/abstract_mesh.h"

class MeshManager {
private:
	std::vector<AbstractMesh*> m_meshes;

	Event<unsigned int> on_cell_selected;

	Event<const std::map<unsigned int, glm::vec3>&> on_colors_recalculated;

	std::function<unsigned int(GLint, GLint)> m_index_selection_function;

	unsigned int m_scene_view_FBO;
	unsigned int m_scene_view_texture, m_scene_view_RBO_depth;

public:
	MeshManager(const glm::ivec2& window_dimensions);

	~MeshManager();

	void setup_scene_view_framebuffer(const glm::ivec2& scene_view_dimensions);

	void view_mat_changed(const glm::mat4& view);

	void colors_recalculated(const std::map<unsigned int, glm::vec3>& colors) { on_colors_recalculated.invoke(colors); }

	void cell_selected(unsigned int cell_index) { on_cell_selected.invoke(cell_index); }

	void load_vertex_positions(const char* path);

	void load_cell_vertices(const char* path);

	void window_size_changed(const glm::ivec2& window_dimensions);

	unsigned int get_index_at_pos(GLint x, GLint y);

	void render();

	unsigned int scene_texture() { return m_scene_view_texture; }
};