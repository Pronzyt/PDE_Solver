#ifndef LAYER_H
#define LAYER_H

#include <functional>
#include "Range.h"


template<typename State_Ty> 
class BaseLayer{
public:
	typedef BaseLayer<State_Ty> my_Ty;
	typedef State_Ty value_type;
	typedef IRange<Range<value_type>, value_type> iterator;
	typedef std::function<value_type&(iterator&)> recount_func;

	BaseLayer(const BaseLayer& rhs) 
		: m_f_recount_func(rhs.m_f_recount_func), m_b_recount_func(rhs.m_b_recount_func)
	{};
//	virtual void setForwardFunc(recount_func func);
//	virtual void setBackwardFunc(recount_func func);
	virtual bool forward_recount_step() = 0;
	virtual bool backward_recount_step() = 0;
	virtual void resetForward() = 0;
	virtual void resetBackward() = 0;
	virtual ~BaseLayer() = 0;
protected:
	BaseLayer(recount_func f_func, recount_func b_func)
		:m_f_recount_func(f_func), m_b_recount_func(b_func)
	{};
	recount_func m_f_recount_func;
	recount_func m_b_recount_func;
};

template<typename Ty>
BaseLayer<Ty>::~BaseLayer(){};


template<typename State_Ty>
class Layer: public BaseLayer<State_Ty>{
public:
	typedef Layer<State_Ty> my_Ty;
	typedef typename my_Ty::value_type value_type;
	typedef typename my_Ty::recount_func recount_func;

	Layer() = delete;

	template<typename BidirectionalIterator>
	Layer(BidirectionalIterator from, BidirectionalIterator to, recount_func f_func, recount_func b_func)
		:BaseLayer<State_Ty>(f_func, b_func), m_f_range(from, to), m_r_range(from, to)
	{};

	virtual ~Layer() override{};

//	Layer(const Layer& rhs) : BaseLayer<State_Ty>(rhs), m_range(rhs.m_range){};
//	void setRange(range_type range);	
	virtual bool forward_recount_step() override
	{
		auto temp = m_f_range;
		this->m_f_recount_func(temp);
		++m_f_range;
		return m_f_range.in_end();
	};

	virtual bool backward_recount_step() override
	{
		auto temp = m_r_range;
		this->m_b_recount_func(temp);
		++m_r_range;
		return m_r_range.in_end();
	};

	virtual void resetForward() override
	{
		m_f_range.to_begin();
	};

	virtual void resetBackward() override
	{
		m_r_range.to_begin();
	};

private:
	FRange<value_type> m_f_range;
	RRange<value_type> m_r_range;
};


#endif	/* LAYER_H */