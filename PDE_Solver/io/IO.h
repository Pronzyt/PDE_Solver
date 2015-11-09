#ifndef IO_H
#define IO_H

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "csvfile.h"


template<typename Stream, typename Ty>
void saveToCSV(Stream& stream, Ty value)
{
	stream << std::to_string(value).c_str() << "\n";
};


template<typename Stream, typename Ty, typename... Args>
void saveToCSV(Stream& stream, Ty value, Args... args)
{
	stream << std::to_string(value).c_str() << ",";
	saveToCSV(stream, args);
};


template<typename Ty, typename... Args>
void loadFromCSV(Ty& value, Args&... args)
{

};



#endif