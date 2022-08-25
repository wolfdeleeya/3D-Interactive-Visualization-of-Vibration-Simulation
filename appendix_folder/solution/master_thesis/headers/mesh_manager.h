#pragma once

#include "application_model.h"
#include "engine_model.h"

#include "mesh/abstract_mesh.h"
#include "mesh/abstract_engine_mesh.h"
#include "mesh/axis_mesh.h"
#include "mesh/engine_cell_selection_mesh.h"

#include "events.h"
#include "signals.h"

class MeshManager {
private:
	enum class MeshType {ENGINE_VISUALIZATION, ENGINE_CELL_SELECTION, ENGINE_LINE_MESH, AXIS_MESH};

	ApplicationModel* m_application_model;
	EngineModel* m_engine_model;

	std::vector<AbstractMesh*> m_meshes;

	Event<unsigned int> on_cell_hovered;

	std::function<unsigned int(GLint, GLint)> m_index_selection_function;

	unsigned int m_scene_view_FBO, m_scene_view_MS_FBO;
	unsigned int m_scene_view_texture, m_scene_view_MS_texture;
	unsigned int m_scene_view_MS_RBO_depth;

	glm::ivec2 m_window_dimensions;
public:
	Event<const glm::vec3&> on_engine_center_calculated;

	MeshManager(ApplicationModel* application_model, EngineModel* engine_model, const glm::ivec2& window_dimensions);

	~MeshManager();

	void setup_scene_view_framebuffer(const glm::ivec2& scene_view_dimensions);

	void view_mat_changed(const glm::mat4& view);

	void cell_hovered(unsigned int cell_index) { on_cell_hovered.invoke(cell_index); }

	void window_size_changed(const glm::ivec2& window_dimensions);

	unsigned int get_index_at_pos(GLint x, GLint y) { return m_index_selection_function(x, y); }

	void render();

	unsigned int scene_texture() { return m_scene_view_texture; }
};