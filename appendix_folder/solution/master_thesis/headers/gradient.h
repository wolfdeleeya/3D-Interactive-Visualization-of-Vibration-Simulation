#pragma once

#include <vector>
#include <functional>

#include "glm/glm.hpp"
#include "imgui.h"

class Gradient {
	enum class InterpolationMode { LINEAR, CUBIC, QUARTIC, CUBIC_SYMMETRICAL, QUARTIC_SYMMETRICAL, END };
	typedef std::function<float(float)> easing_function;
private:
	static const std::vector<easing_function> EASING_FUNCTIONS; 
	static const std::vector<const char*> INTERPOLATION_MODE_NAMES;

	InterpolationMode m_current_interpolation_mode;

	glm::vec3 m_color1, m_color2;

public:
	const static double COLOR_DIFF_TOLERANCE;

	Gradient(const glm::vec3& c1 = glm::vec3(0), const glm::vec3& c2 = glm::vec3(0));

	glm::vec3 evaluate(float value) const;

	ImU32 evaluate_packed(float value) const;
	
	void set_next_interpolation_mode();

	bool operator==(const Gradient& g) const;

	bool operator!=(const Gradient& g) const { return !(*this == g); }

	glm::vec3 color1() const { return m_color1; }

	glm::vec3 color2() const { return m_color2; }

	glm::vec3* color1_ptr() { return &m_color1; }

	glm::vec3* color2_ptr() { return &m_color2; }

	void set_interpolation_mode(InterpolationMode mode) { m_current_interpolation_mode = mode; }

	const char* get_interpolation_mode_name(InterpolationMode mode) const { return INTERPOLATION_MODE_NAMES[(unsigned int)mode]; }

	const char* get_current_interpolation_mode_name() const { return get_interpolation_mode_name(m_current_interpolation_mode); }

	static float identity_operation(float val) { return val; }

	static float cubic_operation(float val) { return powf(val, 3); }

	static float quart_operation(float val) { return powf(val, 4); }

	static float ease_in_out_quart_operation(float val) { return val < 0.5f ? 8 * powf(val, 4) : (1 - powf(-2 * val + 2, 4) / 2); }

	static float ease_in_out_cubic_operation(float val) { return val < 0.5f ? 4 * powf(val, 3) : (1 - powf(-2 * val + 2, 3) / 2); }
};