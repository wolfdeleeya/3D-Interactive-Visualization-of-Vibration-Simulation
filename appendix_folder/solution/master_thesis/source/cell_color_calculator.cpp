#include "cell_color_calculator.h"

const std::vector <std::shared_ptr<cell_functors::AbstractCellFunctor>> CellColorCalculator::CELL_FUNCTIONS{
	std::make_shared<cell_functors::MinFunctor>(),
	std::make_shared<cell_functors::MaxFunctor>(),
	std::make_shared<cell_functors::AvgFunctor>(),
	std::make_shared<cell_functors::MedianFunctor>(),
	std::make_shared<cell_functors::SpreadFunctor>(),
};

void CellColorCalculator::set_default_color()
{
	const std::vector<unsigned int>& selected_cells = m_engine_model->selected_cells();
	const std::vector<unsigned int>& cell_indeces = m_engine_model->cell_indeces();

	const auto& selected_cells_begin = selected_cells.begin();
	const auto& selected_cells_end = selected_cells.end();

	for (unsigned int index : cell_indeces) {
		if (std::find(selected_cells_begin, selected_cells_end, index) != selected_cells_end)	//if is selected ignore loop
			continue;

		m_current_color_map[index] = m_engine_model->get_color_val(EngineModel::ColorVariables::DEFAULT_COLOR);
	}
}

void CellColorCalculator::set_normal_mode_colors()
{
	EngineModel::VibrationLimitsVariables current_limit_mode = (EngineModel::VibrationLimitsVariables)m_engine_model->get_uint_val(EngineModel::UnsignedIntVariables::VIBRATION_LIMITS);

	glm::vec2 limits = m_engine_model->get_vibration_limits_val(current_limit_mode);

	const std::vector<unsigned int>& selected_cells = m_engine_model->selected_cells();
	const std::vector<unsigned int>& selected_frequencies_indeces = m_engine_model->selected_frequencies_indeces();
	const std::vector<unsigned int>& cell_indeces = m_engine_model->cell_indeces();

	unsigned int n_selected_frequencies = selected_frequencies_indeces.size();

	const auto& selected_cells_begin = selected_cells.begin();
	const auto& selected_cells_end = selected_cells.end();

	for (unsigned int cell_index : cell_indeces) {
		if (std::find(selected_cells_begin, selected_cells_end, cell_index) != selected_cells_end)	//if is selected ignore loop
			continue;

		if (n_selected_frequencies == 1) {
			auto& cell_stats = m_engine_model->get_stats_for_cell(cell_index);

			float value = cell_stats.freq_data[selected_frequencies_indeces[0]];
			float norm_value = (value - limits.x) / (limits.y - limits.x);

			m_current_color_map[cell_index] = m_engine_model->get_gradient_val(EngineModel::GradientVariables::NORMAL_MODE_GRADIENT).evaluate(norm_value);
		}
		else {
			auto& cell_stats = m_engine_model->get_stats_for_cell(cell_index);

			std::vector<float> values;
			for (unsigned int i = 0; i < n_selected_frequencies; ++i)
				values.push_back(cell_stats.freq_data[selected_frequencies_indeces[i]]);

			float value = (*CELL_FUNCTIONS[m_engine_model->get_uint_val(EngineModel::UnsignedIntVariables::NORMAL_MODE_FUNCTION)])(values);
			float norm_value = (value - limits.x) / (limits.y - limits.x);

			m_current_color_map[cell_index] = m_engine_model->get_gradient_val(EngineModel::GradientVariables::NORMAL_MODE_GRADIENT).evaluate(norm_value);
		}
	}
}

void CellColorCalculator::set_limits_mode_colors()
{
	const std::vector<unsigned int>& selected_cells = m_engine_model->selected_cells();
	const std::vector<unsigned int>& cell_indeces = m_engine_model->cell_indeces();

	const auto& selected_cells_begin = selected_cells.begin();
	const auto& selected_cells_end = selected_cells.end();

	for (unsigned int index : cell_indeces) {
		if (std::find(selected_cells_begin, selected_cells_end, index) != selected_cells_end)	//if is selected ignore continue onto the next iteration
			continue;

		m_current_color_map[index] = calculate_limits_color_for_cell(index);
	}
}

glm::vec3 CellColorCalculator::calculate_limits_color_for_cell(unsigned int cell_index) {
	const std::vector<unsigned int>& selected_frequencies_indeces = m_engine_model->selected_frequencies_indeces();
	const std::vector<unsigned int>& cell_indeces = m_engine_model->cell_indeces();

	unsigned int n_selected_frequencies = selected_frequencies_indeces.size();
	
	unsigned int num_of_valid_frequencies = 0;
	unsigned int num_of_mid = 0;
	unsigned int num_of_bad = 0;

	auto& freq_data = m_engine_model->get_stats_for_cell(cell_index).freq_data;

	for (unsigned int f_index : selected_frequencies_indeces) {
		++num_of_valid_frequencies;

		glm::vec2 l = m_engine_model->get_frequency_limit(f_index);
		float val = freq_data[f_index];

		if (val >= l.x && val <= l.y)
			++num_of_mid;
		else if (val >= l.y)
			++num_of_bad;
	}

	if (num_of_bad > 0) {			//if there are bad frequencies sample from bad gradient
		const Gradient& bad_gradient = m_engine_model->get_gradient_val(EngineModel::GradientVariables::LIMITS_MODE_DANGEROUS_GRADIENT);
		return bad_gradient.evaluate(float(num_of_bad) / num_of_valid_frequencies);
	}
	else if (num_of_mid > 0) {	//if there are no bad frequencies, but mids exist, sample from mid gradient
		const Gradient& mid_gradient = m_engine_model->get_gradient_val(EngineModel::GradientVariables::LIMITS_MODE_RISKY_GRADIENT);
		return mid_gradient.evaluate(float(num_of_mid) / num_of_valid_frequencies);
	}								//if there are valid cells, and no bad or mid frequencies, return good color
	else if (num_of_valid_frequencies > 0) {
		const glm::vec3& good_color = m_engine_model->get_color_val(EngineModel::ColorVariables::GOOD_LIMITS_COLOR);
		return good_color;
	}
}

CellColorCalculator::CellColorCalculator(EngineModel* engine_model): m_engine_model(engine_model)
{
	m_coloring_functions = {
		std::bind(&CellColorCalculator::set_normal_mode_colors, this),
		std::bind(&CellColorCalculator::set_limits_mode_colors, this)
	};

	m_engine_model->on_selected_frequencies_changed.add_member_listener(&CellColorCalculator::calculate_color, this);
	m_engine_model->on_selected_cells_changed.add_member_listener(&CellColorCalculator::calculate_color, this);
	m_engine_model->on_selected_cells_pallete_changed.add_listener([this](unsigned int id) { this->calculate_color(); });

	//calculate default color
	m_engine_model->on_vertex_positions_loaded.add_member_listener(&CellColorCalculator::calculate_color, this);
	m_engine_model->on_cell_vertices_loaded.add_member_listener(&CellColorCalculator::calculate_color, this);

	std::function<void(void)> calculate_color_function = std::bind(&CellColorCalculator::calculate_color, this);

	unsigned int num_of_gradient_variables = (unsigned int) EngineModel::GradientVariables::END;
	
	for (int i = 0; i < num_of_gradient_variables; ++i)
		m_engine_model->add_variable_listener((EngineModel::GradientVariables)i, calculate_color_function);


	unsigned int num_of_color_variables = (unsigned int)EngineModel::ColorVariables::END;

	for (int i = 0; i < num_of_color_variables; ++i)
		m_engine_model->add_variable_listener((EngineModel::ColorVariables)i, calculate_color_function);


	unsigned int num_of_uint_variables = (unsigned int)EngineModel::UnsignedIntVariables::END;

	for (int i = 0; i < num_of_uint_variables; ++i)
		m_engine_model->add_variable_listener((EngineModel::UnsignedIntVariables)i, calculate_color_function);


	unsigned int num_of_limits_variables = (unsigned int)EngineModel::VibrationLimitsVariables::END;

	for (int i = 0; i < num_of_limits_variables; ++i)
		m_engine_model->add_variable_listener((EngineModel::VibrationLimitsVariables)i, calculate_color_function);
}

void CellColorCalculator::calculate_color()
{
	const std::vector<unsigned int>& selected_cells = m_engine_model->selected_cells();

	unsigned int n_selected_frequencies = m_engine_model->num_of_selected_frequencies();
	unsigned int n_selected_cells = selected_cells.size();

	for (unsigned int i = 0; i < n_selected_cells; ++i)
		m_current_color_map[selected_cells[i]] = m_engine_model->get_color_for_selected_cell(i);

	if (n_selected_frequencies == 0)
		set_default_color();
	else {
		unsigned int current_visualization_mode = m_engine_model->get_uint_val(EngineModel::UnsignedIntVariables::VISUALIZATION_MODE);
		m_coloring_functions[current_visualization_mode]();
	}

	on_colors_recalculated.invoke();
}