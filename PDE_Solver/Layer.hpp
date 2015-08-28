template<typename Ty>
BaseLayer<Ty>::BaseLayer(recount_func f_func, recount_func b_func)
	:m_f_recount_func(f_func), m_b_recount_func(b_func)
{}

template<typename Ty>
BaseLayer<Ty>::~BaseLayer(){}

//template<typename Ty>
//void BaseLayer<Ty>::setForwardFunc(recount_func func)
//{
//	m_f_recount_func = func;
//}
//
//template<typename Ty>
//void BaseLayer<Ty>::setBackwardFunc(recount_func func)
//{
//	m_b_recount_func = func;
//}

//template<typename Ty>
//Layer<Ty>::Layer()
//{
//	this->m_f_recount_func = []( value_type& val)->value_type{return val;};			
//}

template<typename Ty>
Layer<Ty>::Layer(range_type range, recount_func f_func, recount_func b_func)
	: BaseLayer<Ty>(f_func, b_func),
	m_range(range)
{}


template<typename Ty>
void Layer<Ty>::setRange(range_type range)
{
	m_range = range;
}

template<typename Ty>
bool Layer<Ty>::forward_recount_step()
{
	range_type temp = m_f_range;
	this->m_f_recount_func(temp);
	++m_f_range;
	return m_f_range.in_end();
}

template<typename Ty>
bool Layer<Ty>::backward_recount_step()
{
	range_type temp = m_r_range;
	this->m_b_recount_func(temp);
	++m_r_range;
	return m_r_range.in_end();
}

template<typename Ty>
void Layer<Ty>::resetForward()
{
	m_f_range.to_begin();
}

template<typename Ty>
void Layer<Ty>::resetBackward()
{
	m_r_range.to_begin();
}
