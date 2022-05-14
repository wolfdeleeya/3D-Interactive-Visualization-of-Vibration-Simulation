#include <iostream>

#include "graph_manager.h"
#include "implot_helper.h"

GraphManager::GraphManager(ApplicationModel* application_model, EngineData* engine_data): m_graph_data({}, {}, {})
{
	m_application_model = application_model;
	m_engine_data = engine_data;

	m_engine_data->on_cell_hovered.add_listener([this](unsigned int id) { this->update_cell_plot(); });
	m_engine_data->on_selected_cells_changed.add_member_listener(&GraphManager::update_cell_plot, this);
	m_engine_data->on_selected_frequencies_changed.add_member_listener(&GraphManager::update_cell_plot, this);
}

void GraphManager::update_cell_plot()
{
	if (!m_engine_data->are_stats_loaded())
		return;
	static int updated_count = 0;
	++updated_count;
	std::cout << "GRAPH UPDATED: " << updated_count << std::endl;

	std::vector<std::pair<std::string, std::vector<float>>> item_data;
	std::vector<glm::vec3> colors;

	std::vector<unsigned int> selected_cells = m_engine_data->selected_cells();

	unsigned int n_selected_cells = selected_cells.size();

	for (int i = 0; i < n_selected_cells; ++i) {
		colors.push_back(m_engine_data->get_color_for_selected_cell(i, n_selected_cells));

		std::pair<std::string, std::vector<float>> data_entry;
		unsigned int cell_id = selected_cells[i];

		data_entry.first = "CELL " + std::to_string(cell_id);
		data_entry.second = m_engine_data->get_values_for_cell(cell_id);

		item_data.push_back(data_entry);
	}

	//add hovered cell to the list

	colors.push_back(m_engine_data->get_color_for_selected_cell(n_selected_cells, n_selected_cells + 1));

	std::pair<std::string, std::vector<float>> data_entry;

	unsigned int hovered_cell_id = m_engine_data->hovered_cell();

	data_entry.first = "CELL " + std::to_string(hovered_cell_id);
	data_entry.second = m_engine_data->get_hovered_cell_values();

	item_data.push_back(data_entry);

	m_graph_data = GraphData(m_engine_data->selected_frequencies(), item_data, colors);
}

void GraphManager::draw_cell_plot()
{
	unsigned int num_of_groups = m_graph_data.group_labels.size();
	unsigned int num_of_items = m_graph_data.item_labels.size();

	if (ImPlot::BeginPlot("Selected Cell Frequencies")) {
		ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
		ImPlot::SetupAxes("Frequency", "Vibrations", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);

		if (m_graph_data.positions.size() > 0) {
			ImPlot::SetupAxisTicks(ImAxis_X1, &m_graph_data.positions[0], num_of_groups, &m_graph_data.group_labels[0]);
			ImPlot::PlotBarGroups(&m_graph_data.item_labels[0], &m_graph_data.plot_data[0], num_of_items, num_of_groups, m_graph_data.size, 0, 0);
			MyImPlot::PlotBarGroups(m_graph_data);
		}

		ImPlot::EndPlot();
	}
}
