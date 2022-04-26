#include "engine_data.h"
#include "glm_vec_helper.h"

const std::vector <std::shared_ptr<cell_functors::AbstractCellFunctor>> EngineData::CELL_FUNCTIONS{
	std::make_shared<cell_functors::MinFunctor>(),
	std::make_shared<cell_functors::MaxFunctor>(),
	std::make_shared<cell_functors::AvgFunctor>(),
	std::make_shared<cell_functors::MedianFunctor>(),
	std::make_shared<cell_functors::SpreadFunctor>(),
};

void EngineData::calculate_color()
{
	std::map<unsigned int, glm::vec3> color_map;

	unsigned int n_selected_frequencies = m_selected_frequencies_names.size();

	glm::vec2& limits = m_limits[m_limits_mode];
	if (n_selected_frequencies == 0) {
		for (unsigned int& index : m_cell_indeces)
			color_map[index] = default_color;
	}
	else if (n_selected_frequencies == 1) {
		for (auto& pair : m_cell_stats) {
			float value = pair.second.freq_map[m_selected_frequencies_names[0]];
			float norm_value = (value - limits.x) / (limits.y - limits.x);
			color_map[pair.first] = gradient.evaluate(norm_value);
		}
	}
	else {
		for (auto& pair : m_cell_stats) {
			std::vector<float> values;
			for (int i = 0; i < n_selected_frequencies; ++i)
				values.push_back(pair.second.freq_map[m_selected_frequencies_names[i]]);

			float value = (*CELL_FUNCTIONS[m_selected_function])(values);
			float norm_value = (value - limits.x) / (limits.y - limits.x);
			color_map[pair.first] = gradient.evaluate(norm_value);
		}
	}

	on_colors_recalculated.invoke(color_map);
}

void EngineData::refresh_cached_values()
{
	m_cached_default_color = default_color;
	m_cached_gradient = gradient;
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
			float value = map.at(name);

			local_limits.x = local_limits.x > value ? value : local_limits.x;		//set min limit
			local_limits.y = local_limits.y < value ? value : local_limits.y;		//set max limit
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

EngineData::EngineData(const glm::vec3& color)
{
	m_limits_mode = GLOBAL;
	m_selected_function = AVERAGE;

	default_color = color;
	gradient = m_cached_gradient;
	m_limits = { {0, 0}, {0, 0}, {0, 0} };
	refresh_cached_values();
}

void EngineData::load_cell_stats(const char* path)
{
	m_cell_stats = loader::load_cell_stats(path, m_frequenzy_names);

	m_cell_indeces.clear();

	for (const auto& pair : m_cell_stats)
		m_cell_indeces.push_back(pair.first);

	find_global_limits();

	calculate_color();
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

	if (!are_equal(default_color, m_cached_default_color))
		are_changes_pending = true;
	else if (gradient != m_cached_gradient)
		are_changes_pending = true;
	else if (!are_equal(m_limits[USER_DEFINED], user_limits))
		are_changes_pending = true;

	if (are_changes_pending)
	{
		calculate_color();
		refresh_cached_values();
	}
}

void EngineData::select_frequency(const std::string& f_name, bool value)
{
	if (value)				//if true add name
		m_selected_frequencies_names.push_back(f_name);
	else {					//if false delete name
		const auto& last = std::remove(m_selected_frequencies_names.begin(), m_selected_frequencies_names.end(), f_name);
		m_selected_frequencies_names.erase(last);
	}

	find_local_limits();

	calculate_color();
}