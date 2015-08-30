template<typename Ty>
Space1D<Ty>::Space1D(typename Space1D<Ty>::size_type size)
{
    m_container.resize(size);
}

template<typename Ty>
Space1D<Ty>::Space1D(typename Space1D<Ty>::size_type size, typename Space1D<Ty>::init_func_type init_func)
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
typename Space1D<Ty>::const_iterator Space1D<Ty>::cbegin() const
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

template<typename Ty>
typename Space1D<Ty>::iterator Space1D<Ty>::getIterator(size_type pos)
{
	iterator temp = m_container.begin();
	std::advance(temp, pos);
	return temp;
}

//template<typename Ty>
//Range<typename Space1D<Ty>::value_type> Space1D<Ty>::getRange(size_type from, size_type to)
//{
//	return Range<value_type>(m_container.begin(), m_container.end());
//}
