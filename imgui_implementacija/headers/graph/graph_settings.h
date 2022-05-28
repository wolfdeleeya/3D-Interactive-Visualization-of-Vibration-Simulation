#pragma once
#include "engine_data.h"
#include "events.h"

class BaseGraphSettings {
public:
	virtual	void draw() {}
};

class BarGraphSettings : public BaseGraphSettings {
public:
	float bar_width;

	BarGraphSettings(float init_bar_width): bar_width(init_bar_width) {}

	virtual void draw() override;
};

class SubplotsComparisonSettings : public BaseGraphSettings {
private:
	const EngineData* m_engine_data;
public:
	int num_of_columns;
	
	SubplotsComparisonSettings(const EngineData* engine_data, int init_num_of_columns): m_engine_data(engine_data), num_of_columns(init_num_of_columns) {}

	virtual void draw() override;
};

class RelativeComparisonSettings : public BaseGraphSettings {
private:
	const EngineData* m_engine_data;
public:
	Event<unsigned int> on_referent_cell_changed;

	RelativeComparisonSettings(const EngineData* engine_data):m_engine_data(engine_data) {}

	virtual void draw() override;
};