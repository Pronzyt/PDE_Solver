#include <vector>
#include <iterator>

template<typename Ty>
class Space1D
{
public:
	typedef typename std::vector<Ty>::size_type size_type;
	typedef typename std::vector<Ty>::value_type value_type;
	typedef typename std::vector<Ty>::iterator iterator;
	typedef typename std::vector<Ty>::const_iterator const_iterator;
	typedef value_type(*init_func);
	Space1D(){};
	Space1D(const Space1D&){};
	~Space1D(){};
	iterator begin();
	iterator end();
	const_iterator cbegin();
	const_iterator cend();
	value_type operator[](int i);
	size_type size();	
private:
	std::vector<Ty> m_container;
};

#include "Space.hpp"