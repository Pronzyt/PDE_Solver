#ifndef LAYER_H
#define LAYER_H

#include <iterator>
#include <functional>
#include "Range.h"


template<typename Ty> 
class BaseLayer{
public:
	typedef BaseLayer<Ty> my_Ty;
	typedef Ty value_type;
	typedef IRange<Range<value_type>, value_type> iterator;
	//typedef value_type&(*recount_func)(iterator&);
	typedef std::function<value_type&(iterator&)> recount_func;
	BaseLayer(const BaseLayer& rhs) : m_f_recount_func(rhs.m_f_recount_func),
									  m_b_recount_func(rhs.m_b_recount_func){};
//	virtual void setForwardFunc(recount_func func);
//	virtual void setBackwardFunc(recount_func func);
	virtual bool forward_recount_step() = 0;
	virtual bool backward_recount_step() = 0;
	virtual void resetForward() = 0;
	virtual void resetBackward() = 0;
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
	typedef typename my_Ty::recount_func recount_func;
	Layer() = delete;
	template<typename BidirectionalIterator>
	Layer(BidirectionalIterator from, BidirectionalIterator to, recount_func f_func, recount_func b_func);
//	Layer(const Layer& rhs) : BaseLayer<Ty>(rhs), m_range(rhs.m_range){};
//	void setRange(range_type range);	
	virtual bool forward_recount_step() override;
	virtual bool backward_recount_step() override;
	virtual void resetForward() override;
	virtual void resetBackward() override;
	virtual ~Layer() override{};
private:
	FRange<value_type> m_f_range;
	RRange<value_type> m_r_range;
};


#include "Layer.hpp"

#endif	/* LAYER_H */