#include <vector>
#include <iterator>

typedef double Ty;

class Space1D
{
public:
	typedef std::vector<Ty>::size_type size_type;
	typedef std::vector<Ty>::value_type value_type;
	typedef std::vector<Ty>::iterator iterator;
	typedef std::vector<Ty>::const_iterator const_iterator;
	typedef value_type(*init_func);
	Space1D() = delete;
	Space1D(const Space1D&);
	~Space1D();
	iterator begin();
	iterator end();
	const_iterator cbegin();
	const_iterator cend();
	value_type operator[](int i);
	size_type size();	
private:
	std::vector<Ty> m_container;
};