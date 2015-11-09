#include <iostream>
#include "instruments\Composite.h"
#include "instruments\Space.h"
#include "io\IO.h"
#include <fstream>
#include "io/csvfile.h"


const int N1 = 10;
const int N2 = 25;
const int N3 = 40;



struct State{
	int num;
	double value;

	template<typename Stream>
	void save(Stream& stream) const
	{
		saveToCSV(stream, num, value);
	};

	template <typename Iterator>
	void load(Iterator iterator)
	{
		loadFromCSV(iterator, num, value);
	};
};


typedef Space1D<State> Space;
typedef Composite1D<Space::iterator> Composite;
typedef Composite::state_iterator state_iterator;



State init1(Space::size_type num)
{
	return {num, num};
};

State init2(Space::size_type num)
{
	return {N2 + num, num};
};


State init3(Space::size_type num)
{
	return { N3 + num, num };
};


State& empty(state_iterator& it)
{
	return *it;
};


//---------------------Start and Pause functions-----------------------------//
Space& start(Space& space, std::string filename)
{
	std::ifstream stream(filename);
	if (stream.is_open())
	{
		CSVIterator csviterator(stream);
		space = Space(csviterator.size());
		for (auto it = space.begin(); (it != space.end()) && csviterator.hasNext(); ++it, ++csviterator)
		{
			it->load(csviterator->cbegin());
		};
		stream.close();
	}
	else
	{
		space = Space{
			{5, init1},
			{5, init2},
			{5, init3}};
	};
	return space;
};


void pause(const Space& space, std::string filename)
{
	std::ofstream stream(filename);
	std::stringstream sstream;
	for (auto it = space.cbegin(); it != space.cend(); ++it)
	{
		it->save(stream);
	};
	stream << "\n";	
	stream.close();
};

/*------------------------------------------------------------------------------------*/


int main()
{
	Space space;

	Composite composite;

	start(space, "Example7.txt");
	pause(space, "Example7.txt");

	composite.placeLayer(space.getIterator(0), space.getIterator(5), 0, empty, empty);
	composite.recountForward();

	char ch;
	std::cin >> ch;
    return 0;
};