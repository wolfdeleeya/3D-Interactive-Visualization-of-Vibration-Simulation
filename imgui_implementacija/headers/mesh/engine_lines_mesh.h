#pragma once

#include "abstract_mesh.h"
#include "data_loading.h"

class EngineLineMesh : public AbstractMesh {
private:
	static const char* VERTEX_SHADER, * FRAGMENT_SHADER;

	unsigned int m_target_FBO;

	std::vector<glm::vec3> m_model_data;

	std::map<unsigned int, unsigned int> m_indeces_map;

	virtual void setup_buffers() override;

	virtual void setup_indices() override;

	virtual void setup_vertex_data() override;

	void load_model_data();
public:
	EngineLineMesh(const glm::ivec2& window_dimensions, unsigned int target_FBO = 0);

	virtual void render() override;

	void set_target_FBO(unsigned int target_FBO) { m_target_FBO = target_FBO; }
};