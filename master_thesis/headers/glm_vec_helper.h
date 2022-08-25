#pragma once

#include "glm/glm.hpp"

template<size_t L, typename T>
bool are_equal(const glm::vec<L, T>& v1, const glm::vec<L, T>& v2, const double tolerance = 0.01) {
	for (int i = 0; i < L; ++i)
		if (abs(v1[i] - v2[i]) > tolerance)
			return false;

	return true;
}