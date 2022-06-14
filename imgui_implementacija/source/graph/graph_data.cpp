#include "graph/graph_data.h"
#include "color_helper.h"
#include <iostream>

GraphData::GraphData(const std::vector<std::string>& group_names, const std::vector<std::pair<std::string, std::vector<float>>>& item_data, const std::vector<glm::vec3>& colors) {
	this->colors = colors;
	std::cout << group_names.size();
	unsigned int num_of_items = item_data.size();
	unsigned int num_of_groups = group_names.size();

	item_labels.resize(num_of_items);

	positions.resize(num_of_groups);
	group_labels.resize(num_of_groups);

	for (int i = 0; i < num_of_groups; ++i) {
		positions[i] = i;

		const std::string group_label = group_names[i];

		group_labels[i] = new char[group_label.size() + 1];
		std::copy(group_label.begin(), group_label.end(), group_labels[i]);
		group_labels[i][group_label.size()] = '\0';
	}

	for (int i = 0; i < num_of_items; ++i) {
		const std::string item_label = item_data[i].first;

		item_labels[i] = new char[item_label.size() + 1];
		std::copy(item_label.begin(), item_label.end(), item_labels[i]);
		item_labels[i][item_label.size()] = '\0';


		const std::vector<float>& current_item_data = item_data[i].second;

		for (int j = 0; j < num_of_groups; ++j)
			plot_data.push_back(current_item_data[j]);
	}
}

GraphData::GraphData(const GraphData& gd)
{
	unsigned int num_of_items = gd.item_labels.size();
	unsigned int num_of_groups = gd.group_labels.size();

	plot_data = gd.plot_data;
	positions = gd.positions;

	colors = gd.colors;

	item_labels.resize(num_of_items);
	group_labels.resize(num_of_groups);

	for (int i = 0; i < num_of_groups; ++i) {
		positions[i] = i;

		const std::string group_label = gd.group_labels[i];

		group_labels[i] = new char[group_label.size() + 1];
		std::copy(group_label.begin(), group_label.end(), group_labels[i]);
		group_labels[i][group_label.size()] = '\0';
	}

	for (int i = 0; i < num_of_items; ++i) {
		const std::string item_label = gd.item_labels[i];

		item_labels[i] = new char[item_label.size() + 1];
		std::copy(item_label.begin(), item_label.end(), item_labels[i]);
		item_labels[i][item_label.size()] = '\0';
	}
}

GraphData::~GraphData()
{
	for (char* label : group_labels)
		delete[] label;

	for (char* label : item_labels)
		delete[] label;
}

void GraphData::operator=(const GraphData& gd)
{
	for (char* label : group_labels)
		delete[] label;

	for (char* label : item_labels)
		delete[] label;

	group_labels.clear();
	item_labels.clear();
	plot_data.clear();
	positions.clear();

	unsigned int num_of_items = gd.item_labels.size();
	unsigned int num_of_groups = gd.group_labels.size();

	plot_data = gd.plot_data;
	positions = gd.positions;

	colors = gd.colors;

	item_labels.resize(num_of_items);
	group_labels.resize(num_of_groups);

	for (int i = 0; i < num_of_groups; ++i) {
		positions[i] = i;

		const std::string group_label = gd.group_labels[i];

		group_labels[i] = new char[group_label.size() + 1];
		std::copy(group_label.begin(), group_label.end(), group_labels[i]);
		group_labels[i][group_label.size()] = '\0';
	}

	for (int i = 0; i < num_of_items; ++i) {
		const std::string item_label = gd.item_labels[i];

		item_labels[i] = new char[item_label.size() + 1];
		std::copy(item_label.begin(), item_label.end(), item_labels[i]);
		item_labels[i][item_label.size()] = '\0';
	}
}