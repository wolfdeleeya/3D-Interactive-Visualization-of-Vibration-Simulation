#pragma once

#include "glm/glm.hpp"
#include "imgui.h"

glm::vec3 convert_hsv_to_rgb(const glm::vec3 hsv);

ImU32 imgui_convert_hsv_to_rgb(const glm::vec3 hsv);