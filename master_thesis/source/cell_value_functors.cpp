#include <limits>
#include <algorithm>

#include "cell_value_functors.h"

float cell_functors::MinFunctor::operator()(const std::vector<float>& values) const
{
	float min = std::numeric_limits<float>::max();

	for (const float& val : values)
		min = val < min ? val : min;

	return min;
}

float cell_functors::MaxFunctor::operator()(const std::vector<float>& values) const
{
	float max = std::numeric_limits<float>::min();

	for (const float& val : values)
		max = val > max ? val : max;

	return max;
}

float cell_functors::AvgFunctor::operator()(const std::vector<float>& values) const
{
	float result = 0;

	for (const float& val : values)
		result += val;

	return result / values.size();
}

float cell_functors::MedianFunctor::operator()(const std::vector<float>& values) const
{
	std::vector<float> temp_vec(values);
	std::sort(temp_vec.begin(), temp_vec.end());
	bool is_even = temp_vec.size() % 2 == 0;

	if (is_even) {
		unsigned int median_index1 = temp_vec.size() / 2, median_index2 = median_index1 - 1;
		return (temp_vec[median_index1] + temp_vec[median_index2]) / 2;
	}
	else {
		unsigned int median_index = temp_vec.size() / 2;
		return temp_vec[median_index];
	}
}

float cell_functors::SpreadFunctor::operator()(const std::vector<float>& values) const
{
	float min = std::numeric_limits<float>::max();
	float max = std::numeric_limits<float>::min();

	for (const float& val : values) {
		min = val < min ? val : min;
		max = val > max ? val : max;
	}

	return max - min;
}
