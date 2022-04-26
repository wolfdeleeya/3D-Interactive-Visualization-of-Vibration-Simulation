#pragma once

#include <vector>

namespace cell_functors {
	struct AbstractCellFunctor { 
		virtual float operator()(const std::vector<float>& values) const = 0; 
	};

	struct MinFunctor : public AbstractCellFunctor {
		virtual float operator()(const std::vector<float>& values) const override;
	};

	struct MaxFunctor : public AbstractCellFunctor {
		virtual float operator()(const std::vector<float>& values) const override;
	};

	struct AvgFunctor : public AbstractCellFunctor {
		virtual float operator()(const std::vector<float>& values) const override;
	};

	struct MedianFunctor : public AbstractCellFunctor {
		virtual float operator()(const std::vector<float>& values) const override;
	};

	struct SpreadFunctor : public AbstractCellFunctor {
		virtual float operator()(const std::vector<float>& values) const override;
	};
}