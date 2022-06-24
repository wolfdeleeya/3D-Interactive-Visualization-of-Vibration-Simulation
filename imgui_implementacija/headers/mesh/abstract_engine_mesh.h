#pragma once

#include "engine_model.h"
#include "abstract_mesh.h"

class AbstractEngineMesh : public AbstractMesh {
protected:
	EngineModel* m_engine_model;

	unsigned int m_EBO;

	std::vector<unsigned int> m_indeces;

	virtual void setup_indices() = 0;

public:
	AbstractEngineMesh(const char* vertex_shader_dest, const char* fragment_shader_dest, EngineModel* engine_model, const glm::ivec2& window_dimensions);

	virtual ~AbstractEngineMesh();

	void setup_cell_vertices();

	void setup_vertex_positions();
};