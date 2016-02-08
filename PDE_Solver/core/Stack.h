#ifndef STACK_H
#define STACK_H


#include "Range.h"
#include "Layer.h"
#include <stack>

namespace core
{



template<typename Layer_Ty>
class Stack{
public:
	typedef Layer_Ty value_type;

	//template<typename BidirectionalIterator>
	//Stack(std::pair<BidirectionalIterator, BidirectionalIterator> range)
	//	:m_range(range.first, range.second)
	//{};

	template<typename BidirectionalIterator>
	void set_range(std::pair<BidirectionalIterator, BidirectionalIterator> range)
	{
		
	};

	void push(size_t size, value_type val)
	{

		m_stack.push(val);
	};

	void push(){};

	//value_type pop();
	//recount;

private:
	Stack() = delete;

	Range<typename Layer_Ty::value_type> m_range;
	std::stack<value_type> m_stack;
	
};

};


#endif
