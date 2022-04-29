#pragma once

#include <string>
#include <map>
#include <vector>
#include "glm/glm.hpp"

struct cell_stats {
	std::string selection_id;
	float area;
	double area_percent;
	float area_based_weight;
	glm::vec3 pos_mean;
	std::map<std::string, float> freq_map;
};

namespace loader {
	std::map<unsigned int, glm::vec3> load_vertices(const char* path);

	std::map<unsigned int, std::vector<unsigned int>> load_cells(const char* path);

	std::vector<unsigned int> triangulate_cell_indeces(const std::map<unsigned int, std::vector<unsigned int>>& cell_indeces,
		const std::map<unsigned int, std::map<unsigned int, unsigned int>>& index_map);

	std::map<unsigned int, cell_stats> load_cell_stats(const char* path, std::vector<std::string>& frequenzy_names);
}