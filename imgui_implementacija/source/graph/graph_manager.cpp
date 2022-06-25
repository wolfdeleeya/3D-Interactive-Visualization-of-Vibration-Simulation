#include <iostream>

#include "graph/graph_manager.h"
#include "implot_helper.h"

void GraphManager::draw_bar_graph(const GraphData& gd, int cell_index)
{
	if (cell_index >= 0)
		MyImPlot::PlotBars(gd, cell_index, m_application_model->graph_bar_width);
	else
		MyImPlot::PlotBarGroups(gd, m_application_model->graph_bar_width);
}

void GraphManager::draw_line_graph(const GraphData& gd, int cell_index)
{
	if (cell_index >= 0)
		MyImPlot::PlotLine(gd, cell_index);
	else {
		unsigned int num_of_lines = gd.item_labels.size();
		for(unsigned int i = 0; i < num_of_lines; ++i)
			MyImPlot::PlotLine(gd, i);
	}
}

void GraphManager::draw_default_comparison()
{
	unsigned int num_of_groups = m_graph_data.group_labels.size();
	unsigned int num_of_items = m_graph_data.item_labels.size();

	if (ImPlot::BeginPlot("Selected Cell Frequencies - Default Display", { -1, -1 })) {
		ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
		ImPlot::SetupAxes("Frequency", "Vibrations", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);

		if (m_graph_data.positions.size() > 0) {
			ImPlot::SetupAxisTicks(ImAxis_X1, &m_graph_data.positions[0], num_of_groups, &m_graph_data.group_labels[0]);

			render_plot_function& render_function = m_render_plot_functions[(unsigned int) m_application_model->current_graph_render_mode()];
			render_function(m_graph_data, -1);
		}

		ImPlot::EndPlot();
	}
}

void GraphManager::draw_subplot_comparison()
{
	unsigned int num_of_selected_cells = m_engine_model->num_of_selected_cells();
	unsigned int num_of_groups = m_graph_data.group_labels.size();

	unsigned int num_of_columns = m_application_model->graph_num_of_columns;

	unsigned int num_of_rows = num_of_selected_cells / num_of_columns;
	num_of_rows = num_of_rows < 1 ? 1 : num_of_rows;	//make sure that the number of rows is not 0

	if (ImPlot::BeginSubplots("Selected Cell Frequencies - Subplots Display", num_of_rows, num_of_columns, { -1, -1 })) {
		for (unsigned int i = 0; i < num_of_selected_cells; ++i) {
			
			std::string plot_title = "Plot of Cell ";
			plot_title += std::to_string(m_engine_model->selected_cell_index(i));

			if (ImPlot::BeginPlot(plot_title.c_str())) {
				ImPlot::SetupAxes("Frequency", "Vibrations", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
				ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
				if (m_graph_data.positions.size() > 0) {
					ImPlot::SetupAxisTicks(ImAxis_X1, &m_graph_data.positions[0], num_of_groups, &m_graph_data.group_labels[0]);

					render_plot_function& render_function = m_render_plot_functions[(unsigned int) m_application_model->current_graph_render_mode()];
					render_function(m_graph_data, i);
				}

				ImPlot::EndPlot();
			}
		}
		ImPlot::EndSubplots();
	}
}

void GraphManager::draw_relative_comparison()
{
	unsigned int num_of_groups = m_cached_relative_graph_data.group_labels.size();
	unsigned int num_of_items = m_cached_relative_graph_data.item_labels.size();
	
	unsigned int current_referent_cell_index = m_application_model->current_graph_referant_cell_index();

	std::string title = "Selected Cell Frequencies - Relative Display - ";
	std::string referent_cell_title = std::string("Relative To Cell ") + std::to_string(current_referent_cell_index);

	title += m_engine_model->does_cell_exist(current_referent_cell_index) ? referent_cell_title : "No Selected Referent Cell";

	if (ImPlot::BeginPlot(title.c_str(), {-1, -1})) {
		ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
		ImPlot::SetupAxes("Frequency", "Vibrations", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);

		if (m_graph_data.positions.size() > 0) {
			ImPlot::SetupAxisTicks(ImAxis_X1, &m_cached_relative_graph_data.positions[0], num_of_groups, &m_cached_relative_graph_data.group_labels[0]);

			render_plot_function& render_function = m_render_plot_functions[(unsigned int)m_application_model->current_graph_render_mode()];
			render_function(m_cached_relative_graph_data, -1);
		}

		ImPlot::EndPlot();
	}
}

void GraphManager::draw_limits_mode_colormap_legend()
{
	unsigned int num_of_selected_frequencies = m_engine_model->num_of_selected_frequencies();

	Gradient mid_gradient = *m_engine_model->get_gradient(EngineModel::GradientVariables::LIMITS_MODE_RISKY_GRADIENT);
	Gradient bad_gradient = *m_engine_model->get_gradient(EngineModel::GradientVariables::LIMITS_MODE_DANGEROUS_GRADIENT);

	MyImPlot::ColormapScale("Limits Mode Risky Colormap", mid_gradient, 0, num_of_selected_frequencies, num_of_selected_frequencies, { 0, -1 }, false);
	ImGui::SameLine(),
	MyImPlot::ColormapScale("Limits Mode Dangerous Colormap", bad_gradient, 0, num_of_selected_frequencies, num_of_selected_frequencies, { 0, -1 }, false);
}

void GraphManager::draw_normal_mode_colormap_legend()
{
	glm::vec2 limits = m_engine_model->get_current_normal_mode_limits();
	MyImPlot::ColormapScale("Normal Mode Colormap", *(m_engine_model->get_gradient(EngineModel::GradientVariables::NORMAL_MODE_GRADIENT)), limits.x, limits.y, 10, { 0, -1 }, true);
}

GraphManager::GraphManager(ApplicationModel* application_model, EngineModel* engine_model) : m_graph_data({}, {}, {}), m_cached_relative_graph_data({}, {}, {})
{
	m_application_model = application_model;
	m_engine_model = engine_model;

	m_application_model->on_graph_referent_cell_changed.add_member_listener(&GraphManager::update_relative_plot, this);

	m_engine_model->on_cell_hovered.add_listener([this](unsigned int id) { this->update_cell_plot(); });
	m_engine_model->on_cell_hovered.add_listener([this](unsigned int id) { this->update_relative_plot(); });

	m_engine_model->on_selected_cells_pallete_changed.add_listener([this](unsigned int pallete_index) {this->update_cell_plot(); });
	m_engine_model->on_selected_cells_pallete_changed.add_listener([this](unsigned int pallete_index) {this->update_relative_plot(); });

	m_engine_model->on_selected_cells_changed.add_member_listener(&GraphManager::update_cell_plot, this);
	m_engine_model->on_selected_cells_changed.add_member_listener(&GraphManager::update_relative_plot, this);

	m_engine_model->on_selected_frequencies_changed.add_member_listener(&GraphManager::update_cell_plot, this); 
	m_engine_model->on_selected_frequencies_changed.add_member_listener(&GraphManager::update_relative_plot, this);

	m_render_plot_functions = {
		std::bind(&GraphManager::draw_bar_graph, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&GraphManager::draw_line_graph, this, std::placeholders::_1, std::placeholders::_2)
	};

	m_comparison_plot_functions = {
		std::bind(&GraphManager::draw_default_comparison, this),
		std::bind(&GraphManager::draw_subplot_comparison, this),
		std::bind(&GraphManager::draw_relative_comparison, this)
	};

	m_colormap_legend_functions = {
	std::bind(&GraphManager::draw_normal_mode_colormap_legend, this),
	std::bind(&GraphManager::draw_limits_mode_colormap_legend, this)
	};
}

void GraphManager::update_cell_plot()
{
	if (!m_engine_model->are_stats_loaded())
		return;

	unsigned int num_of_selected_cells = m_engine_model->num_of_selected_cells();

	std::vector<std::pair<std::string, std::vector<float>>> item_data;
	std::vector<glm::vec3> colors;

	std::vector<unsigned int> selected_cells = m_engine_model->selected_cells();

	unsigned int n_selected_cells = selected_cells.size();

	for (unsigned int i = 0; i < n_selected_cells; ++i) {
		colors.push_back(m_engine_model->get_color_for_selected_cell(i));

		std::pair<std::string, std::vector<float>> data_entry;
		unsigned int cell_id = selected_cells[i];

		data_entry.first = "CELL " + std::to_string(cell_id);
		data_entry.second = m_engine_model->get_values_for_cell(cell_id);

		item_data.push_back(data_entry);
	}

	//add hovered cell to the list
	if (m_engine_model->is_valid_cell_hovered()) {
		colors.push_back(m_application_model->hovered_cell_graph_color);

		std::pair<std::string, std::vector<float>> data_entry;

		unsigned int hovered_cell_id = m_engine_model->hovered_cell();

		data_entry.first = "CELL " + std::to_string(hovered_cell_id);
		data_entry.second = m_engine_model->get_hovered_cell_values();

		item_data.push_back(data_entry);
	}

	m_graph_data = GraphData(m_engine_model->selected_frequencies_names(), item_data, colors);
}

void GraphManager::update_relative_plot()
{
	if (!m_engine_model->are_stats_loaded())
		return;

	std::vector<std::pair<std::string, std::vector<float>>> item_data;
	std::vector<glm::vec3> colors;

	unsigned int current_referent_cell_index = m_application_model->current_graph_referant_cell_index();

	std::vector<unsigned int> selected_cells = m_engine_model->selected_cells();

	unsigned int n_selected_cells = selected_cells.size();

	std::vector<float> referent_cell_data = m_engine_model->get_values_for_cell(current_referent_cell_index);

	for (unsigned int i = 0; i < n_selected_cells; ++i) {
		if (selected_cells[i] == current_referent_cell_index) {
			continue;
		}

		colors.push_back(m_engine_model->get_color_for_selected_cell(i));

		std::pair<std::string, std::vector<float>> data_entry;
		unsigned int cell_id = selected_cells[i];

		data_entry.first = "CELL " + std::to_string(cell_id);
		data_entry.second = m_engine_model->get_values_for_cell(cell_id);

		//this will produce correct relative graph data if the referent cell exists
		for (int i = 0; i < referent_cell_data.size(); ++i)
			data_entry.second[i] -= referent_cell_data[i];

		item_data.push_back(data_entry);
	}

	//add hovered cell to the list
	if (m_engine_model->is_valid_cell_hovered()) {
		colors.push_back(m_application_model->hovered_cell_graph_color);

		std::pair<std::string, std::vector<float>> data_entry;

		unsigned int hovered_cell_id = m_engine_model->hovered_cell();

		data_entry.first = "CELL " + std::to_string(hovered_cell_id);
		data_entry.second = m_engine_model->get_hovered_cell_values();

		//this will produce correct relative graph data if the referent cell exists
		for (int i = 0; i < referent_cell_data.size(); ++i)
			data_entry.second[i] -= referent_cell_data[i];

		item_data.push_back(data_entry);
	}

	m_cached_relative_graph_data = GraphData(m_engine_model->selected_frequencies_names(), item_data, colors);
}

void GraphManager::draw_cell_plot()
{
	comparison_plot_function current_comparison_plot_function = m_comparison_plot_functions[(unsigned int)m_application_model->current_graph_comparison_mode()];
	current_comparison_plot_function();
}

void GraphManager::draw_legend()
{
	unsigned int current_visualization_mode_index = (unsigned int)m_engine_model->current_visualization_mode();
	m_colormap_legend_functions[current_visualization_mode_index]();
}
