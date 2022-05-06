#include "color_helper.h"

glm::vec3 convert_hsv_to_rgb(const glm::vec3 hsv)
{
	float h = 360 * hsv.x, s = hsv.y, v = hsv.z;

	float c = v * s;
	float x = c * (1 - abs(fmod(h / 60.f, 2) - 1));
	float m = v - c;

	int div_res = int(h / 60) % 6;
	
	glm::vec3 res;

	int zero_index = (2 + (div_res / 2)) % 3;
	int c_index = ((div_res + 1) / 2 ) % 3;
	int x_index = (1 - div_res) % 3 + 3;

	res[zero_index] = m;
	res[c_index] = c + m;
	res[x_index] = x + m;

	return res;
}

ImU32 imgui_convert_hsv_to_rgb(const glm::vec3 hsv)
{
	float h = 360 * hsv.x, s = hsv.y, v = hsv.z;

	float c = v * s;
	float x = c * (1 - abs(fmod(h / 60.f, 2) - 1));
	float m = v - c;

	int div_res = int(h / 60) % 6;

	glm::vec3 res;

	int zero_index = (2 + (div_res / 2)) % 3;
	int c_index = ((div_res + 1) / 2) % 3;
	int x_index = (1 - div_res) % 3 + 3;

	res[zero_index] = m;
	res[c_index] = c + m;
	res[x_index] = x + m;

	return ImGui::GetColorU32({res.x, res.y, res.z, 1});
}