template<typename Ty>
BaseLayer<Ty>::~BaseLayer(){}

template<typename Ty>
void BaseLayer<Ty>::setForwardFunc(recount_func func)
{
	m_f_recount_func = func;
}

template<typename Ty>
void BaseLayer<Ty>::setBackwardFunc(recount_func func)
{
	m_b_recount_func = func;
}

template<typename Ty>
Layer<Ty>::Layer()
{
	this->m_f_recount_func = []( value_type& val)->value_type{return val;};			
}

template<typename Ty>
void Layer<Ty>::setRange(range_type range)
{
	m_range = range;
}


template<typename Ty>
void Layer<Ty>::forward_recount_step()
{
//	*m_current = this->m_f_recount_func(*m_current);
//	m_current++;
}



