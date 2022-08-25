#pragma once
#include <map>
#include <functional>

#include "engine_model.h"
#include "cell_value_functors.h"
#include "signals.h"

class CellColorCalculator {
private:
	static const std::vector <std::shared_ptr<cell_functors::AbstractCellFunctor>> CELL_FUNCTIONS;

	EngineModel* m_engine_model;

	std::map<unsigned int, glm::vec3> m_current_color_map;

	std::vector<std::function<void(void)>> m_coloring_functions;

	void set_default_color();

	void set_normal_mode_colors();

	void set_limits_mode_colors();

	glm::vec3 calculate_limits_color_for_cell(unsigned int cell_index);

public:
	CellColorCalculator(EngineModel* engine_model);

	Signal on_colors_recalculated;

	void calculate_color();

	const std::map<unsigned int, glm::vec3>& current_color_map() { return m_current_color_map; }
};