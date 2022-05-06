#pragma once

#include <vector>
#include <string>

#include "glm/glm.hpp"
#include "implot.h"

struct GraphData {
	std::vector<char*> item_labels;
	std::vector<char*> group_labels;
	std::vector<ImS32> plot_data;
	std::vector<double> positions;
	std::vector<glm::vec3> colors;
	
	int items = 0;
	int groups = 0;
	float size;

	GraphData(const std::vector<std::pair<std::string, float>>& data, const glm::vec3& color);

	GraphData(const std::vector<std::string>& group_names, const std::vector<std::pair<std::string, std::vector<float>>>& item_data, const std::vector<glm::vec3>& colors);

	GraphData(const GraphData& gd);

	~GraphData();
	
	void operator=(const GraphData& gd);
};