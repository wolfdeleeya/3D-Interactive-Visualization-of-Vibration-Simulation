#include <iostream>

#include "graph/graph_manager.h"
#include "implot_helper.h"

const std::vector<const char*> GraphManager::RENDER_MODE_LABELS = {
	"Bars Mode", 
	"Lines Mode"
};

const std::vector<const char*> GraphManager::COMPARISON_MODE_LABELS = {
	"Default Comparison Mode",
	"Subplots Comparison Mode",
	"Relative Comparison Mode"
};

void GraphManager::draw_bar_graph(const GraphData& gd, int cell_index)
{
	BarGraphSettings* bar_settings = (BarGraphSettings*)m_render_graph_settings[(unsigned int)RenderMode::BARS];
	if (cell_index >= 0)
		MyImPlot::PlotBars(gd, cell_index, bar_settings->bar_width);
	else
		MyImPlot::PlotBarGroups(gd, bar_settings->bar_width);
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

			render_plot_function& render_function = m_render_plot_functions[(unsigned int)m_current_render_mode];
			render_function(m_graph_data, -1);
		}

		ImPlot::EndPlot();
	}
}

void GraphManager::draw_subplot_comparison()
{
	unsigned int num_of_selected_cells = m_engine_data->num_of_selected_cells();
	unsigned int num_of_groups = m_graph_data.group_labels.size();

	unsigned int num_of_columns = ((SubplotsComparisonSettings*)m_comparison_graph_settings[(unsigned int)ComparisonMode::SUBPLOTS])->num_of_columns;
	unsigned int num_of_rows = num_of_selected_cells / num_of_columns;
	num_of_rows = num_of_rows < 1 ? 1 : num_of_rows;	//make sure that the number of rows is not 0

	if (ImPlot::BeginSubplots("Selected Cell Frequencies - Subplot Display", num_of_rows, num_of_columns, { -1, -1 })) {
		for (unsigned int i = 0; i < num_of_selected_cells; ++i) {
			
			std::string plot_title = "Plot of Cell ";
			plot_title += std::to_string(m_engine_data->selected_cell_index(i));

			if (ImPlot::BeginPlot(plot_title.c_str())) {
				ImPlot::SetupAxes("Frequency", "Vibrations", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
				ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
				if (m_graph_data.positions.size() > 0) {
					ImPlot::SetupAxisTicks(ImAxis_X1, &m_graph_data.positions[0], num_of_groups, &m_graph_data.group_labels[0]);

					render_plot_function& render_function = m_render_plot_functions[(unsigned int)m_current_render_mode];
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

	if (ImPlot::BeginPlot("Selected Cell Frequencies - Relative Display", {-1, -1})) {
		ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
		ImPlot::SetupAxes("Frequency", "Vibrations", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);

		if (m_graph_data.positions.size() > 0) {
			ImPlot::SetupAxisTicks(ImAxis_X1, &m_cached_relative_graph_data.positions[0], num_of_groups, &m_cached_relative_graph_data.group_labels[0]);

			render_plot_function& render_function = m_render_plot_functions[(unsigned int)m_current_render_mode];
			render_function(m_cached_relative_graph_data, -1);
		}

		ImPlot::EndPlot();
	}
}

void GraphManager::draw_limits_mode_colormap_legend()
{
	unsigned int num_of_selected_frequencies = m_engine_data->num_of_selected_frequencies();

	Gradient mid_gradient = *m_engine_data->get_gradient(EngineData::GradientVariables::LIMITS_MODE_MID_GRADIENT);
	Gradient bad_gradient = *m_engine_data->get_gradient(EngineData::GradientVariables::LIMITS_MODE_BAD_GRADIENT);

	MyImPlot::ColormapScale("Limits Mode Mid Colormap", mid_gradient, 0, num_of_selected_frequencies, num_of_selected_frequencies, { 0, -1 }, false);
	ImGui::SameLine(),
	MyImPlot::ColormapScale("Limits Mode Bad Colormap", bad_gradient, 0, num_of_selected_frequencies, num_of_selected_frequencies, { 0, -1 }, false);
}

void GraphManager::draw_normal_mode_colormap_legend()
{
	glm::vec2 limits = m_engine_data->get_current_normal_mode_limits();
	MyImPlot::ColormapScale("Normal Mode Colormap", *(m_engine_data->get_gradient(EngineData::GradientVariables::NORMAL_MODE_GRADIENT)), limits.x, limits.y, 3, { 0, -1 }, true);
}

GraphManager::GraphManager(ApplicationModel* application_model, EngineData* engine_data) : m_graph_data({}, {}, {}), m_cached_relative_graph_data({}, {}, {})
{
	m_application_model = application_model;
	m_engine_data = engine_data;

	m_engine_data->on_cell_hovered.add_listener([this](unsigned int id) { this->update_cell_plot(); });
	m_engine_data->on_cell_hovered.add_listener([this](unsigned int id) { this->update_relative_plot(); });

	m_engine_data->on_selected_cells_pallete_changed.add_listener([this](unsigned int pallete_index) {this->update_cell_plot(); });
	m_engine_data->on_selected_cells_pallete_changed.add_listener([this](unsigned int pallete_index) {this->update_relative_plot(); });

	m_engine_data->on_selected_cells_changed.add_member_listener(&GraphManager::update_cell_plot, this);
	m_engine_data->on_selected_cells_changed.add_member_listener(&GraphManager::update_relative_plot, this);

	m_engine_data->on_selected_frequencies_changed.add_member_listener(&GraphManager::update_cell_plot, this); 
	m_engine_data->on_selected_frequencies_changed.add_member_listener(&GraphManager::update_relative_plot, this);

	m_application_model->on_limits_mode_toggled.add_listener(std::bind(&GraphManager::limits_mode_toggled, this, std::placeholders::_1));

	m_render_graph_settings = {
		new BarGraphSettings(0.5),
		new BaseGraphSettings()
	};

	m_comparison_graph_settings = {
		new BaseGraphSettings(),
		new SubplotsComparisonSettings(m_engine_data, 1),
		new RelativeComparisonSettings(m_engine_data)
	};

	//hook up to event so that referant cell index updated accordingly
	((RelativeComparisonSettings*)m_comparison_graph_settings[(unsigned int)ComparisonMode::RELATIVE])->on_referent_cell_changed.add_member_listener(&GraphManager::referant_cell_changed, this);

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

	m_colormap_legend_plot_function = std::bind(&GraphManager::draw_normal_mode_colormap_legend, this);

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

	std::vector<std::pair<std::string, std::vector<float>>> item_data;
	std::vector<glm::vec3> colors;

	std::vector<unsigned int> selected_cells = m_engine_data->selected_cells();

	unsigned int n_selected_cells = selected_cells.size();

	for (unsigned int i = 0; i < n_selected_cells; ++i) {
		colors.push_back(m_engine_data->get_color_for_selected_cell(i));

		std::pair<std::string, std::vector<float>> data_entry;
		unsigned int cell_id = selected_cells[i];

		data_entry.first = "CELL " + std::to_string(cell_id);
		data_entry.second = m_engine_data->get_values_for_cell(cell_id);

		item_data.push_back(data_entry);
	}

	//add hovered cell to the list
	if (m_engine_data->is_valid_cell_hovered()) {
		colors.push_back(m_hovered_cell_graph_color);

		std::pair<std::string, std::vector<float>> data_entry;

		unsigned int hovered_cell_id = m_engine_data->hovered_cell();

		data_entry.first = "CELL " + std::to_string(hovered_cell_id);
		data_entry.second = m_engine_data->get_hovered_cell_values();

		item_data.push_back(data_entry);
	}

	m_graph_data = GraphData(m_engine_data->selected_frequencies(), item_data, colors);
}

void GraphManager::update_relative_plot()
{
	if (!m_engine_data->are_stats_loaded())
		return;

	std::vector<std::pair<std::string, std::vector<float>>> item_data;
	std::vector<glm::vec3> colors;

	std::vector<unsigned int> selected_cells = m_engine_data->selected_cells();

	unsigned int n_selected_cells = selected_cells.size();

	std::vector<float> referant_cell_data = m_engine_data->get_values_for_cell(m_current_referent_cell_index);

	for (unsigned int i = 0; i < n_selected_cells; ++i) {
		if (selected_cells[i] == m_current_referent_cell_index) {
			continue;
		}

		colors.push_back(m_engine_data->get_color_for_selected_cell(i));

		std::pair<std::string, std::vector<float>> data_entry;
		unsigned int cell_id = selected_cells[i];

		data_entry.first = "CELL " + std::to_string(cell_id);
		data_entry.second = m_engine_data->get_values_for_cell(cell_id);

		//this will produce correct relative graph data if the referant cell exists
		for (int i = 0; i < referant_cell_data.size(); ++i)
			data_entry.second[i] -= referant_cell_data[i];

		item_data.push_back(data_entry);
	}

	//add hovered cell to the list
	if (m_engine_data->is_valid_cell_hovered()) {
		colors.push_back(m_hovered_cell_graph_color);

		std::pair<std::string, std::vector<float>> data_entry;

		unsigned int hovered_cell_id = m_engine_data->hovered_cell();

		data_entry.first = "CELL " + std::to_string(hovered_cell_id);
		data_entry.second = m_engine_data->get_hovered_cell_values();

		//this will produce correct relative graph data if the referant cell exists
		for (int i = 0; i < referant_cell_data.size(); ++i)
			data_entry.second[i] -= referant_cell_data[i];

		item_data.push_back(data_entry);
	}

	m_cached_relative_graph_data = GraphData(m_engine_data->selected_frequencies(), item_data, colors);
}

void GraphManager::referant_cell_changed(unsigned int new_referant_cell_index)
{
	m_current_referent_cell_index = new_referant_cell_index;
	update_relative_plot();
}

void GraphManager::draw_cell_plot()
{
	comparison_plot_function current_comparison_plot_function = m_comparison_plot_functions[(unsigned int)m_current_comparison_mode];
	current_comparison_plot_function();
}

void GraphManager::switch_render_mode()
{
	unsigned int next_mode = ((unsigned int)m_current_render_mode + 1) % (unsigned int)RenderMode::END;
	set_render_mode((RenderMode)next_mode);
}

void GraphManager::switch_comparison_mode()
{
	unsigned int next_mode = ((unsigned int)m_current_comparison_mode + 1) % (unsigned int)ComparisonMode::END;
	set_comparison_mode((ComparisonMode)next_mode);
}

void GraphManager::draw_graph_settings()
{
	const char* render_mode_button_label = RENDER_MODE_LABELS[(unsigned int)m_current_render_mode];
	if (ImGui::Button(render_mode_button_label)) {
		switch_render_mode();
	}

	const char* comparison_mode_button_label = COMPARISON_MODE_LABELS[(unsigned int)m_current_comparison_mode];
	if (ImGui::Button(comparison_mode_button_label)) {
		switch_comparison_mode();
	}

	ImGui::ColorEdit3("Hovered Cell Graph Color", glm::value_ptr(m_hovered_cell_graph_color));

	m_render_graph_settings[(unsigned int)m_current_render_mode]->draw();

	m_comparison_graph_settings[(unsigned int)m_current_comparison_mode]->draw();
}