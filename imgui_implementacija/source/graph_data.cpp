#include "graph_data.h"

GraphData::GraphData(const std::vector<std::pair<std::string, float>>& data)
{
	items = 1;
	char dummy[] = "";
	item_labels.push_back(dummy);

	groups = data.size();

	for (int i = 0; i < groups; ++i) {
		const std::string& name = data[i].first;

		frequenzy_labels.push_back(new char[name.size() + 1]);
		std::copy(name.begin(), name.end(), frequenzy_labels[i]);
		frequenzy_labels[i][name.size()] = '\0';

		plot_data.push_back(data[i].second);
		positions.push_back(i);
	}

	size = 0.5;
}

GraphData::GraphData(const std::vector<std::pair<std::string, std::vector<std::pair<std::string, float>>>>& data) {		//ne ovako, odvojeno cemo predat group imena, a odvojeno podatke
	items = data.size();
	groups = data[0].second.size();
	
	item_labels.resize(items);

	positions.resize(groups);
	frequenzy_labels.resize(groups);

	for (int i = 0; i < items; ++i) {
		const std::string item_label = data[i].first;

		item_labels[i] = new char[item_label.size() + 1];
		std::copy(item_label.begin(), item_label.end(), item_labels[i]);
		item_labels[i][item_label.size()] = '\0';


		for (int j = 0; j < groups; ++j) {

		}
	}

}

GraphData::GraphData(const GraphData& gd)
{
	groups = gd.groups;
	items = gd.items;

	for (int i = 0; i < groups; ++i) {
		const std::string name = gd.frequenzy_labels[i];

		frequenzy_labels.push_back(new char[name.size() + 1]);
		std::copy(name.begin(), name.end(), frequenzy_labels[i]);
		frequenzy_labels[i][name.size()] = '\0';

		plot_data.push_back(gd.plot_data[i]);
		positions.push_back(i);
	}

	for (int i = 0; i < items; ++i) {
		const std::string name = gd.item_labels[i];

		item_labels.push_back(new char[name.size() + 1]);
		std::copy(name.begin(), name.end(), item_labels[i]);
		item_labels[i][name.size()] = '\0';
	}

	size = gd.size;
}

GraphData::~GraphData()
{
	for (char* label : frequenzy_labels)
		delete[] label;

	for (char* label : item_labels)
		delete[] label;
}

void GraphData::operator=(const GraphData& gd)
{
	for (char* label : frequenzy_labels)
		delete[] label;

	for (char* label : item_labels)
		delete[] label;

	frequenzy_labels.clear();
	item_labels.clear();
	plot_data.clear();
	positions.clear();

	groups = gd.groups;
	items = gd.items;

	for (int i = 0; i < groups; ++i) {
		const std::string name = gd.frequenzy_labels[i];

		frequenzy_labels.push_back(new char[name.size() + 1]);
		std::copy(name.begin(), name.end(), frequenzy_labels[i]);
		frequenzy_labels[i][name.size()] = '\0';

		plot_data.push_back(gd.plot_data[i]);
		positions.push_back(i);
	}

	for (int i = 0; i < items; ++i) {
		const std::string name = gd.item_labels[i];

		item_labels.push_back(new char[name.size() + 1]);
		std::copy(name.begin(), name.end(), item_labels[i]);
		item_labels[i][name.size()] = '\0';
	}

	size = gd.size;
}