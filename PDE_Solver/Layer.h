#include <iterator>
#include "Range.h"


template<typename Ty> 
class BaseLayer{
public:
	typedef BaseLayer<Ty> my_Ty;
	typedef Ty value_type;
	typedef value_type(*recount_func)(value_type&);
	virtual void setForwardFunc(recount_func func);
	virtual void setBackwardFunc(recount_func func);
	virtual void forward_recount_step() = 0;
//	virtual void backward_recount_step() = 0;
	virtual ~BaseLayer() = 0;
protected:
	recount_func m_f_recount_func;
	recount_func m_b_recount_func;
};

template<typename Ty>
class Layer: public BaseLayer<Ty>{
public:
	typedef typename BaseLayer<Ty>::value_type value_type;
	typedef Range<value_type> range_type;
	Layer();
	void setRange(range_type range);	
	virtual void forward_recount_step() override;
	virtual ~Layer() override{};
private:
	Range<value_type> m_range;
};

//class BaseBoundaryCondition: public BaseLayer{
//public:
//	BaseBoundaryCondition();
//        void setBound(state_iterator bound);        
//};


#include "Layer.hpp"