#pragma once
#include <vector>
#include <functional>

template <typename T>
class Event {
private:
	std::vector<std::function<void(T)>> m_listeners;
public:
	void add_listener(std::function<void(T)> listener) { m_listeners.push_back(listener); }
	
	template <typename L>
	void add_member_listener(void(L::*m)(T), L* l) { add_listener(std::bind(m, l, std::placeholders::_1)); }
	
	void invoke(T var);
};

template<typename T>
inline void Event<T>::invoke(T var) {
	for (auto& l : m_listeners)
		l(var);
}