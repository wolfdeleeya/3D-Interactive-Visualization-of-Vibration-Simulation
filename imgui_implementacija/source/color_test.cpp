#include <iostream>

#include "color_helper.h"

int main() {
	glm::vec3 color;
	
	int i = 0;
	
	std::cout << "Enter HSV color:";

	while (std::cin >> color[i++]) {
		if (i == 3) {
			i %= 3;
			glm::vec3 rgb = convert_hsv_to_rgb(color);
			std::cout << "R: " << rgb.r << " G: " << rgb.g << " B: " << rgb.b << std::endl;
			std::cout << "Enter HSV color:";
		}
	}

	return 0;
}