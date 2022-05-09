#include "mesh_manager.h"
#include "data_loading.h"

#include "debug.h"

#include "mesh/engine_visualization_mesh.h"
#include "mesh/engine_cell_selection_mesh.h"
#include "mesh/engine_lines_mesh.h"

MeshManager::MeshManager(const glm::ivec2& window_dimensions)
{
	glGenFramebuffers(1, &m_scene_view_FBO); 
	glGenFramebuffers(1, &m_scene_view_MS_FBO); 

	glGenTextures(1, &m_scene_view_texture); 
	glGenTextures(1, &m_scene_view_MS_texture); 

	glGenRenderbuffers(1, &m_scene_view_MS_RBO_depth); 

	setup_scene_view_framebuffer(window_dimensions);

	EngineVisualizationMesh* evm = new EngineVisualizationMesh(window_dimensions, m_scene_view_MS_FBO);
	EngineCellSelectionMesh* ecm = new EngineCellSelectionMesh(window_dimensions, { 200, 200 });
	EngineLineMesh* elm = new EngineLineMesh(window_dimensions, m_scene_view_MS_FBO);

	on_cell_selected.add_member_listener(&EngineVisualizationMesh::on_cell_selected, evm);
	on_colors_recalculated.add_member_listener(&EngineVisualizationMesh::set_colors, evm);

	m_index_selection_function = std::bind(&EngineCellSelectionMesh::get_index_at_pos, ecm, std::placeholders::_1, std::placeholders::_2);

	m_meshes.push_back(evm);
	m_meshes.push_back(ecm);
	m_meshes.push_back(elm);
}

MeshManager::~MeshManager()
{
	for (AbstractMesh* mesh : m_meshes)
		delete mesh;

	glDeleteFramebuffers(1, &m_scene_view_FBO);
	glDeleteFramebuffers(1, &m_scene_view_MS_FBO);

	glDeleteTextures(1, &m_scene_view_texture);
	glDeleteTextures(1, &m_scene_view_MS_texture);

	glDeleteRenderbuffers(1, &m_scene_view_MS_RBO_depth);
}

void MeshManager::setup_scene_view_framebuffer(const glm::ivec2& scene_view_dimensions)
{
	unsigned int width = scene_view_dimensions.x;
	unsigned int height = scene_view_dimensions.y;

	glBindFramebuffer(GL_FRAMEBUFFER, m_scene_view_MS_FBO); 

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_scene_view_MS_texture); 
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, true);  
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_scene_view_MS_texture, 0); 
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0); 

	glBindRenderbuffer(GL_RENDERBUFFER, m_scene_view_MS_RBO_depth); 
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT16, width, height); 
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_scene_view_MS_RBO_depth); 
	glBindRenderbuffer(GL_RENDERBUFFER, 0); 

	glBindFramebuffer(GL_FRAMEBUFFER, m_scene_view_FBO); 

	glBindTexture(GL_TEXTURE_2D, m_scene_view_texture); 

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr); 

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_scene_view_texture, 0); 

	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
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

	on_vertices_loaded.invoke();
}

void MeshManager::load_cell_vertices(const char* path)
{
	const auto& cell_vertices = loader::load_cells(path);
	for (AbstractMesh* mesh : m_meshes)
		mesh->set_cell_vertices(cell_vertices);

	on_cell_vertices_loaded.invoke();
}

void MeshManager::window_size_changed(const glm::ivec2& window_dimensions)
{
	m_window_dimensions = window_dimensions;
	
	setup_scene_view_framebuffer(window_dimensions);
	
	for (AbstractMesh* mesh : m_meshes)
		mesh->update_window_size(window_dimensions);
}

unsigned int MeshManager::get_index_at_pos(GLint x, GLint y)
{
	return m_index_selection_function(x, y);
}

void MeshManager::render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_scene_view_MS_FBO); 

	glClearColor(m_current_clear_color.r, m_current_clear_color.g, m_current_clear_color.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 

	for (AbstractMesh* mesh : m_meshes)
		mesh->render(); 

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_scene_view_MS_FBO); 
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_scene_view_FBO); 

	int width = m_window_dimensions.x;
	int height = m_window_dimensions.y;

	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR); 

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); 
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
}
