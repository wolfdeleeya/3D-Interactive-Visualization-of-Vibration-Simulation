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

	m_normal_mode_limits_variables.set(NormalModeLimitsVariables::LOCAL, local_limits);
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

	m_normal_mode_limits_variables.set(NormalModeLimitsVariables::GLOBAL, global_limits);
}

glm::vec3 EngineData::calculate_limits_color_for_cell(unsigned int cell_index)
{
	unsigned int num_of_valid_frequencies = 0;
	unsigned int num_of_mid = 0;
	unsigned int num_of_bad = 0;

	auto& stats = m_cell_stats.at(cell_index).freq_map;

	for (std::string frequency : selected_frequencies()) {
		if (m_frequenzy_limits.find(frequency) != m_frequenzy_limits.end()) {
			++num_of_valid_frequencies;

			glm::vec2& l = m_frequenzy_limits.at(frequency);
			float val = stats.at(frequency);

			if (val >= l.x && val <= l.y)
				++num_of_mid;
			else if (val >= l.y)
				++num_of_bad;
		}
	}

	if (num_of_bad > 0) {			//if there are bad frequencies sample from bad gradient
		Gradient& bad_gradient = *get_gradient(GradientVariables::LIMITS_MODE_BAD_GRADIENT);
		return bad_gradient.evaluate(float(num_of_bad) / num_of_valid_frequencies);
	} else if (num_of_mid > 0) {	//if there are no bad frequencies, but mids exist, sample from mid gradient
		Gradient& mid_gradient = *get_gradient(GradientVariables::LIMITS_MODE_MID_GRADIENT);
		return mid_gradient.evaluate(float(num_of_mid) / num_of_valid_frequencies);
	}								//if there are valid cells, and no bad or mid frequencies, return good color
	else if (num_of_valid_frequencies > 0) {
		glm::vec3& good_color = *get_color(ColorVariables::GOOD_LIMITS_COLOR);
		return good_color;
	}
	else {							//else return default if there are no valid frequencies
		glm::vec3& default_color = *get_color(ColorVariables::DEFAULT_COLOR);
		return default_color;
	}
}

void EngineData::normal_mode_coloring(std::map<unsigned int, glm::vec3>& color_map)
{
	unsigned int n_selected_frequencies = m_selected_frequencies_names.size();

	NormalModeLimitsVariables current_limit_mode = (NormalModeLimitsVariables)*get_uint(UnsignedIntVariables::NORMAL_MODE_LIMITS);

	glm::vec2& limits = *get_normal_mode_limits(current_limit_mode);

	const auto& selected_cells_begin = m_selected_cells.begin();
	const auto& selected_cells_end = m_selected_cells.end();

	for (unsigned int index : m_cell_indeces) {
		if (std::find(selected_cells_begin, selected_cells_end, index) != selected_cells_end)	//if is selected ignore loop
			continue;

		if (n_selected_frequencies == 1) {
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
}

void EngineData::default_coloring(std::map<unsigned int, glm::vec3>& color_map)
{
	const auto& selected_cells_begin = m_selected_cells.begin();
	const auto& selected_cells_end = m_selected_cells.end();

	for (unsigned int index : m_cell_indeces) {
		if (std::find(selected_cells_begin, selected_cells_end, index) != selected_cells_end)	//if is selected ignore loop
			continue;

		color_map[index] = *get_color(ColorVariables::DEFAULT_COLOR);
	}
}

void EngineData::limits_mode_coloring(std::map<unsigned int, glm::vec3>& color_map)
{
	const auto& selected_cells_begin = m_selected_cells.begin();
	const auto& selected_cells_end = m_selected_cells.end();

	for (unsigned int index : m_cell_indeces) {
		if (std::find(selected_cells_begin, selected_cells_end, index) != selected_cells_end)	//if is selected ignore loop
			continue;

		color_map[index] = calculate_limits_color_for_cell(index);
	}
}

EngineData::EngineData(const glm::vec3& color) : m_frq_comparator({}),
	m_gradient_variables([](const Gradient& g1, const Gradient& g2) {return g1 == g2; }, GradientVariables::END, Gradient{ glm::vec3(1), glm::vec3(0) }, std::bind(&EngineData::calculate_color, this)),
	m_color_variables([](const glm::vec3& c1, const glm::vec3& c2) { return are_equal(c1, c2); }, ColorVariables::END, glm::vec3(0), std::bind(&EngineData::calculate_color, this)),
	m_uint_variables([](unsigned int ui1, unsigned int ui2) {return ui1 == ui2; }, UnsignedIntVariables::END, 0, std::bind(&EngineData::calculate_color, this)),
	m_normal_mode_limits_variables([](const glm::vec2& v1, const glm::vec2& v2) { return are_equal(v1, v2); },NormalModeLimitsVariables::END, glm::vec2(0), std::bind(&EngineData::calculate_color, this))
{
	set_uint(UnsignedIntVariables::NORMAL_MODE_LIMITS, (unsigned int)NormalModeLimitsVariables::LOCAL);
	set_uint(UnsignedIntVariables::NORMAL_MODE_FUNCTION, (unsigned int)CellFunctions::AVERAGE);

	set_color(ColorVariables::DEFAULT_COLOR, color);

	set_normal_mode_coloring();
}

void EngineData::calculate_color()
{
	std::map<unsigned int, glm::vec3> color_map;

	unsigned int n_selected_frequencies = m_selected_frequencies_names.size();

	unsigned int n_selected_cells = m_selected_cells.size();

	for (int i = 0; i < n_selected_cells; ++i)
		color_map[m_selected_cells[i]] = get_color_for_selected_cell(i, n_selected_cells);

	if (n_selected_frequencies == 0)
		default_coloring(color_map);
	else
		m_cell_coloring_function(color_map);

	on_colors_recalculated.invoke(color_map);
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

	on_cell_stats_loaded.invoke();
}

void EngineData::load_frequency_limits(const char* path)
{
	m_frequenzy_limits = loader::load_frequenzy_limits(path, m_frequencies_with_limits);

	on_frequency_limits_loaded.invoke();
}

void EngineData::load_cell_vertices(const char* path)		//used to set default color when stats are not yet loaded
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

	are_changes_pending |= m_gradient_variables.check_for_changes();
	are_changes_pending |= m_color_variables.check_for_changes();
	are_changes_pending |= m_uint_variables.check_for_changes();
	are_changes_pending |= m_normal_mode_limits_variables.check_for_changes();
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

	on_selected_frequencies_changed.invoke();
}

void EngineData::clear_frequency_selection()
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

		on_selected_cells_changed.invoke();
	}
}

void EngineData::handle_mouse_click()
{
	if (m_hovered_cell <= 0)
		return;

	const auto& hovered_it = std::find(m_selected_cells.begin(), m_selected_cells.end(), m_hovered_cell);
	
	if (hovered_it != m_selected_cells.end())		
		m_selected_cells.erase(hovered_it);			//if it's already selected, erase it from selection		
	else
		m_selected_cells.push_back(m_hovered_cell);	//else select it

	calculate_color();

	on_selected_cells_changed.invoke();
}

void EngineData::set_hovered_cell(unsigned int cell_index)
{
	bool is_hovered_cell_changed = cell_index != m_hovered_cell;
	m_hovered_cell = cell_index;
	if (is_hovered_cell_changed)
		on_cell_hovered.invoke(m_hovered_cell);
}

std::vector<float> EngineData::get_values_for_cell(unsigned int index) 
{
	std::vector<float> result;
	
	if (m_cell_stats.find(index) != m_cell_stats.end()) {		//if cell exists
		cell_stats stats = m_cell_stats.at(index);

		for (const auto& name : m_selected_frequencies_names)
			result.push_back(stats.freq_map[name]);
	} else
		for (const auto& name : m_selected_frequencies_names)
			result.push_back(0);
	
	return result;
}

glm::vec3 EngineData::get_color_for_selected_cell(unsigned int index, unsigned int num_of_cells)
{
	glm::vec3 color = convert_hsv_to_rgb({ float(index + 1) / (num_of_cells + 1), 1, 1 });
	return color;
}

void EngineData::set_normal_mode_coloring()
{
	m_cell_coloring_function = std::bind(&EngineData::normal_mode_coloring, this, std::placeholders::_1);
	calculate_color();
}

void EngineData::set_limits_mode_coloring()
{
	m_cell_coloring_function = std::bind(&EngineData::limits_mode_coloring, this, std::placeholders::_1);
	calculate_color();
}
