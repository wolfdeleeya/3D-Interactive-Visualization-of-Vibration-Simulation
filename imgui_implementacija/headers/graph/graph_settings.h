#pragma once

struct BaseGraphSettings {
	virtual	void draw() {}
};

struct GroupBarGraphSettings : public BaseGraphSettings {
	float bar_width;

	virtual void draw() override;
};