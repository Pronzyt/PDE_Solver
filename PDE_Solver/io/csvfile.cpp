#include "csvfile.h"

const std::string& CSVRow::operator[](std::size_t index) const
{
    return _data[index];
};

void CSVRow::readNextRow(std::istream& stream)
{
    _data.clear();

	std::string line;
	std::getline(stream, line);	

	std::stringstream lineStream(line);
    std::string cell;

    while(std::getline(lineStream, cell, ','))
    {
        _data.push_back(cell);
    };
};
