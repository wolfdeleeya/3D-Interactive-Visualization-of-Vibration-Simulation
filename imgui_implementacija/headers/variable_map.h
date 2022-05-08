#pragma once
#include <map>
#include <functional>

template <typename E, typename V>	//E must be enum, V can be any value
class VariableMap {
private:
	std::map<E, V> m_cached_values;
	std::map<E, V> m_values;

	std::function<bool(const V&, const V&)> m_comparator;

	E m_end_enum;

public:
	VariableMap(std::function<bool(const V&, const V&)> comparator, const E& end_enum, const V& default_value);

	bool check_for_changes();

	V* get(const E& e) { return &m_values.at(e); }
};

template<typename E, typename V>
inline VariableMap<E, V>::VariableMap(std::function<bool(const V&, const V&)> comparator, const E& end_enum, const V& default_value)
{
	m_comparator = comparator;
	m_end_enum = end_enum;
	unsigned int num_of_values = (unsigned int) m_end_enum;

	for (int i = 0; i < num_of_values; ++i) {
		m_values.insert({ (E) i, default_value });
		m_cached_values.insert({ (E)i, default_value });
	}
}

template<typename E, typename V>
inline bool VariableMap<E, V>::check_for_changes()
{
	bool are_changes_pending = false;
	unsigned int num_of_values = (unsigned int)m_end_enum;

	for (int i = 0; i < num_of_values; ++i) {
		if (m_comparator(!m_cached_values[(E)i], m_values[(E)i])) {
			are_changes_pending = true;
			m_cached_values[(E)i] = m_values[(E)i];
		}
	}

	return are_changes_pending;
}