#include "gradient.h"
#include "glm_vec_helper.h"

const std::vector<Gradient::easing_function> Gradient::EASING_FUNCTIONS = {
	&Gradient::identity_operation,
	&Gradient::cubic_operation,
	&Gradient::quart_operation,
	&Gradient::ease_in_out_cubic_operation,
	&Gradient::ease_in_out_quart_operation,
};

const std::vector<const char*> Gradient::INTERPOLATION_MODE_NAMES = {
	"Linear",
	"Cubic",
	"Quartic",
	"Cubic Symmetrical",
	"Quartic Symmetrical"
};

const double Gradient::COLOR_DIFF_TOLERANCE = 0.01;

Gradient::Gradient(const glm::vec3& c1, const glm::vec3& c2) :m_color1(c1), m_color2(c2) 
{
	set_interpolation_mode(InterpolationMode::LINEAR);
}

glm::vec3 Gradient::evaluate(float value) const
{
	if (value < 0)
		value = 0;
	if (value > 1)
		value = 1;

	easing_function current_easing_function = EASING_FUNCTIONS[unsigned int(m_current_interpolation_mode)];
	value = current_easing_function(value);

	return m_color1 + (m_color2 - m_color1) * value;
}

ImU32 Gradient::evaluate_packed(float value) const
{
	glm::vec3 color = evaluate(value);
	return ImGui::GetColorU32({ color.r, color.g, color.b, 1 });
}

void Gradient::set_next_interpolation_mode()
{
	unsigned int current_interpolation_mode = unsigned int(m_current_interpolation_mode);
	unsigned int next_interpolation_mode = (current_interpolation_mode + 1) % unsigned int(InterpolationMode::END);

	set_interpolation_mode(InterpolationMode(next_interpolation_mode));
}

bool Gradient::operator==(const Gradient& g) const
{
	if (this->m_current_interpolation_mode != g.m_current_interpolation_mode)
		return false;

	bool c1_equal = are_equal(this->m_color1, g.m_color1, COLOR_DIFF_TOLERANCE);

	if (!c1_equal)
		return false;

	bool c2_equal = are_equal(this->m_color2, g.m_color2, COLOR_DIFF_TOLERANCE);

	if (!c2_equal)
		return false;

	return true;
}