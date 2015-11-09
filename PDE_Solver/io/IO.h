#ifndef IO_H
#define IO_H

#include <string>
#include "boost/lexical_cast.hpp"


template<typename Stream, typename Ty>
void saveToCSV(Stream& stream, Ty value)
{
	stream << std::to_string(value).c_str() << "\n";
};


template<typename Stream, typename Ty, typename... Args>
void saveToCSV(Stream& stream, Ty value, Args... args)
{
	stream << std::to_string(value).c_str() << ",";
	saveToCSV(stream, args...);
};


template<typename ForwardIterator, typename Ty>
void loadFromCSV(ForwardIterator iterator, Ty& value)
{
	value = boost::lexical_cast<Ty>(*iterator);
};


template<typename ForwardIterator, typename Ty, typename... Args>
void loadFromCSV(ForwardIterator iterator, Ty& value, Args&... args)
{
	value = boost::lexical_cast<Ty>(*iterator);
	loadFromCSV(++iterator, args...);
};


#endif