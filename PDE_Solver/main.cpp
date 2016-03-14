//#include "core/Layer.h"

#include <vector>
#include <fstream>
#include "core/Layer.h"
#include "core/Stack.h"
#include "instruments/Space.h"
#include "io/IO.h"
#include "io/csvfile.h"
#include "functions/functions.h"
#include "test/TimeTester.h"


struct State{
	int num;
	double value;
	double prev_value;
	double prev_iter_value;

	template<typename Stream>
	void save(Stream& stream) const
	{
		saveToCSV(stream, num, value);
	};

	template <typename Iterator>
	void load(Iterator iterator)
	{
		loadFromCSV(iterator, num, value);
		prev_iter_value = value;
	};
};


typedef Space1D<State> Space;
typedef core::Stack<core::Layer<Space::reverse_iterator>> Stack;
typedef Stack::state_iterator state_iterator;
typedef Stack::recount_func recount_func;




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



bool setCentralValueLeft(state_iterator arg)
{
	State& next = *arg;
	State& curr = *++arg;
	curr.value = next.value;
	return true;
};


recount_func generator(double a, double tau, double h)
{
	double k = a * tau / (h * h);
	double denom = 1 / (1 + 2 * k);
	double b = k * denom;

	return[b, denom](state_iterator arg)->bool
	{
		State& next = *arg;
		State& curr = *++arg;
		State& prev = *++arg;

		curr.prev_value = curr.value;
		curr.value = b * (next.value * (1 + 1 / (2 * curr.num)) + prev.value * (1 - 1 / (2 * curr.num))) + denom * curr.prev_iter_value;

		return std::abs(curr.value - curr.prev_value) < EPSILON;
	};
};


recount_func generatorIdealContact(double a1, double a2, double tau, double lambda1, double lambda2, double h1, double h2)
{
	double k1 = a1 * tau / (h1 * h1);
	double k2 = a2 * tau / (h2 * h2);

	double t1 = lambda1 / h1;
	double t2 = lambda2 / h2;

	double b1 = t1 / k1;
	double b2 = t2 / k2;

	return[k1, k2, b1, b2, t1, t2](state_iterator arg)->bool
	{
		State& next = *arg;
		State& curr = *++arg;
		State& prev = *++arg;

		curr.prev_value = curr.value;

		double i2plus = 1 + 1 / (2 * curr.num);
		double i2minus = 1 - 1 / (2 * curr.num);

		double denom = b1 * (1 + 2 * k1) / i2plus + b2 * (1 + 2 * k2) / i2minus;

		double numer = t2 * next.value * (1 + i2plus / i2minus) +
			curr.prev_iter_value * (b2 / i2minus + b1 / i2plus) +
			t1 * prev.value * (1 + i2minus / i2plus);

		curr.value = numer / denom;

		return std::abs(curr.value - curr.prev_value) < EPSILON;
	};
};


recount_func generatorBoundaryLeft(double h, double lambda, double a, double tau, double viewfactor)
{
	double k = a * tau / (h * h);
	double denom = 1 / (1 + 2 * k);
	double b = k * denom;
	double t = 2 * sigma * viewfactor * h / lambda;

	return[b, t, denom](state_iterator arg)->bool
	{
		State& next = *arg;
		State& curr = *++arg;
		State& prev = *++arg;

		curr.prev_value = curr.value;
		double temp = t * (std::pow(next.value, 4) - std::pow(curr.value, 4)) + prev.value;
		curr.value = b * (temp * (1 + 1 / (2 * curr.num)) + prev.value * (1 - 1 / (2 * curr.num))) + denom * curr.prev_iter_value;

		return (std::abs(curr.value - curr.prev_value) < EPSILON);
	};
};


recount_func generatorBoundaryRight(double h, double lambda, double a, double tau, double factor)
{
	double k = a * tau / (h * h);
	double denom = 1 / (1 + 2 * k);
	double b = k * denom;
	double t = 2 * sigma * factor * h / lambda;

	return[b, t, denom](state_iterator arg)->bool
	{
		State& next = *arg;
		State& curr = *++arg;
		State& prev = *++arg;

		curr.prev_value = curr.value;
		double temp = t * (std::pow(prev.value, 4) - std::pow(curr.value, 4)) + next.value;
		curr.value = b * (next.value * (1 + 1 / (2 * curr.num)) + temp * (1 - 1 / (2 * curr.num))) + denom * curr.prev_iter_value;
		return std::abs(curr.value - curr.prev_value) < EPSILON;
	};
};


bool reset(state_iterator it)
{
	auto temp = *it;

	it->prev_iter_value = it->value;
	return true;
}; 


bool empty(state_iterator it)
{
	return true;
};


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



int main()
{
	Space space;

	start(space, "Example8.txt");

	Stack stack;

	stack.push(1, -1, setCentralValueLeft);
	stack.push(L5 - 1, -1, generator(a5, tau, h));
	 
	stack.push(1, -1, generatorIdealContact(a5, a4, tau, lambda5, lambda4, h, h));

	stack.push(L4 - 1, -1, generator(a4, tau, h));
	stack.push(1, -1, generatorBoundaryLeft(h, lambda4, a4, tau, 1));

	stack.push(1, -1, generatorBoundaryRight(h, lambda3, a3, tau, 1));
	stack.push(L3 - 2, -1, generator(a3, tau, h));
	stack.push(1, -1, generatorBoundaryLeft(h, lambda3, a3, tau, 1));

	stack.push(1, -1, generatorBoundaryRight(h, lambda2, a2, tau, 1));
	stack.push(L2 - 2, -1, generator(a2, tau, h));
	stack.push(1, -1, generatorBoundaryLeft(h, lambda2, a2, tau, 1));

	stack.push(1, -1, generatorBoundaryRight(h, lambda1, a1, tau, 1));
	stack.push(L1 - 2, -1, generator(a1, tau, h));
	stack.push(1, -1, generatorBoundaryLeft(h, lambda1, a1, tau, 1));

	stack.push(1, 0, empty);

	Stack reset_stack;
	reset_stack.push(space.size(), 0, reset);

	auto size = stack.total_size();

	stack.set_reverse_range(space.rbegin(), space.rend());
	reset_stack.set_range(space.rbegin(), space.rend());

	//Полное моделирование

	auto func = [&stack, &reset_stack]()
	{
		double current_time = 0;

		while (current_time < time_end)
		{
			int current_iteration_num = 0;
			bool done = false;
			while (!done)
			{
				done = stack.reverse_recount_until(false);
			};
			reset_stack.recount_until(false);
			std::cout << "Current time: " << current_time << '\n';
			current_time += tau;
		};
	};

	TimeTest<> time_test;
	TimeTest<>::rep executuion_duration = time_test.execute(func);

	std::cout << executuion_duration.count();
	
	char ch;
	std::cin >> ch;

	pause(space, "Example81.txt");

	//----------------------------//

	//----------------------------//

	return 0;
};