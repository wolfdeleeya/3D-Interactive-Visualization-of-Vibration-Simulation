#include <iostream>
#include <algorithm>

#include "engine_data.h"
#include "glm_vec_helper.h"
#include "color_helper.h"
#include "app.h"

const std::vector <std::shared_ptr<cell_functors::AbstractCellFunctor>> EngineData::CELL_FUNCTIONS{
	std::make_shared<cell_functors::MinFunctor>(),
	std::make_shared<cell_functors::MaxFunctor>(),
	std::make_shared<cell_functors::AvgFunctor>(),
	std::make_shared<cell_functors::MedianFunctor>(),
	std::make_shared<cell_functors::SpreadFunctor>(),
};

const glm::vec3 EngineData::START_GOOD_LIMITS_COLOR = { 0.560, 0.984, 0.262 };
const glm::vec3 EngineData::START_RISKY_LIMITS_COLOR1 = { 0.956, 0.956, 0.105 };
const glm::vec3 EngineData::START_RISKY_LIMITS_COLOR2 = { 0.807, 0.478, 0.145 };
const glm::vec3 EngineData::START_DANGEROUS_LIMITS_COLOR1 = { 0.941, 0.305, 0.305 };
const glm::vec3 EngineData::START_DANGEROUS_LIMITS_COLOR2 = { 0.584, 0.003, 0.003 };

const char* EngineData::FUNCTION_NAMES[5] {"MIN", "MAX", "AVERAGE", "MEDIAN", "SPREAD"};
const char* EngineData::LIMITS_NAMES[3] {"GLOBAL", "LOCAL", "USER DEFINED"};

const char* EngineData::DEFAULT_PALLETE_PATH = "./Palletes/default_pallete.txt";

void EngineData::find_local_limits()
{
	if (m_selected_frequencies_indeces.size() <= 0)
		return;
	
	glm::vec2 local_limits(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());

	for (const auto& pair : m_cell_stats) {
		const std::vector<float>& freq_data = pair.second.freq_data;

		for (unsigned int index : m_selected_frequencies_indeces) {
			float value = freq_data[index];

			local_limits.x = local_limits.x > value ? value : local_limits.x;		//set min limit
			local_limits.y = local_limits.y < value ? value : local_limits.y;		//set max limit
		}
	}

	m_normal_mode_limits_variables.set(VibrationLimitsVariables::LOCAL, local_limits);
}

void EngineData::find_global_limits()
{
	glm::vec2 global_limits(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());

	for (const auto& pair : m_cell_stats) {
		const std::vector<float>& freq_data = pair.second.freq_data;

		for (unsigned int index = 0; index < m_frequenzy_names.size(); ++index) {
			float value = freq_data[index];

			global_limits.x = global_limits.x > value ? value : global_limits.x;		//set min limit
			global_limits.y = global_limits.y < value ? value : global_limits.y;		//set max limit
		}
	}

	m_normal_mode_limits_variables.set(VibrationLimitsVariables::GLOBAL, global_limits);
}

glm::vec3 EngineData::calculate_limits_color_for_cell(unsigned int cell_index)
{
	unsigned int num_of_valid_frequencies = 0;
	unsigned int num_of_mid = 0;
	unsigned int num_of_bad = 0;

	auto& freq_data = m_cell_stats.at(cell_index).freq_data;

	for (unsigned int f_index : m_selected_frequencies_indeces) {
			++num_of_valid_frequencies;

			glm::vec2 l =  m_frequenzy_limits.at(m_frequenzy_names[f_index]);
			float val = freq_data[f_index];

			if (val >= l.x && val <= l.y)
				++num_of_mid;
			else if (val >= l.y)
				++num_of_bad;
	}

	if (num_of_bad > 0) {			//if there are bad frequencies sample from bad gradient
		Gradient& bad_gradient = *get_gradient(GradientVariables::LIMITS_MODE_DANGEROUS_GRADIENT);
		return bad_gradient.evaluate(float(num_of_bad) / num_of_valid_frequencies);
	} else if (num_of_mid > 0) {	//if there are no bad frequencies, but mids exist, sample from mid gradient
		Gradient& mid_gradient = *get_gradient(GradientVariables::LIMITS_MODE_RISKY_GRADIENT);
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

void EngineData::normal_mode_coloring()
{
	unsigned int n_selected_frequencies = num_of_selected_frequencies();

	VibrationLimitsVariables current_limit_mode = (VibrationLimitsVariables)*get_uint(UnsignedIntVariables::VIBRATION_LIMITS);

	glm::vec2& limits = *get_vibration_limits(current_limit_mode);

	const auto& selected_cells_begin = m_selected_cells.begin();
	const auto& selected_cells_end = m_selected_cells.end();

	for (unsigned int index : m_cell_indeces) {
		if (std::find(selected_cells_begin, selected_cells_end, index) != selected_cells_end)	//if is selected ignore loop
			continue;

		if (n_selected_frequencies == 1) {
			auto& cell_stats = m_cell_stats[index];

			float value = cell_stats.freq_data[m_selected_frequencies_indeces[0]];
			float norm_value = (value - limits.x) / (limits.y - limits.x);

			m_current_color_map[index] = get_gradient(GradientVariables::NORMAL_MODE_GRADIENT)->evaluate(norm_value);
		}
		else {
			auto& cell_stats = m_cell_stats[index];

			std::vector<float> values;
			for (unsigned int i = 0; i < n_selected_frequencies; ++i)
				values.push_back(cell_stats.freq_data[m_selected_frequencies_indeces[i]]);

			float value = (*CELL_FUNCTIONS[*get_uint(UnsignedIntVariables::NORMAL_MODE_FUNCTION)])(values);
			float norm_value = (value - limits.x) / (limits.y - limits.x);

			m_current_color_map[index] = get_gradient(GradientVariables::NORMAL_MODE_GRADIENT)->evaluate(norm_value);
		}
	}
}

void EngineData::default_coloring()
{
	const auto& selected_cells_begin = m_selected_cells.begin();
	const auto& selected_cells_end = m_selected_cells.end();

	for (unsigned int index : m_cell_indeces) {
		if (std::find(selected_cells_begin, selected_cells_end, index) != selected_cells_end)	//if is selected ignore loop
			continue;

		m_current_color_map[index] = *get_color(ColorVariables::DEFAULT_COLOR);
	}
}

void EngineData::limits_mode_coloring()
{
	const auto& selected_cells_begin = m_selected_cells.begin();
	const auto& selected_cells_end = m_selected_cells.end();

	for (unsigned int index : m_cell_indeces) {
		if (std::find(selected_cells_begin, selected_cells_end, index) != selected_cells_end)	//if is selected ignore loop
			continue;

		m_current_color_map[index] = calculate_limits_color_for_cell(index);
	}
}

void EngineData::refresh_selected_frequencies_names()
{
	m_selected_frequencies_names.clear();
	for (unsigned int index : m_selected_frequencies_indeces)
		m_selected_frequencies_names.push_back(m_frequenzy_names[index]);
}

void EngineData::add_selected_cell(unsigned int cell_index)
{
	//if the number of selected cells is greater or equal replace the last one with the new one
	// -> it will be colored as the last one because colors save only "local" cell indeces
	if (m_selected_cells.size() >= current_pallete().second.size()) {
		m_selected_cells[m_selected_cells.size() - 1] = cell_index;
	}
	//else add new index and find the first color that is not taken and assign "local" index of new cell to that color
	else {
		m_selected_cells.push_back(cell_index);
		
		int color_index = 0;
		
		while (m_selected_cells_color_indeces[color_index] >= 0)
			++color_index;

		m_selected_cells_color_indeces[color_index] = m_selected_cells.size() - 1;
	}

}

void EngineData::remove_selected_cell_at(unsigned int index)
{
	//erase cell at given "local" index from selected cells list
	m_selected_cells.erase(m_selected_cells.begin() + index);
	
	//find color with given "local" index and set it to -1, or find greater indeces and decrease them by 1
	for (int i = 0; i < m_selected_cells_color_indeces.size(); ++i) {
		if (m_selected_cells_color_indeces[i] == index)				//it the color's index is equal to given index, set "local" index of the found color to -1 (empty)
			m_selected_cells_color_indeces[i] = -1;
		//else if the color's local index is greater decrease it so that the "local" indeces don't go out of range
		else if (m_selected_cells_color_indeces[i] > (int)index)	//we need to cast index to int or the comparison will be skewed
			--m_selected_cells_color_indeces[i];
	}
}

void EngineData::set_selected_cells_color_pallete(unsigned int pallete_index)
{
	m_current_selected_cell_pallet = pallete_index;
	
	unsigned int num_of_available_colors = m_selected_cells_palletes[m_current_selected_cell_pallet].second.size();
	unsigned int num_of_selected_cells = m_selected_cells.size();

	//if no cell is selected, just resize color_indeces vector and set each color to -1 (empty)
	if (num_of_selected_cells == 0) {								
		m_selected_cells_color_indeces = std::vector<int>(num_of_available_colors, -1);
	}
	else {
		unsigned int num_of_colors_in_last_pallete = m_selected_cells_color_indeces.size();

		//if number of colors is greater than number of colors in the last pallete,
		//append color_indeces with -1 up to the size difference between current color_indeces and new number of colors
		if (num_of_available_colors > num_of_colors_in_last_pallete) {
			unsigned int colors_to_add = num_of_available_colors - num_of_colors_in_last_pallete;
			
			while (colors_to_add > 0) {
				m_selected_cells_color_indeces.push_back(-1);
				--colors_to_add;
			}
		}
		//if number of colors is lesser than number of selected cells
		else if (num_of_available_colors < num_of_selected_cells) {
			unsigned int index = 0;
			unsigned int num_of_empty_colors_to_remove = num_of_colors_in_last_pallete - num_of_selected_cells;

			while (index < m_selected_cells_color_indeces.size()) {
				//if color's index is greater than or equal to the number of available colors remove the selected cell 
				if (m_selected_cells_color_indeces[index] >= (int) num_of_available_colors) {
					remove_selected_cell_at(m_selected_cells_color_indeces[index]);
					m_selected_cells_color_indeces.erase(m_selected_cells_color_indeces.begin() + index);
				}
				else if (m_selected_cells_color_indeces[index] == -1 && num_of_empty_colors_to_remove > 0) {
					m_selected_cells_color_indeces.erase(m_selected_cells_color_indeces.begin() + index);
					--num_of_empty_colors_to_remove;
				}
				//else, increase index by one and check the next color
				else
					++index;
			}
		}
		//if number of colors is greater than number of selected cells, but lesser than number of colors in pallete,
		//remove -1's (empty colors) up to the size difference
		else if (num_of_available_colors < num_of_colors_in_last_pallete) {
			unsigned int num_of_colors_to_remove = num_of_colors_in_last_pallete - num_of_available_colors;
			unsigned int index = 0;

			while (num_of_colors_to_remove > 0) {
				//if color is "empty" remove it and decrease num_of_colors_to_remove
				if (m_selected_cells_color_indeces[index] == -1) {
					m_selected_cells_color_indeces.erase(m_selected_cells_color_indeces.begin() + index);
					--num_of_colors_to_remove;
				}
				//else if it's non-empty color, increase index by one and check the next color
				else
					++index;
			}
		}
	}

	calculate_color();
	on_selected_cells_pallete_changed.invoke(m_current_selected_cell_pallet);
}

EngineData::EngineData(const glm::vec3& color) : m_frq_comparator({}),
	m_gradient_variables([](const Gradient& g1, const Gradient& g2) {return g1 == g2; }, GradientVariables::END, Gradient{ glm::vec3(1), glm::vec3(0) }, std::bind(&EngineData::calculate_color, this)),
	m_color_variables([](const glm::vec3& c1, const glm::vec3& c2) { return are_equal(c1, c2); }, ColorVariables::END, glm::vec3(0), std::bind(&EngineData::calculate_color, this)),
	m_uint_variables([](unsigned int ui1, unsigned int ui2) {return ui1 == ui2; }, UnsignedIntVariables::END, 0, std::bind(&EngineData::calculate_color, this)),
	m_normal_mode_limits_variables([](const glm::vec2& v1, const glm::vec2& v2) { return are_equal(v1, v2); },VibrationLimitsVariables::END, glm::vec2(0), std::bind(&EngineData::calculate_color, this))
{
	set_uint(UnsignedIntVariables::VIBRATION_LIMITS, (unsigned int)VibrationLimitsVariables::LOCAL);
	set_uint(UnsignedIntVariables::NORMAL_MODE_FUNCTION, (unsigned int)CellFunctions::AVERAGE);

	set_color(ColorVariables::DEFAULT_COLOR, color);

	set_color(ColorVariables::GOOD_LIMITS_COLOR, START_GOOD_LIMITS_COLOR);

	set_gradient(GradientVariables::LIMITS_MODE_RISKY_GRADIENT, Gradient(START_RISKY_LIMITS_COLOR1, START_RISKY_LIMITS_COLOR2));
	set_gradient(GradientVariables::LIMITS_MODE_DANGEROUS_GRADIENT, Gradient(START_DANGEROUS_LIMITS_COLOR1, START_DANGEROUS_LIMITS_COLOR2));

	set_normal_mode_coloring();

	if (!load_selected_cells_color_pallete(DEFAULT_PALLETE_PATH))		//load default color pallete 
	{
		std::cerr << "DEFAULT SELECTED CELLS PALLETE MISSING!\n";
		exit(-1);
	}
}

void EngineData::calculate_color()
{
	unsigned int n_selected_frequencies = m_selected_frequencies_indeces.size();

	unsigned int n_selected_cells = m_selected_cells.size();

	for (unsigned int i = 0; i < n_selected_cells; ++i)
		m_current_color_map[m_selected_cells[i]] = get_color_for_selected_cell(i);

	if (n_selected_frequencies == 0)
		default_coloring();
	else
		m_cell_coloring_function();

	on_colors_recalculated.invoke();
}

void EngineData::load_cell_stats(const char* path)
{
	m_cell_stats = data::load_cell_stats(path, m_frequenzy_names);

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
	m_frequenzy_limits = data::load_frequenzy_limits(path, m_frequencies_with_limits);

	on_frequency_limits_loaded.invoke();
}

void EngineData::load_cell_vertices(const char* path)		//used to set default color when stats are not yet loaded
{
	if (m_cell_stats.size() != 0)
		return;

	const auto& cell_vertices = data::load_cells(path);
	
	m_cell_indeces.clear();
	
	for (const auto& pair : cell_vertices)
		m_cell_indeces.push_back(pair.first);

	calculate_color();
}

void EngineData::check_for_changes()
{
	static bool are_changes_pending = false;

	are_changes_pending |= m_gradient_variables.check_for_changes();
	are_changes_pending |= m_color_variables.check_for_changes();
	are_changes_pending |= m_uint_variables.check_for_changes();
	are_changes_pending |= m_normal_mode_limits_variables.check_for_changes();
}

void EngineData::select_frequency(const std::string& f_name, bool is_selected)
{
	unsigned int f_index = get_index_from_frequency_name(f_name);
	if (is_selected) {
		//if true add index and sort
		m_selected_frequencies_indeces.push_back(f_index);
		std::sort(m_selected_frequencies_indeces.begin(), m_selected_frequencies_indeces.end());
	}
	else {					//if false delete index
		const auto& last = std::remove(m_selected_frequencies_indeces.begin(), m_selected_frequencies_indeces.end(), f_index);
		m_selected_frequencies_indeces.erase(last);
	}

	find_local_limits();

	calculate_color();

	refresh_selected_frequencies_names();

	on_selected_frequencies_changed.invoke();
}

void EngineData::clear_frequency_selection()
{
	m_selected_frequencies_indeces.clear();
	on_selected_frequencies_changed.invoke();

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
		on_selected_cells_changed.invoke();

		calculate_color();
	}
}

void EngineData::handle_mouse_click()
{
	if (!is_valid_cell_hovered())
		return;

	const auto& hovered_it = std::find(m_selected_cells.begin(), m_selected_cells.end(), m_hovered_cell);
	
	if (hovered_it != m_selected_cells.end()) {
		unsigned int index = hovered_it - m_selected_cells.begin();
		remove_selected_cell_at(index);
	}
	else
		add_selected_cell(m_hovered_cell);

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

std::vector<float> EngineData::get_values_for_cell(unsigned int index) const
{
	std::vector<float> result;
	
	if (does_cell_exist(index)) {		//if cell exists
		cell_stats stats = m_cell_stats.at(index);

		for (const auto& f_index : m_selected_frequencies_indeces)
			result.push_back(stats.freq_data[f_index]);
	} else
		for (const auto& f_index : m_selected_frequencies_indeces)
			result.push_back(0);
	
	return result;
}

glm::vec3 EngineData::get_color_for_selected_cell(unsigned int local_index) const
{
	unsigned int color_index = 0;

	while (m_selected_cells_color_indeces[color_index] != local_index) ++color_index;

	glm::vec3 color = current_pallete().second[color_index];
	return color;
}

void EngineData::set_normal_mode_coloring()
{
	m_cell_coloring_function = std::bind(&EngineData::normal_mode_coloring, this);
	calculate_color();
}

void EngineData::set_limits_mode_coloring()
{
	m_cell_coloring_function = std::bind(&EngineData::limits_mode_coloring, this);
	calculate_color();
}

bool EngineData::load_selected_cells_color_pallete(const char* path)
{
	std::vector<data::pallete> loaded_palletes = data::load_selected_cells_pallete(path);

	if (loaded_palletes.size() > 0) {		//if palletes are loaded, save them, invoke the signal and return true
		m_selected_cells_palletes = loaded_palletes;
		
		set_selected_cells_color_pallete(0);

		on_selected_cells_palletes_loaded.invoke();
		
		return true;
	}
	else                                    // else if palletes are empty, return false	
		return false;
}

void EngineData::set_next_selected_cells_pallete()
{
	unsigned int index_to_set = (m_current_selected_cell_pallet + 1) % m_selected_cells_palletes.size();
	set_selected_cells_color_pallete(index_to_set);
}

void EngineData::on_limits_mode_toggled(bool is_active)
{
	clear_frequency_selection();
	is_active ? set_limits_mode_coloring() : set_normal_mode_coloring();
}

glm::vec2 EngineData::get_current_normal_mode_limits()
{
	unsigned int current_limits_index = *get_uint(UnsignedIntVariables::VIBRATION_LIMITS);
	return *get_vibration_limits((VibrationLimitsVariables)current_limits_index);
}

unsigned int EngineData::get_index_from_frequency_name(const std::string& name) const
{
	unsigned int index = 0;
	for (const auto& f_name : m_frequenzy_names) {
		if (f_name == name)
			return index;
		++index;
	}

	if (index >= m_frequenzy_names.size())
		throw std::out_of_range("FREQUENCY INDEX OUT OF RANGE!");
}

bool EngineData::is_frequency_selected(const std::string& f_name) const
{
	unsigned int f_index = get_index_from_frequency_name(f_name);
	return is_frequency_selected(f_index);
}
