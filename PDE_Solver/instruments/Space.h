#ifndef SPACE_H
#define SPACE_H

#include <vector>
#include <initializer_list>

template <typename Ty>
class Space1D
{
public:
	typedef Space1D<Ty> my_Ty;
	typedef std::vector<Ty> container_type;
	typedef typename container_type::size_type size_type;
	typedef typename container_type::value_type value_type;
	typedef typename container_type::iterator iterator;
	typedef typename container_type::const_iterator const_iterator;
	typedef value_type (*init_func_type)(size_type);

	struct Initilizer
	{
		size_type range;
		init_func_type init_func;
	};

	typedef std::initializer_list<Initilizer> Initilizers;

	Space1D(){};

	Space1D(size_type size)
	{
		m_container.resize(size);
	};

	Space1D(size_type size, init_func_type init_func)
	{
		m_container.resize(size);
		size_type x = 0;
		for (auto it = m_container.begin(); it != m_container.end(); ++it)
		{
			*it = init_func(x++);
		};
	};

	//Каждая инициализирующая функция вызывается range раз,
	//при этом в нее передаются параметр из диапазона (0, range)
	Space1D(Initilizers initilizers)
	{
		size_type size = 0;
		for (auto initilizer = initilizers.begin(); initilizer != initilizers.end(); ++initilizer)
		{
			size += initilizer->range;
		};

		m_container.resize(size);
		auto curr = m_container.begin();
		for (auto initilizer = initilizers.begin(); initilizer != initilizers.end(); ++initilizer)
		{
			for (size_type i = 0; i < initilizer->range; ++i)
			{
				(*curr) = initilizer->init_func(i);
				++curr;
			};
		};
	};

	Space1D(const Space1D&){};

	~Space1D(){};

	iterator begin()
	{
		return this->m_container.begin();
	};

	iterator end()
	{
		return this->m_container.end();
	};

	const_iterator cbegin() const
	{
		return this->m_container.cbegin();
	};

	const_iterator cend() const
	{
		return this->m_container.cend();
	};

	value_type operator[](int i)
	{
		return this->m_container[i];
	};

	size_type size()
	{
		return m_container.size();
	};

	iterator getIterator(size_type pos)
	{
		iterator temp = m_container.begin();
		std::advance(temp, pos);
		return temp;
	};

	//Range<value_type> getRange(size_type from, size_type to);
private:
	container_type m_container;
};

#endif /*SPACE_H*/

