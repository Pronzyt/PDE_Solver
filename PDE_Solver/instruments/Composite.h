#ifndef COMPOSITE_H
#define COMPOSITE_H

#include <vector>
#include "../core/Layer.h"
#include "Space.h"

template<typename State_Iterator> 
class Composite1D{
	typedef typename State_Iterator::value_type state_type;
	typedef  Layer<state_type> value_type;
public:
	typedef typename value_type::recount_func recount_func;
	typedef typename value_type::iterator state_iterator;

	void placeLayer(State_Iterator from, State_Iterator to, int offset, recount_func forward_func, recount_func backward_func)
	{
		m_container.emplace(m_container.end(), from, to, forward_func, backward_func);
	};

private:
	std::vector<value_type> m_container;
};

#endif /* COMPOSITE_H */

