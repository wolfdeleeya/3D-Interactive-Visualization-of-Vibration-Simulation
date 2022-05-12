#pragma once

#include "graph_data.h"
#include "events.h"


class GraphModel {
private:

public:
	Event<GraphData> on_graph_data_changed;
};