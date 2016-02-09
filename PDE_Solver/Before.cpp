#include <iostream>
#include "instruments\Composite.h"
#include "instruments\Space.h"
#include "io\IO.h"
#include <fstream>
#include "io/csvfile.h"


struct State{
	int num;
	double value;
	double prev_value;

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
typedef Composite::recount_func recount_func;

/*----------------------------Constants and parameters------------------------*/


const int scale = 10;

const int N1 = 60 * scale;
const int N2 = 10 * scale;
const int N3 = 10 * scale;
const int N4 = 100 * scale;
const int N5 = 120 * scale;


const int L1 = 30;
const int L2 = 200;
const int L3 = 250;


const double sigma = 5.67e-8;

const double h = 0.001 / scale;


const double h = 0.1;

double lambda1 = 47;
double lambda2 = 2;
double lambda3 = 2;


const double MAX_ITERATION_NUM = 100;
const double MAX_ITERATION_NUM = 1500000;
/*----------------------------------------------------------------------------*/


/*----------------------------Initialization functions------------------------*/

State init1(Space::size_type num)
{
	return {N5, 2100};
};


State init2(Space::size_type num)
{
	return {N5 + N4 + num, 60};
};


State init3(Space::size_type num)
{
	return{N5 + N4 + L3 + N3 + num, 60};
};


State init4(Space::size_type num)
{
	return{N5 + N4 + L3 + N3 + N2 + L2 + num, 60};
};


State init5(Space::size_type num)
{
	return{N5 + N4 + L3 + N3 + N2 + L2 + N1 + L1, 60};
};

/*----------------------------------------------------------------------------*/

/*----------------------View factors------------------------------------------*/


double viewFactor(State from, State to)
{
	return (from.num > to.num) ? double(to.num) / double(from.num) : 1;
};

/*----------------------------------------------------------------------------*/


/*---------------------Recount functions--------------------------------------*/

//Генератор итераций пересчета температуры в слое
recount_func generator()
{
	return[](state_iterator& arg)->State&
	{
		State& next = *++arg;
		State& curr = *--arg;
		State& prev = *--arg;
		curr.prev_value = curr.value;
		curr.value = 1 / (4 * curr.num) * (next.value - prev.value) + (next.value + prev.value) / 2;
		return curr;
	};
};

//Генератор итераций пересчета температуры на левой границе
recount_func generatorBoundaryLeft(double h, double lambda, double factor)
{
	return[h, lambda, factor](state_iterator& arg)->State&
	{
		State& next = *++arg;
		State& curr = *--arg;
		State& prev = *--arg;
		curr.prev_value = curr.value;
		double temp = 2 * h * sigma * factor / lambda * (std::pow(next.value, 4) - std::pow(curr.value, 4)) + prev.value;
		curr.value = 1 / (4 * curr.num) * (temp - prev.value) + (temp + prev.value) / 2;
		return curr;
	};
};

//Генератор итераций пересчета температуры на правой границе
recount_func generatorBoundaryRight(double h, double lambda, double factor)
{

	return[h, lambda, factor](state_iterator& arg)->State&
	{
		State& next = *++arg;
		State& curr = *--arg;
		State& prev = *--arg;
		curr.prev_value = curr.value;
		double temp = 2 * sigma * factor * h / lambda * (std::pow(prev.value, 4) - std::pow(curr.value, 4)) + next.value;
		curr.value = 1 / (4 * curr.num) * (next.value - temp) + (next.value + temp) / 2;
		return curr;
	};
};


State& empty(state_iterator& it)
{
	return *it;
};


/*----------------------------------------------------------------------------*/

/*---------------------Start and Pause functions-----------------------------*/
Space& start(Space& space, std::string filename)
{
	std::ifstream stream(filename);
	/*Если файл существует, грузим из него*/
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
		/*Иначе инициализируем по умолчанию*/
		space = Space{
			{1, init1},
			{L3, init2},
			{L2, init3},
			{L1, init4},
			{1, init5}};
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

	double view_factor_right = viewFactor(*space.getIterator(0), *space.getIterator(1));
	double view_factor_left = viewFactor(*space.getIterator(L3 + 1), *space.getIterator(L3));

	/*Внутренняя футеровка*/
	composite.placeLayer(space.getIterator(1), space.getIterator(1), generatorBoundaryRight(h, lambda3, view_factor_right), empty);
	composite.placeLayer(space.getIterator(2), space.getIterator(L3 - 1), generator(), empty);
	composite.placeLayer(space.getIterator(L3), space.getIterator(L3),  generatorBoundaryLeft(h, lambda3, view_factor_left), empty);

	view_factor_right = viewFactor(*space.getIterator(L3), *space.getIterator(L3 + 1));
	view_factor_left = viewFactor(*space.getIterator(L3 + L2 + 1), *space.getIterator(L3 + L2));

	/*Внешняя футеровка*/
	composite.placeLayer(space.getIterator(L3 + 1), space.getIterator(L3 + 1), generatorBoundaryRight(h, lambda2, view_factor_right), empty);
	composite.placeLayer(space.getIterator(L3 + 2), space.getIterator(L3 + L2 - 1), generator(), empty);
	composite.placeLayer(space.getIterator(L3 + L2), space.getIterator(L3 + L2), generatorBoundaryLeft(h, lambda2, view_factor_left), empty);

	view_factor_right = viewFactor(*space.getIterator(L3 + L2), *space.getIterator(L3 + L2 + 1));
	view_factor_left = viewFactor(*space.getIterator(L3 + L2 + L1 + 1), *space.getIterator(L3 + L2 + L1));
	
	/*Стальной экран*/
	composite.placeLayer(space.getIterator(L3 + L2 + 1), space.getIterator(L3 + L2 + 1), generatorBoundaryRight(h, lambda1, view_factor_right), empty);
	composite.placeLayer(space.getIterator(L3 + L2 + 2), space.getIterator(L3 + L2 + L1 - 1), generator(), empty);
	composite.placeLayer(space.getIterator(L3 + L2 + L1), space.getIterator(L3 + L2 + L1), generatorBoundaryLeft(h, lambda1, view_factor_left), empty);


	for (auto i = 0; i < MAX_ITERATION_NUM; ++i)
	{
		composite.recountForward();
		std::cout << " Number of iterations: " << i << " Value: " << space.getIterator(1)->value << "\n";
	};

	pause(space, "Example7.txt");

	char ch;
	std::cin >> ch;
    return 0;
};