#include "Examples.h"

//Brick endless cylinder with surface boundary conditions of 1st type
//Кирпичный бесконечный цилиндр с поверхностным граничным условием первого типа

namespace Example2{

struct State;


using Bundle = Bundle<State>;
using Layer = Bundle::Layer;
using Space1D = Bundle::Space1D;
using Iterator = Bundle::Iterator;


//Создаем структуру, в которой хранятся значения 
struct State{
	Space1D::size_type num;
	double a;		// lambda / (rho * c )
	double alpha;
	double beta;
	double tau;		//timestep
	double value;	//current value
};

//Number of steps
unsigned N = 100;


//parameters of steel
double lambda = 0.7;
double rho = 1500;
double c = 750;
double a = lambda / (rho * c);

//init values
double t_r = 50;
double t_begin = 20;

//time
double time_end = 3600;
double time_step_num = 10000;
double tau = time_end / time_step_num;

//size
double L = 0.1;
double h = L / N;

//constant
double b = a * tau / (2 * h * h);

State init(Space1D::size_type x)
{
	if (x == 0)
		return{x, a, 1, 0, tau,  t_begin };
	else if (x == N)
		return{x, 0, 0, 0, tau, t_r};
	else
		return{x, a, 0, 0, tau, t_begin };
}


//left-side boundary condition (1st type)
State& recountForward1(Iterator& arg)
{
	State& curr = *arg;
	return *arg;
};

//
State& recountBackward1(Iterator& arg)
{
	State& curr = *arg;
	State& prev = *--arg;
	curr.value = curr.alpha * prev.value + curr.beta;
	return curr;
};

//heat transmission in not boundary layer
State& recountForward2(Iterator& arg)
{
	static const double part = 1 + 2 * b;
	State& prev = *(--arg);
	State& curr = *(++arg);
	State& next = *(++arg);
	double denom = part - b * prev.alpha * (1 - 1 / (2 * curr.num));
	curr.alpha = b * (1 + 1 / (2 * curr.num)) / denom;
	double F1 = curr.value + b * (next.value * (1 + 1 / (2 * curr.num)) - 2 * curr.value + prev.value *(1 - 1 / (2 * curr.num)));
	double F2 = b * prev.beta * (1 - 1 / (2 * curr.num)) + F1;
	curr.beta = F2 / denom;
//-------------------TEST----------------------------------------------------------
	State& prev = *(--arg);
	State& curr = *(++arg);
	State& next = *(++arg);
	double r_l = (prev.num * h + curr.num * h) / 2;
	double r_r = (curr.num * h + next.num * h) / 2;
	double r = h * curr.num;
	double A = lambda / (h * h) * r_r / r;
	double B = lambda / (h * h) * (r_l + r_r) / r + rho * c / tau;
	double C = lambda / (h * h) * r_l / r;
	double F = - rho * c / tau * curr.value;
//-----------------------------------------------------------------------------------
	return curr;
};

State& recountBackward2(Iterator& arg)
{
	State& curr = *arg;
	State& prev = *--arg;
	curr.value = curr.alpha * prev.value + curr.beta;
	return curr;
};

//right-side boundary condition
State& recountForward3(Iterator& arg)
{
	State& curr = *arg;
	return *arg;
};

State& recountBackward3(Iterator& arg)
{
	State& curr = *arg;
	return *arg;
};


void run()
{
	Space1D space(N + 1, init);
	std::vector<Layer*> v;
	v.push_back(new Layer(space.getIterator(0), space.getIterator(0), recountForward1, recountBackward1));
	v.push_back(new Layer(space.getIterator(1), space.getIterator(N - 1), recountForward2, recountBackward2));
	v.push_back(new Layer(space.getIterator(N), space.getIterator(N), recountForward3, recountBackward3));

	double current_time = 0;
	while (current_time < time_end)
	{
		for (auto it = v.begin(); it != v.end(); ++it)
		{
			Layer& curr = **it;
			curr.resetForward();
			while (!curr.forward_recount_step()){};
		};

		for (auto rit = v.rbegin(); rit != v.rend(); ++rit)
		{
			Layer& curr = **rit;
			curr.resetBackward();
			while (!curr.backward_recount_step()){};
		};		
		current_time += tau;
	};

	double distance = 0;
	std::ofstream stream("test.txt");
	std::stringstream sstream;
	for (auto it = space.begin(); it != space.end(); ++it)
	{
		sstream << distance << " " << it->value << "\n";
		stream << sstream.str();
		sstream.str(std::string());
		sstream.clear();
		//stream << std::to_string(distance) << " " << std::to_string(it->value) << "\n";
		distance += h;
	};
	stream.close();
};

};


void runExample2()
{
	Example2::run();
}