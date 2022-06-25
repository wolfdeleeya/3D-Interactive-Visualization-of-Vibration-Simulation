#include "mesh_manager.h"
#include "data_loading.h"

#include "debug.h"

#include "mesh/engine_visualization_mesh.h"
#include "mesh/engine_cell_selection_mesh.h"
#include "mesh/engine_lines_mesh.h"

MeshManager::MeshManager(ApplicationModel* application_model, EngineModel* engine_model, const glm::ivec2& window_dimensions)
{
	m_application_model = application_model;
	m_engine_model = engine_model;

	glGenFramebuffers(1, &m_scene_view_FBO); 
	glGenFramebuffers(1, &m_scene_view_MS_FBO); 

	glGenTextures(1, &m_scene_view_texture); 
	glGenTextures(1, &m_scene_view_MS_texture); 

	glGenRenderbuffers(1, &m_scene_view_MS_RBO_depth); 

	setup_scene_view_framebuffer(window_dimensions);

	EngineVisualizationMesh* evm = new EngineVisualizationMesh(m_engine_model, window_dimensions, m_scene_view_MS_FBO);
	EngineCellSelectionMesh* ecm = new EngineCellSelectionMesh(m_engine_model, window_dimensions, { 200, 200 });
	EngineLineMesh* elm = new EngineLineMesh(m_engine_model, window_dimensions, m_scene_view_MS_FBO);

	AxisMesh* axis_mesh = new AxisMesh(window_dimensions, m_scene_view_MS_FBO);

	on_cell_hovered.add_member_listener(&EngineVisualizationMesh::cell_hovered, evm);

	m_index_selection_function = std::bind(&EngineCellSelectionMesh::get_index_at_pos, ecm, std::placeholders::_1, std::placeholders::_2);

	m_meshes.push_back(evm);
	m_meshes.push_back(ecm);
	m_meshes.push_back(elm);
	m_meshes.push_back(axis_mesh);
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
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB, width, height, true);  
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_scene_view_MS_texture, 0); 
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0); 

	glBindRenderbuffer(GL_RENDERBUFFER, m_scene_view_MS_RBO_depth); 
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH_COMPONENT16, width, height); 
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

void MeshManager::window_size_changed(const glm::ivec2& window_dimensions)
{
	m_window_dimensions = window_dimensions;
	
	setup_scene_view_framebuffer(window_dimensions);
	
	for (AbstractMesh* mesh : m_meshes)
		mesh->update_window_size(window_dimensions);
}

void MeshManager::render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_scene_view_MS_FBO); 

	glm::vec3 clear_color = *m_application_model->get_color(ApplicationModel::ColorVariables::CLEAR_COLOR);

	glClearColor(clear_color.r, clear_color.g, clear_color.b, 1);
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
