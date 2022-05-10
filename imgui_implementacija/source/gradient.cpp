#include "gradient.h"
#include "glm_vec_helper.h"

const double Gradient::COLOR_DIFF_TOLERANCE = 0.01;

glm::vec3 Gradient::evaluate(float value)
{
	if (value < 0)
		value = 0;
	if (value > 1)
		value = 1;

	return (1 - value) * color1 + value * color2;
}

bool Gradient::operator==(const Gradient& g) const
{
	bool c1_equal = are_equal(this->color1, g.color1, COLOR_DIFF_TOLERANCE);

	if (!c1_equal)
		return false;

	bool c2_equal = are_equal(this->color2, g.color2, COLOR_DIFF_TOLERANCE);

	if (!c2_equal)
		return false;

	return true;
}