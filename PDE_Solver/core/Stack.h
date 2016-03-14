#ifndef STACK_H
#define STACK_H

#include <vector>

namespace core
{



template<typename Val_Ty>
class Stack{
public:
	typedef Val_Ty value_type;

	typedef typename Val_Ty::state_iterator state_iterator;
	typedef typename state_iterator::value_type state_type;
	typedef typename Val_Ty::recount_func recount_func;


	Stack(){};


	void set_range(state_iterator begin, state_iterator end)
	{
		state_iterator begin_local = begin;
		state_iterator end_local = begin;
		for (auto it = m_container.begin(); it != m_container.end(); ++it)
		{
			std::advance(end_local, it->first);
			it->second.set_range(begin_local, end_local);
			std::advance(begin_local, it->first);
		};		
	};


	void set_reverse_range(state_iterator begin, state_iterator end)
	{
		state_iterator begin_local = begin;
		state_iterator end_local = begin;
		for (auto it = m_container.rbegin(); it != m_container.rend(); ++it)
		{
			std::advance(end_local, it->first);
			it->second.set_range(begin_local, end_local);
			std::advance(begin_local, it->first);
		};
	};


	bool recount_until(const bool& condition)
	{
		bool result = true;
		//while (!condition)
		{
			for (auto it = m_container.begin(); it != m_container.end(); ++it)
			{
				result = it->second.recount() && result;
			};
		};
		return result;
	};


	bool reverse_recount_until(const bool& condition)
	{
		bool result = true;
		//while (!condition)
		{
			for (auto it = m_container.rbegin(); it != m_container.rend(); ++it)
			{
				result = it->second.recount() && result;
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


	size_t total_size()
	{
		size_t size = 0;
		for (auto it = m_container.begin(); it != m_container.end(); ++it)
		{
			size += it->first;
		};
		return size;
	};

private:
	typedef std::pair<size_t, value_type> pair_type;
	typedef std::vector<pair_type> container_type;

	Stack(const Stack&) = delete;
	Stack(Stack&&) = delete;
	Stack& operator=(const Stack&) = delete;


	container_type m_container;
	
};

};


#endif
