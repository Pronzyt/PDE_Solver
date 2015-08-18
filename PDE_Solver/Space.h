#include <vector>
#include <iterator>
#include "Range.h"

template<typename Ty>
class Space1D
{
public:
	typedef Space1D<Ty> my_Ty;
	typedef std::vector<Ty> container_type;
	typedef typename container_type::size_type size_type;
	typedef typename container_type::value_type value_type;
	typedef typename container_type::iterator iterator;
	typedef typename container_type::const_iterator const_iterator;	
	typedef value_type(*init_func_type)(size_type);
	Space1D(){};
	Space1D(size_type size);
	Space1D(size_type size, init_func_type init_func);
	Space1D(const Space1D&){};
	~Space1D(){};
	iterator begin();
	iterator end();
	const_iterator cbegin() const;
	const_iterator cend();
	value_type operator[](int i);
	size_type size();	
	Range<value_type> getRange(size_type from, size_type to);
private:
	container_type m_container;
};

#include "Space.hpp"