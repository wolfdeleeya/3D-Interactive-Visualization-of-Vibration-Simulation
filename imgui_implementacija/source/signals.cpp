#include "signals.h"

void Signal::invoke()
{
	for (auto& l : m_listeners)
		l();
}
