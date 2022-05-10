#include "graph_data.h"
#include "color_helper.h"

GraphData::GraphData(const std::vector<std::pair<std::string, float>>& data, const glm::vec3& color)
{
	items = 1;

	char* dummy = new char[1];
	dummy[0] = '\0';

	item_labels.push_back(dummy);

	groups = data.size();

	colors.push_back(color);

	for (int i = 0; i < groups; ++i) {
		const std::string& name = data[i].first;

		group_labels.push_back(new char[name.size() + 1]);
		std::copy(name.begin(), name.end(), group_labels[i]);
		group_labels[i][name.size()] = '\0';

		plot_data.push_back(data[i].second);
		positions.push_back(i);
	}

	size = 0.5;
}

GraphData::GraphData(const std::vector<std::string>& group_names, const std::vector<std::pair<std::string, std::vector<float>>>& item_data, const std::vector<glm::vec3>& colors) {
	items = item_data.size();
	groups = group_names.size();
	
	this->colors = colors;

	item_labels.resize(items);

	positions.resize(groups);
	group_labels.resize(groups);

	for (int i = 0; i < groups; ++i) {
		positions[i] = i;

		const std::string group_label = group_names[i];

		group_labels[i] = new char[group_label.size() + 1];
		std::copy(group_label.begin(), group_label.end(), group_labels[i]);
		group_labels[i][group_label.size()] = '\0';
	}

	for (int i = 0; i < items; ++i) {
		const std::string item_label = item_data[i].first;

		item_labels[i] = new char[item_label.size() + 1];
		std::copy(item_label.begin(), item_label.end(), item_labels[i]);
		item_labels[i][item_label.size()] = '\0';


		const std::vector<float>& current_item_data = item_data[i].second;

		for (int j = 0; j < groups; ++j)
			plot_data.push_back(current_item_data[j]);
	}

	size = 0.5;

}

GraphData::GraphData(const GraphData& gd)
{
	items = gd.items;
	groups = gd.groups;

	plot_data = gd.plot_data;
	positions = gd.positions;

	colors = gd.colors;

	item_labels.resize(items);
	group_labels.resize(groups);

	for (int i = 0; i < groups; ++i) {
		positions[i] = i;

		const std::string group_label = gd.group_labels[i];

		group_labels[i] = new char[group_label.size() + 1];
		std::copy(group_label.begin(), group_label.end(), group_labels[i]);
		group_labels[i][group_label.size()] = '\0';
	}

	for (int i = 0; i < items; ++i) {
		const std::string item_label = gd.item_labels[i];

		item_labels[i] = new char[item_label.size() + 1];
		std::copy(item_label.begin(), item_label.end(), item_labels[i]);
		item_labels[i][item_label.size()] = '\0';
	}

	size = gd.size;
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

	items = gd.items;
	groups = gd.groups;

	plot_data = gd.plot_data;
	positions = gd.positions;

	colors = gd.colors;

	item_labels.resize(items);
	group_labels.resize(groups);

	for (int i = 0; i < groups; ++i) {
		positions[i] = i;

		const std::string group_label = gd.group_labels[i];

		group_labels[i] = new char[group_label.size() + 1];
		std::copy(group_label.begin(), group_label.end(), group_labels[i]);
		group_labels[i][group_label.size()] = '\0';
	}

	for (int i = 0; i < items; ++i) {
		const std::string item_label = gd.item_labels[i];

		item_labels[i] = new char[item_label.size() + 1];
		std::copy(item_label.begin(), item_label.end(), item_labels[i]);
		item_labels[i][item_label.size()] = '\0';
	}

	size = gd.size;
}