#include <iostream>
#include "instruments\Composite.h"
#include "instruments\Space.h"
#include "io\IO.h"


const int N1 = 10;
const int N2 = 25;
const int N3 = 40;



struct State{
	int num;
};


typedef Space1D<State> Space;
typedef Composite1D<Space::iterator> Composite;
typedef Composite::state_iterator state_iterator;



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


State& empty(state_iterator& it)
{
	return *it;
};



int main()
{
	Space space{
		{5, init1},
		{5, init2},
		{5, init3}};

	Composite composite;
	composite.placeLayer(space.getIterator(0), space.getIterator(5), 0, empty, empty);

	char ch;
	std::cin >> ch;



    return 0;
};