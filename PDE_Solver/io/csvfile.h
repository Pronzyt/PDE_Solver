#ifndef CSVFILE_H
#define CSVFILE_H

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

//http://stackoverflow.com/questions/1120140/

class CSVRow{
public:
	typedef std::vector<std::string>::const_iterator const_iterator;
    std::string const& operator[](size_t index) const;
    size_t size() const {return _data.size();};
    void readNextRow(std::istream& str);
	const_iterator cbegin() const { return _data.cbegin(); };
	const_iterator cend() const { return _data.cend(); };

private:
    std::vector<std::string> _data;
};


inline std::istream& operator>>(std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}   


class CSVIterator
{   
public:
    typedef std::input_iterator_tag								iterator_category;
    typedef CSVRow												value_type;
	typedef std::istreambuf_iterator<char>::difference_type		difference_type;
    typedef CSVRow*											    pointer;
    typedef CSVRow&											    reference;

	CSVIterator(std::istream& str) :_str(str.good() ? &str : NULL) { ++(*this); };
	CSVIterator() :_str(NULL) {};
	//CSVIterator(const char* name)	:

    //	Pre Increment
	CSVIterator& operator++()               { if (_str) { (*_str) >> _row; _str = _str->good() ? _str : NULL; }return *this; };
    //	Post increment
	CSVIterator operator++(int)             { CSVIterator    tmp(*this); ++(*this); return tmp; };
	CSVRow const& operator*()   const       { return _row; };
	CSVRow const* operator->()  const       { return &_row; };

	bool operator==(CSVIterator const& rhs) { return ((this == &rhs) || ((this->_str == NULL) && (rhs._str == NULL))); };
	bool operator!=(CSVIterator const& rhs) { return !((*this) == rhs); };
	bool hasNext(){ return !(_str == NULL); };
	size_t size() const
	{
		auto pos = _str->tellg();
		difference_type result = std::count(std::istreambuf_iterator<char>(*_str), std::istreambuf_iterator<char>(), '\n');
		_str->seekg(pos);
		return  static_cast<size_t>(result);
	};
private:
    std::istream*       _str;
    CSVRow              _row;
};

#endif