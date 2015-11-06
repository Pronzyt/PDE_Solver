#include <iostream>
#include "instruments\Composite.h"
#include "instruments\Space.h"


const int N1 = 10;
const int N2 = 25;
const int N3 = 40;



struct State{
	int num;
};

typedef Space1D<State> Space;
typedef Composite<Space::iterator>::value_type::iterator iter;


State init1(Space::size_type num)
{
	return {num};
};

State init2(Space::size_type num)
{
	return{N2 + num};
};


State init3(Space::size_type num)
{
	return{ N3 + num };
};


State& empty(iter& it)
{
	return *it;
};


int main()
{
	Space1D<State> space1d{
		{5, init1},
		{5, init2},
		{5, init3}};

	Composite<Space::iterator> composite;
	composite.placeLayer(space1d.getIterator(0), space1d.getIterator(5), 0, empty, empty);

	char ch;
	std::cin >> ch;
    return 0;
};