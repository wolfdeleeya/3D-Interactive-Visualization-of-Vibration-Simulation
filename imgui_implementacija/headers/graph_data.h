#pragma once

#include <vector>
#include <string>

struct GraphData {
	std::vector<char*> item_labels;
	std::vector<char*> frequenzy_labels;
	std::vector<float> plot_data;
	std::vector<double> positions;
	
	int items = 0;
	int groups = 0;
	float size;

	GraphData(const std::vector<std::pair<std::string, float>>& data);

	GraphData(const std::vector<std::pair<std::string, std::vector<std::pair<std::string, float>>>>& data);

	GraphData(const GraphData& gd);

	~GraphData();
	
	void operator=(const GraphData& gd);
};