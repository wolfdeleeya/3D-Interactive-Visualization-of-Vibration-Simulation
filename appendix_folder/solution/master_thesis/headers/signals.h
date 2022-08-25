#pragma once
#include <vector>
#include <functional>


class Signal {
private:
	std::vector<std::function<void()>> m_listeners;
public:
	void add_listener(std::function<void()> listener) { m_listeners.push_back(listener); }

	template <typename L>
	void add_member_listener(void(L::* m)(), L* l) { add_listener(std::bind(m, l)); }

	void invoke() {
		for (auto& l : m_listeners)
			l();
	}
};
