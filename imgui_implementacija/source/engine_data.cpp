#include <iostream>
#include <algorithm>

#include "engine_data.h"
#include "glm_vec_helper.h"
#include "color_helper.h"

const std::vector <std::shared_ptr<cell_functors::AbstractCellFunctor>> EngineData::CELL_FUNCTIONS{
	std::make_shared<cell_functors::MinFunctor>(),
	std::make_shared<cell_functors::MaxFunctor>(),
	std::make_shared<cell_functors::AvgFunctor>(),
	std::make_shared<cell_functors::MedianFunctor>(),
	std::make_shared<cell_functors::SpreadFunctor>(),
};

const char* EngineData::FUNCTION_NAMES[5] {"MIN", "MAX", "AVERAGE", "MEDIAN", "SPREAD"};
const char* EngineData::LIMITS_NAMES[3] {"GLOBAL", "LOCAL", "USER DEFINED"};

void EngineData::calculate_color()
{
	std::map<unsigned int, glm::vec3> color_map;

	unsigned int n_selected_frequencies = m_selected_frequencies_names.size();

	unsigned int n_selected_cells = m_selected_cells.size();

	for (int i = 0; i < n_selected_cells; ++i)
		color_map[m_selected_cells[i]] = convert_hsv_to_rgb({ float(i + 1) / (n_selected_cells + 1), 1, 1 });

	glm::vec2& limits = m_limits[*get_uint(UnsignedIntVariables::NORMAL_MODE_LIMITS)];

	const auto& selected_cells_begin = m_selected_cells.begin();
	const auto& selected_cells_end = m_selected_cells.end();

	for (unsigned int index : m_cell_indeces) {
		if (std::find(selected_cells_begin, selected_cells_end, index) != selected_cells_end)	//if is selected ignore loop
			continue;

		if (n_selected_frequencies == 0)
			color_map[index] = *get_color(ColorVariables::DEFAULT_COLOR);
		else if (n_selected_frequencies == 1) {
			auto& cell_stats = m_cell_stats[index];
			
			float value = cell_stats.freq_map[m_selected_frequencies_names[0]];
			float norm_value = (value - limits.x) / (limits.y - limits.x);

			color_map[index] = get_gradient(GradientVariables::NORMAL_MODE_GRADIENT)->evaluate(norm_value);
		}
		else {
			auto& cell_stats = m_cell_stats[index];

			std::vector<float> values;
			for (int i = 0; i < n_selected_frequencies; ++i)
				values.push_back(cell_stats.freq_map[m_selected_frequencies_names[i]]);

			float value = (*CELL_FUNCTIONS[*get_uint(UnsignedIntVariables::NORMAL_MODE_FUNCTION)])(values);
			float norm_value = (value - limits.x) / (limits.y - limits.x);

			color_map[index] = get_gradient(GradientVariables::NORMAL_MODE_GRADIENT)->evaluate(norm_value);
		}
	}
	on_colors_recalculated.invoke(color_map);
}

void EngineData::refresh_cached_values()
{
	m_limits[USER_DEFINED] = user_limits;
}

void EngineData::find_local_limits()
{
	if (m_selected_frequencies_names.size() <= 0)
		return;
	
	glm::vec2 local_limits(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());

	for (const auto& pair : m_cell_stats) {
		const std::map<std::string, float>& map = pair.second.freq_map;

		for (const std::string& name : m_selected_frequencies_names) {
			if (map.count(name) > 0) {
				float value = map.at(name);

				local_limits.x = local_limits.x > value ? value : local_limits.x;		//set min limit
				local_limits.y = local_limits.y < value ? value : local_limits.y;		//set max limit
			}
			else {
				std::cout << "CELL " << pair.first << " DOESN'T HAVE FRQ " << name << std::endl;
			}
		}
	}

	m_limits[LOCAL] = local_limits;
}

void EngineData::find_global_limits()
{
	glm::vec2 global_limits(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());

	for (const auto& pair : m_cell_stats) {
		const std::map<std::string, float>& map = pair.second.freq_map;

		for (const std::string& name : m_frequenzy_names) {
			float value = map.at(name);

			global_limits.x = global_limits.x > value ? value : global_limits.x;		//set min limit
			global_limits.y = global_limits.y < value ? value : global_limits.y;		//set max limit
		}
	}

	m_limits[GLOBAL] = global_limits;
}

GraphData EngineData::generate_graph_data_selected_cells()
{
	if(m_cell_stats.size() == 0)
		return GraphData({}, m_hovered_cell_color);

	std::vector<std::pair<std::string, std::vector<float>>> item_data;
	std::vector<glm::vec3> colors;

	unsigned int n_selected_cells = m_selected_cells.size();

	for (int i = 0; i < n_selected_cells; ++i) {
		colors.push_back(convert_hsv_to_rgb({ float(i + 1) / (n_selected_cells + 1), 1, 1 }));

		std::pair<std::string, std::vector<float>> data_entry;
		unsigned int cell_id = m_selected_cells[i];
		data_entry.first = "CELL " + std::to_string(cell_id);

		const auto& values = get_values_for_cell(cell_id);

		for (const auto& pair : values)
			data_entry.second.push_back(pair.second);		//ovo mijenjamo, trebamo odvojiti frequenzy name od same frekvencije, ako su vec poredane

		item_data.push_back(data_entry);
	}

	return GraphData(m_selected_frequencies_names, item_data, colors);
}

GraphData EngineData::generate_graph_data_hovered_cell()
{
	if (m_hovered_cell == 0 || m_cell_stats.size() == 0)
		return GraphData({}, m_hovered_cell_color);
	return GraphData(get_hovered_cell_values(), m_hovered_cell_color);
}

std::vector<std::pair<std::string, float>> EngineData::get_empty_cell_values()
{
	std::vector<std::pair<std::string, float>> result;

	for (const auto& name : m_selected_frequencies_names)
		result.push_back({ name, 0 });

	return result;
}

EngineData::EngineData(const glm::vec3& color) : m_frq_comparator({}),
m_gradient_variables([](const Gradient& g1, const Gradient& g2) {return g1 == g2; }, GradientVariables::END, Gradient{ glm::vec3(1), glm::vec3(0) }),
m_color_variables([](const glm::vec3& c1, const glm::vec3& c2) { return are_equal(c1, c2); }, ColorVariables::END, glm::vec3(0)),
m_uint_variables([](unsigned int ui1, unsigned int ui2) {return ui1 == ui2; }, UnsignedIntVariables::END, 0)
{
	*get_uint(UnsignedIntVariables::NORMAL_MODE_LIMITS) = GLOBAL;
	*get_uint(UnsignedIntVariables::NORMAL_MODE_FUNCTION) = AVERAGE;

	*get_color(ColorVariables::DEFAULT_COLOR) = color;

	m_update_graph_on_hover = true;

	m_hovered_cell_color = { 1, 0, 1 };

	m_limits = { {0, 0}, {0, 0}, {0, 0} };
	refresh_cached_values();
}

void EngineData::load_cell_stats(const char* path)
{
	m_cell_stats = loader::load_cell_stats(path, m_frequenzy_names);

	m_cell_indeces.clear();

	for (const auto& pair : m_cell_stats)
		m_cell_indeces.push_back(pair.first);

	m_frq_comparator.all_names = m_frequenzy_names;

	find_global_limits();

	calculate_color();
}

void EngineData::load_frequenzy_limits(const char* path)
{
	m_frequenzy_limits = loader::load_frequenzy_limits(path);
}

void EngineData::on_cell_vertices_loaded(const char* path)
{
	if (m_cell_stats.size() != 0)
		return;

	const auto& cell_vertices = loader::load_cells(path);
	
	m_cell_indeces.clear();
	
	for (const auto& pair : cell_vertices)
		m_cell_indeces.push_back(pair.first);

	calculate_color();
}

void EngineData::check_for_changes()
{
	bool are_changes_pending = false;

	if (!are_equal(m_limits[USER_DEFINED], user_limits))
		are_changes_pending = true;

	are_changes_pending |= m_gradient_variables.check_for_changes();
	are_changes_pending |= m_color_variables.check_for_changes();
	are_changes_pending |= m_uint_variables.check_for_changes();

	if (are_changes_pending)
	{
		calculate_color();
		refresh_cached_values();
	}
}

void EngineData::select_frequency(const std::string& f_name, bool is_selected)
{
	if (is_selected)				//if true add name
		m_selected_frequencies_names.push_back(f_name);
	else {					//if false delete name
		const auto& last = std::remove(m_selected_frequencies_names.begin(), m_selected_frequencies_names.end(), f_name);
		m_selected_frequencies_names.erase(last);
	}

	std::sort(m_selected_frequencies_names.begin(), m_selected_frequencies_names.end(), m_frq_comparator);

	find_local_limits();

	calculate_color();

	if (m_update_graph_on_hover)
		on_graph_data_changed.invoke(generate_graph_data_hovered_cell());
	else
		on_graph_data_changed.invoke(generate_graph_data_selected_cells());
}

void EngineData::clear_selection()
{
	m_selected_frequencies_names.clear();

	calculate_color();
}

void EngineData::clear_hovered_cell()
{
	if (is_valid_cell_hovered())
		set_hovered_cell(0);
}

void EngineData::clear_selected_cells()
{
	if (m_selected_cells.size() > 0)
	{
		m_selected_cells.clear();
		calculate_color();

		on_graph_data_changed.invoke(generate_graph_data_selected_cells());
	}
}

void EngineData::handle_cell_selection()
{
	if (m_hovered_cell <= 0)
		return;

	const auto& hovered_it = std::find(m_selected_cells.begin(), m_selected_cells.end(), m_hovered_cell);
	
	if (hovered_it != m_selected_cells.end())		
		m_selected_cells.erase(hovered_it);			//if it's already selected, erase it from selection		
	else
		m_selected_cells.push_back(m_hovered_cell);	//else select it

	calculate_color();

	on_graph_data_changed.invoke(generate_graph_data_selected_cells());
}

void EngineData::set_hovered_cell(unsigned int cell_index)
{
	m_hovered_cell = cell_index;
	on_cell_hovered.invoke(cell_index);

	if (m_update_graph_on_hover)
		on_graph_data_changed.invoke(generate_graph_data_hovered_cell());
}

std::vector<std::pair<std::string, float>> EngineData::get_values_for_cell(unsigned int index) 
{
	std::vector<std::pair<std::string, float>> result;
	
	cell_stats stats = m_cell_stats.at(index);

	for (const auto& name : m_selected_frequencies_names)
		result.push_back({ name, stats.freq_map[name] });
	
	return result;
}

void EngineData::update_graph_on_hover(bool value)
{
	m_update_graph_on_hover = value;
	on_graph_data_changed.invoke(m_update_graph_on_hover ? generate_graph_data_hovered_cell() : generate_graph_data_selected_cells());
}
