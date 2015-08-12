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
