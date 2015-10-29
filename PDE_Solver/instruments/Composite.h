#ifndef COMPOSITE_H
#define COMPOSITE_H

#include <vector>
#include "../core/Layer.h"
#include "Space.h"

template<typename State_Iterator> 
class Composite{
public:
	typedef State_Iterator state_iterator;
	typedef typename state_iterator::value_type state_type;
	typedef  Layer<state_type> value_type;
	typedef typename Layer<state_type>::recount_func recount_func;


	void placeLayer(state_iterator from, state_iterator to, int offset, recount_func forward_func, recount_func backward_func)
	{
		m_container.emplace(m_container.end(), from, to, forward_func, backward_func);
	};



private:
	std::vector<value_type> m_container;
};



#endif