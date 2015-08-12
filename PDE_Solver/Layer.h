#include <iterator>

typedef double Ty;
typedef std::iterator<std::bidirectional_iterator_tag, Ty> state_iterator;


class BaseLayer{
public:
	typedef Ty value_type;
	typedef Ty(*recount_func)(const Ty&);
	virtual void setForwardFunc(recount_func func);
	virtual void setBackwardFunc(recount_func func);
	virtual void recount_forward_step() = 0;
	virtual void recount_backward_step() = 0;
	virtual ~BaseLayer() = 0 {};
protected:
	recount_func m_f_recount_func;
	recount_func m_b_recount_func;
};


class Layer: public BaseLayer{
public:
	Layer();
	void setRange(state_iterator begin, state_iterator end);	
};

class BaseBoundaryCondition: public BaseLayer{
public:
	BaseBoundaryCondition();
        void setBound(state_iterator bound);        
};