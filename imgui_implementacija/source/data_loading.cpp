#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>

#include "data_loading.h"

namespace loader {
std::map<unsigned int, glm::vec3> load_vertices(const char* path) {
	std::map<unsigned int, glm::vec3> result;
	std::ifstream file(path);
	
	std::string line;

	while (std::getline(file, line)) {

		std::stringstream line_stream(line);
		std::string string_part;

		std::getline(line_stream, string_part, ',');
		unsigned int id = std::stoi(string_part);
		glm::vec3 pos;

		for (int i = 0; i < 3; ++i) {
			std::getline(line_stream, string_part, ',');
			pos[i] = std::stof(string_part);
		}

		result.insert({id, pos});
	}

	return result;
}

std::map<unsigned int, std::vector<unsigned int>> load_cells(const char* path) {
	std::map<unsigned int, std::vector<unsigned int>> result;
	std::ifstream file(path);

	std::string line;

	while (std::getline(file, line)) {

		std::stringstream line_stream(line);
		std::string string_part;

		std::getline(line_stream, string_part, ',');
		unsigned int id = std::stoi(string_part);
		
		std::vector<unsigned int> vertex_indeces;

		while(std::getline(line_stream, string_part, ',')) {
			vertex_indeces.push_back(std::stoi(string_part));
		}

		result.insert({ id, vertex_indeces});
	}

	return result;
}

std::vector<unsigned int> triangulate_cell_indeces(const std::map<unsigned int, std::vector<unsigned int>>& cell_indeces,
	const std::map<unsigned int, std::map<unsigned int, unsigned int> > & index_map)
{
	std::vector<unsigned int> result;

	for (const auto& p : cell_indeces) {
		unsigned int cell_index = p.first;
		const std::vector<unsigned int>& vertex_indeces = p.second;
		if (vertex_indeces.size() == 3) {			//already triangulated
			for (int i = 0; i < 3; ++i) {
				unsigned int vertex_index = vertex_indeces[i];
				result.push_back(index_map.at(cell_index).at(vertex_index));
			}
		}
		else if (vertex_indeces.size() == 4) {
			for (int i = 0; i < 3; ++i) {
				unsigned int vertex_index = vertex_indeces[i];
				result.push_back(index_map.at(cell_index).at(vertex_index));
			}

			for (int i = 2; i < 5; ++i) {
				unsigned int vertex_index = vertex_indeces[i % 4];
				result.push_back(index_map.at(cell_index).at(vertex_index));
			}
		}
	}

	return result;
}

std::map<unsigned int, cell_stats> load_cell_stats(const char* path, std::vector<std::string>& frequenzy_names) {
	int valid_line_count = 0;
	std::ifstream file(path);
	std::string line;
	frequenzy_names.clear();
	std::map<unsigned int, cell_stats> result;
	
	while (std::getline(file, line)) {
		if (line[0] == '#')									//is comment?
			continue;
		else if (valid_line_count == 0)						//is header?
			++valid_line_count;
		else if (valid_line_count == 1) {					//is line with category names?
			++valid_line_count;
			std::stringstream line_stream(line);
			std::string string_token;
			int column = 0;
			while (std::getline(line_stream, string_token, ',')) {
				if (column >= 8)
					frequenzy_names.push_back(string_token);
				++column;
			}
		} else {											//is line with data
			cell_stats stats;
			std::stringstream line_stream(line);
			std::string string_token;						
			
			std::getline(line_stream, string_token, ',');	//get id
			unsigned int cell_id = std::stoi(string_token);

			std::getline(line_stream, string_token, ',');	//get selection id
			stats.selection_id = string_token;

			std::getline(line_stream, string_token, ',');	//get area
			stats.area = std::stof(string_token);

			std::getline(line_stream, string_token, ',');	//get area percentage
			stats.area_percent = std::stod(string_token);

			std::getline(line_stream, string_token, ',');	//get area based weight
			stats.area_based_weight = std::stof(string_token);

			for (int i = 0; i < 3; ++i) {					//get mean cell position
				std::getline(line_stream, string_token, ',');
				stats.pos_mean[i] = std::stof(string_token);
			}

			int column_index = 0;							//skip first 8
			
			while (std::getline(line_stream, string_token, ','))	//load remaining frequenzy data
				stats.freq_map.insert({ frequenzy_names[column_index++], std::stof(string_token) });

			result.insert({ cell_id, stats });
		}
	}

	return result;
}
}