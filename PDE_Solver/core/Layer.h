#ifndef LAYER_H
#define LAYER_H

#include <functional>
#include "Range.h"

/*Базовый класс, который производит пересчет состояний (State_Ty),
используя функцию пересчета recount_func. Доступ к диапазону точек осуществляется
при помощи интерфейса IRange.*/


namespace core{


template<typename State_Ty>
class BaseLayer{
public:
	typedef BaseLayer<State_Ty> my_Ty;
	typedef State_Ty value_type;
	typedef IRange<Range<value_type>, value_type> iterator;
	typedef std::function<value_type&(iterator&)> recount_func;

	BaseLayer(const BaseLayer& rhs)
		: m_recount_func(rhs.m_recount_func)
	{};

	/*Осуществляет шаг пересчета 
	return true, если конец диапазона был достигнут	*/
	virtual bool recount_step() = 0; 
	virtual void recount(){ while (!recount_step()){}; }

	/*	Переход на начало диапазона	*/
	virtual void reset() = 0;
	virtual ~BaseLayer() = 0;
protected:
	BaseLayer(recount_func func)
		:m_recount_func(func)
	{};
	recount_func m_recount_func;
};

template<typename Ty>
BaseLayer<Ty>::~BaseLayer(){};



template<template<typename>class Range_Ty, typename State_Ty>
class Layer : public BaseLayer<State_Ty>{
public:

	template<typename BidirectionalIterator>
	Layer(BidirectionalIterator from, BidirectionalIterator to, int offset, recount_func func)
		:BaseLayer<State_Ty>(func), m_range(from, to, offset)
	{};

	template<typename BidirectionalIterator>
	Layer(std::pair<BidirectionalIterator, BidirectionalIterator> range, int offset, recount_func func)
		:Layer(range.first, range.second, offset, func)
	{};

	~Layer() override{};

	bool recount_step() override
	{
		m_range.hold();
		this->m_recount_func(m_range);
		m_range.reset();
		++m_range;
		return m_range.in_end();
	};

	void reset() override
	{
		m_range.to_begin();
	};

private:
	Layer() = delete;
	Range_Ty<State_Ty> m_range;
};


template<typename State_Ty>
class FLayer : public Layer<FRange, State_Ty>{
public:
	template <typename... Args>
	explicit FLayer(Args&&... args)
		: Layer<FRange, State_Ty>(std::forward<Args>(args)...)
	{ };
};


template<typename State_Ty>
class RLayer : public Layer<RRange, State_Ty>{
public:
	template <typename... Args>
	explicit RLayer(Args&&... args)
		: Layer<RRange, State_Ty>(std::forward<Args>(args)...)
	{ };
};

};

#endif	/* LAYER_H */