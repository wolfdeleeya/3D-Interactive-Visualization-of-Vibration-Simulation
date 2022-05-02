#include "mesh_manager.h"
#include "data_loading.h"
#include "mesh/engine_visualization_mesh.h"
#include "mesh/engine_cell_selection_mesh.h"

MeshManager::MeshManager(const glm::ivec2& window_dimensions)
{
	EngineVisualizationMesh* evm = new EngineVisualizationMesh(window_dimensions);
	EngineCellSelectionMesh* ecm = new EngineCellSelectionMesh(window_dimensions, { 100, 100 });

	on_cell_selected.add_member_listener(&EngineCellSelectionMesh::on_cell_selected, ecm);
	on_colors_recalculated.add_member_listener(&EngineVisualizationMesh::set_colors, evm);

	m_meshes.push_back(evm);
	m_meshes.push_back(ecm);
}

MeshManager::~MeshManager()
{
	for (AbstractMesh* mesh : m_meshes)
		delete mesh;
}

void MeshManager::view_mat_changed(const glm::mat4& view)
{
	for (AbstractMesh* mesh : m_meshes)
		mesh->set_view(view);
}

void MeshManager::load_vertex_positions(const char* path)
{
	const auto& vertex_positions = loader::load_vertices(path);
	for (AbstractMesh* mesh : m_meshes)
		mesh->set_vertex_positions(vertex_positions);
}

void MeshManager::load_cell_vertices(const char* path)
{
	const auto& cell_vertices = loader::load_cells(path);
	for (AbstractMesh* mesh : m_meshes)
		mesh->set_cell_vertices(cell_vertices);
}

void MeshManager::window_size_changed(const glm::ivec2& window_dimensions)
{
	for (AbstractMesh* mesh : m_meshes)
		mesh->update_window_size(window_dimensions);
}