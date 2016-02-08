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
	double prev_iter_value;
	bool is_complete;

	template<typename Stream>
	void save(Stream& stream) const
	{
		saveToCSV(stream, num, value, prev_iter_value);
	};

	template <typename Iterator>
	void load(Iterator iterator)
	{
		loadFromCSV(iterator, num, value, prev_iter_value);
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

const int L1 = 3 * scale;
const int L2 = 20 * scale;
const int L3 = 25 * scale;
const int L4 = 10 * scale;
const int L5 = 110 * scale;

const double sigma = 5.67e-8;

const double h = 0.001 / scale;

/*Теплофизические свойства слоев (сталь, внутренняя и внешняя футеровки, 
вольфрамовый стакан и расплава Al2O3 соответственно */

const double lambda1 = 47; 
const double lambda2 = 2;
const double lambda3 = 2;
const double lambda4 = 162.8;
const double lambda5 = 2.05;

const double a1 = 1.172e-5;
const double a2 = 0.65e-6;
const double a3 = 0.65e-6;
const double a4 = 2.778e-5;
const double a5 = 4.8e-7;

const double time_end = 3 * 3600;
const double tau = 1;

const int MAX_ITERATION_NUM = 10000000;
const double EPSILON = 0.0001;


/*----------------------------------------------------------------------------*/
/*----------------------View factors------------------------------------------*/


double viewFactor(State from, State to)
{
	return 1;
	//return (from.num > to.num) ? double(to.num) / double(from.num) : 1;
};

/*----------------------------------------------------------------------------*/

/*---------------------Recount functions--------------------------------------*/
/*Здесь выбран обратный порядок пересчета, так как тело охлаждается*/


/*По условию симметрии*/
State& setCentralValueLeft(state_iterator& arg)
{
	State& curr = *arg;
	State& next = *--arg;
	curr.value = next.value;
	return curr;
};

State& setCentralValueRight(state_iterator& arg)
{
	double temp = arg->value;
	return *(++arg);
};

/*Граничное условие 4го рода*/
recount_func generatorIdealContact(double a1, double a2, double tau, double lambda1, double lambda2, double h1, double h2)
{
	double k1 = a1 * tau / (h1 * h1);
	double k2 = a2 * tau / (h2 * h2);

	double t1 = lambda1 / h1;
	double t2 = lambda2 / h2;

	double b1 = t1 / k1;
	double b2 = t2 / k2;

	return[k1, k2, b1, b2, t1, t2](state_iterator& arg)->State&
	{
		State& prev = *++arg;
		State& curr = *--arg;
		State& next = *--arg;
		curr.prev_value = curr.value;

		double i2plus = 1 + 1 / (2 * curr.num);
		double i2minus = 1 - 1 / (2 * curr.num);

		double denom = b1 * (1 + 2 * k1) / i2plus + b2 * (1 + 2 * k2) / i2minus;

		double numer = t2 * next.value * (1 + i2plus / i2minus) + 
					curr.prev_iter_value * (b2 / i2minus + b1 / i2plus) +
					t1 * prev.value * (1 + i2minus / i2plus);

		curr.value = numer / denom;

		curr.is_complete = next.is_complete && (std::abs(curr.value - curr.prev_value) < EPSILON);
		return curr;
	};
};

//Генератор итераций пересчета температуры в слое
recount_func generator(double a, double tau, double h)
{
	double k = a * tau / (h * h);
	double denom = 1 / (1 + 2 * k);
	double b = k * denom;

	return[b, denom](state_iterator& arg)->State&
	{
		State& prev = *++arg;
		State& curr = *--arg;
		State& next = *--arg;
		curr.prev_value = curr.value;
		curr.value = b * (next.value * (1 + 1 / (2 * curr.num)) + prev.value * (1 - 1 / (2 * curr.num))) + denom * curr.prev_iter_value;

		curr.is_complete = next.is_complete && (std::abs(curr.value - curr.prev_value) < EPSILON);
		return curr;
	};
};

//Генератор итераций пересчета температуры на левой границе
recount_func generatorBoundaryLeft(double h, double lambda, double a, double tau, double factor)
{
	double k = a * tau / (h * h); 
	double denom = 1 / (1 + 2 * k);
	double b = k * denom;
	double t = 2 * sigma * factor * h / lambda;

	return[b, t, denom](state_iterator& arg)->State&
	{
		State& prev = *++arg;
		State& curr = *--arg;
		State& next = *--arg;
		curr.prev_value = curr.value;
		double temp = t * (std::pow(next.value, 4) - std::pow(curr.value, 4)) + prev.value;
		curr.value = b * (temp * (1 + 1 / (2 * curr.num)) + prev.value * (1 - 1 / (2 * curr.num))) + denom * curr.prev_iter_value;

		curr.is_complete = next.is_complete && (std::abs(curr.value - curr.prev_value) < EPSILON);
		return curr;
	};
};

//Генератор итераций пересчета температуры на правой границе
recount_func generatorBoundaryRight(double h, double lambda, double a, double tau, double factor)
{
	double k = a * tau / (h * h);
	double denom = 1 / (1 + 2 * k);
	double b = k * denom;
	double t = 2 * sigma * factor * h / lambda;

	return[b, t, denom](state_iterator& arg)->State&
	{
		State& prev = *++arg;
		State& curr = *--arg;
		State& next = *--arg;
		curr.prev_value = curr.value;
		double temp = t * (std::pow(prev.value, 4) - std::pow(curr.value, 4)) + next.value;
		curr.value = b * (next.value * (1 + 1 / (2 * curr.num)) + temp * (1 - 1 / (2 * curr.num))) + denom * curr.prev_iter_value;

		curr.is_complete = next.is_complete && (std::abs(curr.value - curr.prev_value) < EPSILON);
		return curr;
	};
};


State& empty(state_iterator& it)
{
	return *it;
};


State& reset(state_iterator& it)
{
	it->prev_iter_value = it->value;
	it->is_complete = false;
	return *it;
};

/*---------------------------------------------------------------------------*/

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
		throw;
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

/*-------------------------------------------------------------------------*/


int main()
{
	Space space;

	Composite composite;

	start(space, "Example8.txt");

	int layer_begin = 0;
	int layer_end = L5;

	/*Расплав Al2O3*/
	composite.placeLayer(space.getIterator(layer_begin), space.getIterator(layer_begin), empty, setCentralValueLeft);
	composite.placeLayer(space.getIterator(layer_begin + 1), space.getIterator(layer_end - 1), empty, generator(a5, tau, h));

	/*Граничное условие 4го рода*/
	composite.placeLayer(space.getIterator(layer_end), space.getIterator(layer_end), empty, generatorIdealContact(a5, a4, tau, lambda5, lambda4, h, h));

	layer_begin += L5;
	layer_end += L4;
	double view_factor_left = viewFactor(*space.getIterator(layer_end + 1), *space.getIterator(layer_end));

	/*Вольфрамовый тигель*/
	composite.placeLayer(space.getIterator(layer_begin + 1), space.getIterator(layer_end - 1), empty, generator(a4, tau, h));
	composite.placeLayer(space.getIterator(layer_end), space.getIterator(layer_end), empty, generatorBoundaryLeft(h, lambda4, a4, tau, view_factor_left));


	layer_begin += L4 + 1;
	layer_end += L3;
	double view_factor_right = viewFactor(*space.getIterator(layer_begin - 1), *space.getIterator(layer_begin));
	view_factor_left = viewFactor(*space.getIterator(layer_end + 1), *space.getIterator(layer_end));
	
	/*Внутренняя футеровка*/
	composite.placeLayer(space.getIterator(layer_begin), space.getIterator(layer_begin), empty, generatorBoundaryRight(h, lambda3, a3, tau, view_factor_right));
	composite.placeLayer(space.getIterator(layer_begin + 1), space.getIterator(layer_end - 1), empty, generator(a3, tau, h));
	composite.placeLayer(space.getIterator(layer_end), space.getIterator(layer_end), empty, generatorBoundaryLeft(h, lambda3, a3, tau, view_factor_left));


	layer_begin += L3;
	layer_end += L2;
	view_factor_right = viewFactor(*space.getIterator(layer_begin - 1), *space.getIterator(layer_begin));
	view_factor_left = viewFactor(*space.getIterator(layer_end + 1), *space.getIterator(layer_end));

	/*Внешняя футеровка*/
	composite.placeLayer(space.getIterator(layer_begin), space.getIterator(layer_begin), empty, generatorBoundaryRight(h, lambda2, a2, tau, view_factor_right));
	composite.placeLayer(space.getIterator(layer_begin + 1), space.getIterator(layer_end - 1), empty, generator(a2, tau, h));
	composite.placeLayer(space.getIterator(layer_end), space.getIterator(layer_end), empty, generatorBoundaryLeft(h, lambda2, a2, tau, view_factor_left));


	layer_begin += L2;
	layer_end += L1;
	view_factor_right = viewFactor(*space.getIterator(layer_begin - 1), *space.getIterator(layer_begin));
	view_factor_left = viewFactor(*space.getIterator(layer_end + 1), *space.getIterator(layer_end));

	///*Стальной экран*/
	composite.placeLayer(space.getIterator(layer_begin), space.getIterator(layer_begin), empty, generatorBoundaryRight(h, lambda1, a1, tau, view_factor_right));
	composite.placeLayer(space.getIterator(layer_begin + 1), space.getIterator(layer_end - 1), empty, generator(a1, tau, h));
	composite.placeLayer(space.getIterator(layer_end), space.getIterator(layer_end), empty, generatorBoundaryLeft(h, lambda1, a1, tau, view_factor_left));

	/*Создаем условие остановки итераций*/
	space.getIterator(layer_end + 1)->is_complete = true;
	bool& done = space.getIterator(1)->is_complete;

	/*При переходе на следующий шаг по времени сохраняем значения*/
	Composite resetter;
	resetter.placeLayer(space.getIterator(1), space.getIterator(layer_end), reset, empty);

	/*Запускаем моделирование*/
	double current_time = 0;
	while (current_time < time_end)
	{
		int current_iteration_num = 0;
		while (!done)
		{
			composite.recountBackward();
			std::cout << "Current time: " << current_time << " Current iteration: " << ++current_iteration_num << '\n';
			if (current_iteration_num > MAX_ITERATION_NUM)
			{
				std::cout << 'D:\n';
				break;
			};
		};
		resetter.recountForward();
		current_time += tau;
	};

	pause(space, "Example8.txt");

	char ch;
	std::cin >> ch;
	return 0;
};