template<typename Ty>
Space1D<Ty>::Space1D(Space1D<Ty>::size_type size)
{
    m_container.resize(size);
}

template<typename Ty>
Space1D<Ty>::Space1D(Space1D<Ty>::size_type size, Space1D<Ty>::init_func_type init_func)
{
	m_container.resize(size);
	my_Ty::size_type x = 0;
	for(auto it = m_container.begin(); it != m_container.end(); it++)
	{
		*it = init_func(x++);
	};
}

template<typename Ty>
typename Space1D<Ty>::iterator Space1D<Ty>::begin()
{
	return this->m_container.begin();
}

template<typename Ty>
typename Space1D<Ty>::iterator Space1D<Ty>::end()
{
    return this->m_container.end();
}

template<typename Ty>
typename Space1D<Ty>::const_iterator Space1D<Ty>::cbegin()
{
	return this->m_container.cbegin();
}

template<typename Ty>
typename Space1D<Ty>::const_iterator Space1D<Ty>::cend()
{
    return this->m_container.cend();
}

template<typename Ty>
typename Space1D<Ty>::value_type Space1D<Ty>::operator [](int i)
{
	return this->m_container[i];
}
