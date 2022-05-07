#pragma once
#include <map>

template <typename E, typename V>	//E must be enum that is Ended with END, V can be any value
class VariableMap {
private:
	std::map<E, V> m_cached_values;
	std::map<E, V> values;

public:
	VariableMap(const V& default_value);

	bool check_for_changes();
};

template<typename E, typename V>
inline VariableMap<E, V>::VariableMap(const V& default_value)
{
	unsigned int num_of_values = (unsigned int) E::END;

	for (int i = 0; i < num_of_values; ++i) {
		m_values.insert({ (E)i, default_value });
		m_cached_values.insert({ (E)i, default_value });
	}
}

template<typename E, typename V>
inline bool VariableMap<E, V>::check_for_changes()
{
	bool are_changes_pending = false;



	return are_changes_pending;
}
