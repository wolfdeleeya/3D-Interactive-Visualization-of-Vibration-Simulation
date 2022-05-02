#pragma once

#include "events.h"
#include "mesh/abstract_mesh.h"

class MeshManager {
private:
	std::vector<AbstractMesh*> m_meshes;

	Event<unsigned int> on_cell_selected;

	Event<const std::map<unsigned int, glm::vec3>&> on_colors_recalculated;

public:
	MeshManager(const glm::ivec2& window_dimensions);

	~MeshManager();

	void view_mat_changed(const glm::mat4& view);

	void colors_recalculated(const std::map<unsigned int, glm::vec3>& colors) { on_colors_recalculated.invoke(colors); }

	void cell_selected(unsigned int cell_index) { on_cell_selected.invoke(cell_index); }

	void load_vertex_positions(const char* path);

	void load_cell_vertices(const char* path);

	void window_size_changed(const glm::ivec2& window_dimensions);
};