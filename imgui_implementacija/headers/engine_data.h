#pragma once

#include <functional>
#include <memory>
#include <array>

#include "events.h"
#include "data_loading.h"
#include "gradient.h"
#include "cell_value_functors.h"

enum Limits { GLOBAL, LOCAL, USER_DEFINED};

enum CellFunctions { MIN, MAX, AVERAGE, MEDIAN, SPREAD};

struct FrequenzyPairsComparator {
	std::vector<std::string> all_names;
	FrequenzyPairsComparator(const std::vector<std::string>& all_names) : all_names(all_names) {}

	bool operator()(const std::pair<std::string, float>& p1, const std::pair<std::string, float>& p2) const
	{
		return std::find(all_names.begin(), all_names.end(), p1.first) < std::find(all_names.begin(), all_names.end(), p2.first);
	}
};

class EngineData {
private:
	static const std::vector <std::shared_ptr<cell_functors::AbstractCellFunctor>> CELL_FUNCTIONS;

	std::map<unsigned int, cell_stats> m_cell_stats;
	
	std::vector<unsigned int> m_cell_indeces;

	std::vector<glm::vec2> m_limits;

	std::vector<std::string> m_frequenzy_names;
	std::vector<std::string> m_selected_frequencies_names;

	std::vector<unsigned int> m_selected_cells;
	unsigned int m_hovered_cell;

	glm::vec3 m_cached_default_color;
	Gradient m_cached_gradient;

	Limits m_cached_limits_mode;
	CellFunctions m_cached_selected_function;

	FrequenzyPairsComparator m_pairs_comparator;

	void calculate_color();

	void refresh_cached_values();

	void find_local_limits();

	void find_global_limits();

public:
	static const char* FUNCTION_NAMES[5];
	static const char* LIMITS_NAMES[3];

	glm::vec3 default_color;
	Gradient gradient;
	glm::vec2 user_limits;
	Limits limits_mode;
	CellFunctions selected_function;

	Event<const std::map<unsigned int, glm::vec3>&> on_colors_recalculated;

	Event<unsigned int> on_cell_hovered;

	EngineData(const glm::vec3& default_color);

	void load_cell_stats(const char* path);

	void on_cell_vertices_loaded(const char* path);

	void check_for_changes();

	void select_frequency(const std::string& f_name, bool is_selected);

	void clear_selection();

	void clear_hovered_cell();

	void clear_selected_cells();

	void handle_cell_selection();

	void set_hovered_cell(unsigned int cell_index);

	std::vector<std::pair<std::string, float>> get_values_for_cell(unsigned int index);

	bool is_frequency_selected(const std::string& f_name) { return std::find(m_selected_frequencies_names.begin(), m_selected_frequencies_names.end(), f_name) != m_selected_frequencies_names.end(); }

	bool are_stats_loaded() { return m_cell_stats.size() > 0; }

	std::vector<std::string> frequenzy_names() { return m_frequenzy_names; }

	std::vector<std::string> selected_frequencies() { return m_selected_frequencies_names; }

	bool is_valid_cell_hovered() { return m_hovered_cell != 0; }

	std::vector<std::pair<std::string, float>> get_hovered_cell_values() { return get_values_for_cell(m_hovered_cell); }
};