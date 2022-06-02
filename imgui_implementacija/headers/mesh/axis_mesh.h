#pragma once

#include <vector>

#include "abstract_mesh.h"
#include "shader_helper.h"
#include "glm/glm.hpp"

class AxisMesh: public AbstractMesh {
	struct axis_vertex {
		//0 for {0, 0, 0}, 1 for {1, 0, 0}, 2 for {0, 1, 0}, 3 for {0, 0, 1}
		unsigned char point_id;
		//0 for x, 1 for y, 2 for blue
		unsigned char axis_id;
	};

private:
	unsigned int m_target_FBO;

	static const std::vector<axis_vertex> VERTEX_DATA;
	static const char *VERTEX_SHADER, *FRAGMENT_SHADER;

protected:
	virtual void setup_buffers() override;

	virtual void setup_vertex_data() override;

public:
	AxisMesh(const glm::ivec2& window_dimensions, unsigned int target_FBO = 0);

	virtual void render() override;
};