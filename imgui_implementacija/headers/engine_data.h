#pragma once

#include <functional>
#include <memory>
#include <array>

#include "graph_data.h"
#include "events.h"
#include "data_loading.h"
#include "gradient.h"
#include "cell_value_functors.h"
#include "variable_map.h"
#include "signals.h"

struct FrequenzyComparator {
	std::vector<std::string> all_names;
	FrequenzyComparator(const std::vector<std::string>& all_names) : all_names(all_names) {}

	bool operator()(const std::string& s1, const std::string& s2) const
	{
		return std::find(all_names.begin(), all_names.end(), s1) < std::find(all_names.begin(), all_names.end(), s2);
	}
};

class EngineData {
public:
	enum class CellFunctions { MIN, MAX, AVERAGE, MEDIAN, SPREAD };

	enum class GradientVariables { NORMAL_MODE_GRADIENT, LIMITS_MODE_MID_GRADIENT, LIMITS_MODE_BAD_GRADIENT, END };

	enum class ColorVariables { DEFAULT_COLOR, GOOD_LIMITS_COLOR, END };

	enum class UnsignedIntVariables { NORMAL_MODE_LIMITS, NORMAL_MODE_FUNCTION, END };

	enum class NormalModeLimitsVariables { GLOBAL, LOCAL, USER_DEF, END };
private:
	static const std::vector <std::shared_ptr<cell_functors::AbstractCellFunctor>> CELL_FUNCTIONS;

	std::map<unsigned int, cell_stats> m_cell_stats;
	
	std::vector<unsigned int> m_cell_indeces;

	std::vector<std::string> m_frequenzy_names;
	std::vector<std::string> m_selected_frequencies_names;
	std::vector<std::string> m_frequencies_with_limits;

	std::map<std::string, glm::vec2> m_frequenzy_limits;
	std::vector<unsigned int> m_selected_cells;
	std::vector<data::pallete> m_selected_cells_palletes;

	//int because negative values will be used for indicating that color is not used

	std::vector<int> m_selected_cells_color_indeces;
	
	unsigned int m_hovered_cell;
	unsigned int m_current_selected_cell_pallet;

	VariableMap<GradientVariables, Gradient> m_gradient_variables;
	VariableMap<ColorVariables, glm::vec3> m_color_variables;
	VariableMap<UnsignedIntVariables, unsigned int> m_uint_variables;
	VariableMap<NormalModeLimitsVariables, glm::vec2> m_normal_mode_limits_variables;

	FrequenzyComparator m_frq_comparator;

	std::function<void(std::map<unsigned int, glm::vec3>&)> m_cell_coloring_function;

	void find_local_limits();

	void find_global_limits();

	glm::vec3 calculate_limits_color_for_cell(unsigned int cell_index);

	void normal_mode_coloring(std::map<unsigned int, glm::vec3>& color_map);

	void default_coloring(std::map<unsigned int, glm::vec3>& color_map);

	void limits_mode_coloring(std::map<unsigned int, glm::vec3>& color_map);

	void add_selected_cell(unsigned int cell_index);

	void remove_selected_cell_at(unsigned int index);

	void set_selected_cells_color_pallete(unsigned int pallete_index);
	
	data::pallete& current_pallete() { return m_selected_cells_palletes[m_current_selected_cell_pallet]; }

public:
	static const char* FUNCTION_NAMES[5];
	static const char* LIMITS_NAMES[3];

	static const char* DEFAULT_PALLETE_PATH;

	Signal on_selected_frequencies_changed;
	Signal on_selected_cells_changed;
	Signal on_selected_cells_palletes_loaded;

	Event<const std::map<unsigned int, glm::vec3>&> on_colors_recalculated;

	Event<unsigned int> on_cell_hovered;

	Signal on_cell_stats_loaded,
		on_frequency_limits_loaded;

	EngineData(const glm::vec3& default_color);

	void calculate_color();

	void load_cell_stats(const char* path);

	void load_frequency_limits(const char* path);

	void load_cell_vertices(const char* path);

	void check_for_changes();

	void select_frequency(const std::string& f_name, bool is_selected);

	void clear_frequency_selection();

	void clear_hovered_cell();

	void clear_selected_cells();

	void handle_mouse_click();

	void set_hovered_cell(unsigned int cell_index);

	std::vector<float> get_values_for_cell(unsigned int index);
	
	glm::vec3 get_color_for_selected_cell(unsigned int index);

	void set_normal_mode_coloring();

	void set_limits_mode_coloring();

	bool load_selected_cells_color_pallete(const char* path);

	void set_next_selected_cells_pallete();

	void handle_mouse_dragged(const glm::ivec2& delta) { clear_hovered_cell(); }

	void on_scene_view_focus_changed(bool is_in_focus) { if (!is_in_focus) clear_hovered_cell(); }

	void on_limits_mode_toggled(bool is_active) { is_active ? set_limits_mode_coloring() : set_normal_mode_coloring(); }

	unsigned int hovered_cell() { return m_hovered_cell; }

	unsigned int num_of_selected_frequencies() { return m_selected_frequencies_names.size(); }

	unsigned int num_of_selected_cells() { return m_selected_cells.size(); }

	bool is_frequency_selected(const std::string& f_name) { return std::find(m_selected_frequencies_names.begin(), m_selected_frequencies_names.end(), f_name) != m_selected_frequencies_names.end(); }

	bool are_stats_loaded() { return m_cell_stats.size() > 0; }

	bool are_frequenzy_limits_loaded() { return m_frequenzy_limits.size() > 0; }

	std::vector<data::pallete> selected_cells_palletes() { return m_selected_cells_palletes; }

	std::vector<std::string> frequenzy_names() { return m_frequenzy_names; }

	std::vector<std::string> selected_frequencies() { return m_selected_frequencies_names; }

	std::vector<std::string> frequencies_with_limits() { return m_frequencies_with_limits; }

	std::vector<unsigned int> selected_cells() { return m_selected_cells; }

	bool is_valid_cell_hovered() { return m_hovered_cell != 0; }

	std::vector<float> get_hovered_cell_values() { return get_values_for_cell(m_hovered_cell); }

	//variable getters, pointer is returned, so they can be used with imgui
	Gradient* get_gradient(GradientVariables e) { return m_gradient_variables.get(e); }

	//variable getters, pointer is returned, so they can be used with imgui
	glm::vec3* get_color(ColorVariables e) { return m_color_variables.get(e); }

	//variable getters, pointer is returned, so they can be used with imgui
	unsigned int* get_uint(UnsignedIntVariables e) { return m_uint_variables.get(e); }

	//variable getters, pointer is returned, so they can be used with imgui
	glm::vec2* get_normal_mode_limits(NormalModeLimitsVariables e) { return m_normal_mode_limits_variables.get(e); }

	//variable setters, though the same functionality can be achieved with "getters", these setters are much more readable
	void set_gradient(GradientVariables e, const Gradient& g) { m_gradient_variables.set(e, g); }

	//variable setters, though the same functionality can be achieved with "getters", these setters are much more readable
	void set_color(ColorVariables e, const glm::vec3& c) { m_color_variables.set(e, c); }

	//variable setters, though the same functionality can be achieved with "getters", these setters are much more readable
	void set_uint(UnsignedIntVariables e, unsigned int i) { m_uint_variables.set(e, i); }

	//variable setters, though the same functionality can be achieved with "getters", these setters are much more readable
	void set_normal_mode_limits(NormalModeLimitsVariables e, const glm::vec2& l) { m_normal_mode_limits_variables.set(e, l); }
};