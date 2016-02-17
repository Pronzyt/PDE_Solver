#ifndef STACK_H
#define STACK_H


#include "Range.h"
#include "Layer.h"
#include <functional>
#include <vector>

namespace core
{



template<typename Val_Ty>
class Stack{
public:
	typedef Val_Ty value_type;

	typedef typename  Val_Ty::state_iterator state_iterator;
	typedef typename state_iterator::value_type state_type;


	Stack(){};


	void set_range(state_iterator begin, state_iterator end)
	{
		state_iterator begin_local = begin;
		state_iterator end_local = begin;
		for (auto it = m_container.begin(); it != m_container.end(); ++it)
		{
			std::advance(end_local, it->second + 1);
			it->first.set_range(begin_local, end_local);
			std::advance(begin_local, it->second);
		};		
	};


	bool recount_until(const bool& condition)
	{
		bool result = true;
		while (!condition)
		{
			for (auto it = m_container.begin(); it != m_container.end(); ++it)
			{
				result = it->first.recount() && result;
			};
		};
		return result;
	};


	void push(size_t size, value_type val)
	{
		m_container.emplace_back(size, val);
	};


	template<typename... Args>
	void push(size_t size, Args&&... args)
	{
		m_container.emplace_back(size, Val_Ty(std::forward<Args>(args)...));
	};


	value_type top()
	{
		return m_container.back();
	};


	void pop()
	{
		return m_container.pop_back();
	};


private:
	typedef std::pair<size_t, value_type> pair_type;
	typedef std::vector<pair_type> container_type;

	Stack(const Stack&) = delete;
	Stack(Stack&&) = delete;
	Stack& operator=(const Stack&) = delete;

	state_iterator m_begin;
	state_iterator m_end;
	container_type m_container;
	
};

};


#endif
