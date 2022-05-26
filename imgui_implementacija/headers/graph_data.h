#pragma once

#include <vector>
#include <string>

#include "glm/glm.hpp"
#include "implot.h"

struct GraphData {
	std::vector<char*> item_labels;
	std::vector<char*> group_labels;
	std::vector<double> plot_data;
	std::vector<double> positions;
	std::vector<glm::vec3> colors;
	
	float size;

	GraphData(const std::vector<std::string>& group_names, const std::vector<std::pair<std::string, std::vector<float>>>& item_data, const std::vector<glm::vec3>& colors, float size = 0.5f);

	GraphData(const GraphData& gd);

	~GraphData();
	
	void operator=(const GraphData& gd);
};