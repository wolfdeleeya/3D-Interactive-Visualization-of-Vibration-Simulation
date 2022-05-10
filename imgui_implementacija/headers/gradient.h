#pragma once
#include "glm/glm.hpp"
struct Gradient {
	const static double COLOR_DIFF_TOLERANCE;
	glm::vec3 color1, color2;

	glm::vec3 evaluate(float value);
	
	bool operator==(const Gradient& g) const;

	bool operator!=(const Gradient& g) const { return !(*this == g); }
};