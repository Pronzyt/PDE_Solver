#ifndef LAYER_H
#define LAYER_H

#include <functional>



namespace core{


//template<typename State_Ty>
//class BaseLayer{
//public:
//	typedef BaseLayer<State_Ty> my_Ty;
//	typedef State_Ty value_type;
//
//	/*Осуществляет шаг пересчета 
//	return true, если конец диапазона был достигнут	*/
//	virtual bool recount_step() = 0; 
//	virtual bool recount() = 0;
//
//	/*	Переход на начало диапазона	*/
//	virtual void reset() = 0;
//	virtual ~BaseLayer() = 0;
//};
//
//template<typename Ty>
//BaseLayer<Ty>::~BaseLayer(){};



template<typename Iterator_Ty>
class Layer /*: public BaseLayer<typename Iterator_Ty::value_type>*/{
public:

	typedef Iterator_Ty state_iterator;
	typedef std::function<bool(state_iterator)> recount_func;


	Layer(int offset, recount_func func)
		: m_offset(offset), m_recount_func(func)
	{};


	Layer(Layer&& rhs) :
		m_begin(std::move(rhs.m_begin)),
		m_end(std::move(rhs.m_end)),
		m_offset(std::move(rhs.m_offset)),
		m_recount_func(std::move( rhs.m_recount_func))
	{};


	void set_range(state_iterator begin, state_iterator end)
	{
		std::swap(m_begin, begin);
		std::swap(m_end, end);
		std::advance(m_begin, m_offset);
		std::advance(m_end, m_offset);
	};


	bool recount()
	{
		bool result = true;
		for (state_iterator it = m_begin; it != m_end; ++it)
		{
			bool test1 = m_recount_func(it);
			result = result && test1;
		};
		return result;
	};


private:
	Layer() = delete;
	Layer& operator= (const Layer& rhs) = delete;
	
	state_iterator m_begin;
	state_iterator m_end;
	int m_offset;
	recount_func m_recount_func;
};


};



#endif	/* LAYER_H */