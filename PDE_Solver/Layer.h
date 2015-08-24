#ifndef LAYER_H
#define LAYER_H

#include <iterator>
#include "Range.h"


template<typename Ty> 
class BaseLayer{
public:
	typedef BaseLayer<Ty> my_Ty;
	typedef Ty value_type;
	typedef Range<value_type> range_type; //Создать интерфейс
	typedef value_type&(*recount_func)(range_type);
//	virtual void setForwardFunc(recount_func func);
//	virtual void setBackwardFunc(recount_func func);
	virtual bool forward_recount_step() = 0;
	virtual bool backward_recount_step() = 0;
//	virtual void resetForward() = 0;
//	virtual void resetBackward() = 0;
	virtual ~BaseLayer() = 0;
protected:
	BaseLayer(recount_func f_func, recount_func b_func);
	recount_func m_f_recount_func;
	recount_func m_b_recount_func;
};


template<typename Ty>
class Layer: public BaseLayer<Ty>{
public:
	typedef Layer<Ty> my_Ty;
	typedef typename my_Ty::value_type value_type;
	typedef Range<value_type> range_type;
	typedef typename my_Ty::recount_func recount_func;
	Layer() = delete;
	Layer(range_type range, recount_func f_func, recount_func b_func);
	void setRange(range_type range);	
	virtual bool forward_recount_step() override;
	virtual bool backward_recount_step() override;
//	virtual void resetForward() override;
//	virtual void resetBackward() override;
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

#endif	/* LAYER_H */