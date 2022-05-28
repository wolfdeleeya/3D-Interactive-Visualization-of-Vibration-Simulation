#include <iostream>

#include "graph/graph_manager.h"
#include "implot_helper.h"

void GraphManager::draw_bar_graph(const GraphData& gd, int cell_index)
{
	unsigned int num_of_groups = m_graph_data.group_labels.size();
	unsigned int num_of_items = m_graph_data.item_labels.size();

	if (ImPlot::BeginPlot("Selected Cell Frequencies")) {
		ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
		ImPlot::SetupAxes("Frequency", "Vibrations", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);

		if (m_graph_data.positions.size() > 0) {
			ImPlot::SetupAxisTicks(ImAxis_X1, &m_graph_data.positions[0], num_of_groups, &m_graph_data.group_labels[0]);
			BarGraphSettings* bar_settings = (BarGraphSettings*)m_render_graph_settings[(unsigned int)RenderMode::BARS];
			MyImPlot::PlotBarGroups(m_graph_data, bar_settings->bar_width);
		}

		ImPlot::EndPlot();
	}
}

void GraphManager::draw_line_graph(const GraphData& gd, int cell_index = -1)
{
	MyImPlot::PlotLine(m_graph_data, cell_index);
}

void GraphManager::draw_default_comparison()
{
	render_plot_function& render_function = m_render_plot_functions[(unsigned int)m_current_comparison_mode];
	render_function(m_graph_data, -1);
}

void GraphManager::draw_relative_comparison()
{
	render_plot_function& render_function = m_render_plot_functions[(unsigned int)m_current_comparison_mode];
	render_function(m_cached_relative_graph_data, -1);
}

void GraphManager::draw_graph_settings_widget()
{
	ImGui::ColorEdit3("Hovered Cell Graph Color", glm::value_ptr(m_hovered_cell_graph_color));
	m_render_graph_settings[(unsigned int)m_current_render_mode]->draw();
}

GraphManager::GraphManager(ApplicationModel* application_model, EngineData* engine_data) : m_graph_data({}, {}, {}), m_cached_relative_graph_data({}, {}, {})
{
	m_application_model = application_model;
	m_engine_data = engine_data;

	m_engine_data->on_cell_hovered.add_listener([this](unsigned int id) { this->update_cell_plot(); });
	m_engine_data->on_selected_cells_pallete_changed.add_listener([this](unsigned int pallete_index) {this->update_cell_plot(); });

	m_engine_data->on_selected_cells_changed.add_member_listener(&GraphManager::update_cell_plot, this);
	m_engine_data->on_selected_frequencies_changed.add_member_listener(&GraphManager::update_cell_plot, this);

	m_render_graph_settings = {
		new BarGraphSettings(0.5),
		new BaseGraphSettings()
	};

	m_comparison_graph_settings = {
		new BaseGraphSettings(),
		new SubplotsComparisonSettings(m_engine_data, 1),
		new RelativeComparisonSettings(engine_data)
	};

	m_render_plot_functions = {
		std::bind(&GraphManager::draw_bar_graph, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&GraphManager::draw_line_graph, this, std::placeholders::_1, std::placeholders::_2)
	};

	m_comparison_plot_functions = {
		std::bind(&GraphManager::draw_default_comparison, this),
		std::bind(&GraphManager::draw_subplot_comparison, this),
		std::bind(&GraphManager::draw_relative_comparison, this)
	};

	set_render_mode(RenderMode::BARS);
	set_comparison_mode(ComparisonMode::DEFAULT);

	m_hovered_cell_graph_color = { 0.835, 0.662, 0.427 };
}

GraphManager::~GraphManager()
{
	for (auto& settings_ptr : m_render_graph_settings)
		delete settings_ptr;

	for (auto& settings_ptr : m_comparison_graph_settings)
		delete settings_ptr;
}

void GraphManager::update_cell_plot()
{
	if (!m_engine_data->are_stats_loaded())
		return;
	static int updated_count = 0;
	++updated_count;

	std::vector<std::pair<std::string, std::vector<float>>> item_data;
	std::vector<glm::vec3> colors;

	std::vector<unsigned int> selected_cells = m_engine_data->selected_cells();

	unsigned int n_selected_cells = selected_cells.size();

	for (int i = 0; i < n_selected_cells; ++i) {
		colors.push_back(m_engine_data->get_color_for_selected_cell(i));

		std::pair<std::string, std::vector<float>> data_entry;
		unsigned int cell_id = selected_cells[i];

		data_entry.first = "CELL " + std::to_string(cell_id);
		data_entry.second = m_engine_data->get_values_for_cell(cell_id);

		item_data.push_back(data_entry);
	}

	//add hovered cell to the list

	colors.push_back(m_hovered_cell_graph_color);

	std::pair<std::string, std::vector<float>> data_entry;

	unsigned int hovered_cell_id = m_engine_data->hovered_cell();

	data_entry.first = "CELL " + std::to_string(hovered_cell_id);
	data_entry.second = m_engine_data->get_hovered_cell_values();

	item_data.push_back(data_entry);

	m_graph_data = GraphData(m_engine_data->selected_frequencies(), item_data, colors);
}

void GraphManager::draw_cell_plot()
{
	draw_graph_settings_widget();
	comparison_plot_function current_comparison_plot_function = m_comparison_plot_functions[(unsigned int)m_current_comparison_mode];
	current_comparison_plot_function();

	draw_graph_settings_widget();
}

void GraphManager::ShowDemo_FilledLinePlots() {

	if (m_graph_data.plot_data.size() == 0)
		return;
	static double xs1[101], ys1[101], ys2[101], ys3[101];
	srand(0);
	for (int i = 0; i < 101; ++i) {
		xs1[i] = (float)i;
		ys1[i] = 400 + rand() % 50;
		ys2[i] = 275 + rand() % 75;
		ys3[i] = 150 + rand() % 75;
	}
	static bool show_lines = true;
	static bool show_fills = true;
	static float fill_ref = 0;
	static int shade_mode = 0;
	ImGui::Checkbox("Lines", &show_lines); ImGui::SameLine();
	ImGui::Checkbox("Fills", &show_fills);
	if (show_fills) {
		ImGui::SameLine();
		if (ImGui::RadioButton("To -INF", shade_mode == 0))
			shade_mode = 0;
		ImGui::SameLine();
		if (ImGui::RadioButton("To +INF", shade_mode == 1))
			shade_mode = 1;
		ImGui::SameLine();
		if (ImGui::RadioButton("To Ref", shade_mode == 2))
			shade_mode = 2;
		if (shade_mode == 2) {
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100);
			ImGui::DragFloat("##Ref", &fill_ref, 1, -100, 500);
		}
	}

	if (ImPlot::BeginPlot("Stock Prices")) {
		ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
		ImPlot::SetupAxes("Frequency", "Vibrations", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
		ImPlot::SetupAxesLimits(0, 100, 0, 500);

		unsigned int num_of_groups = m_graph_data.group_labels.size();
		unsigned int num_of_items = m_graph_data.item_labels.size();

		ImPlot::SetupAxisTicks(ImAxis_X1, &m_graph_data.positions[0], num_of_groups, &m_graph_data.group_labels[0]);

		for (int i = 0; i < num_of_items; ++i) {
			MyImPlot::PlotLine(m_graph_data, i);
		}
		ImPlot::EndPlot();
	}
}