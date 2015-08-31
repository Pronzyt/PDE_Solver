#include "Examples.h"

//Steel plate with left and right boundary conditions of 1st type
//Стальная пластина с левым и правым граничным условием 1го типа

namespace Example1{

struct State;


using BundleEx1 = Bundle<State>;
using LayerEx1 = BundleEx1::Layer;
using Space1DEx1 = BundleEx1::Space1D;
using IteratorEx1 = BundleEx1::Iterator;


//Создаем структуру, в которой хранятся значения 
struct State{
	int num;
	double a;		// lambda / (rho * c )
	double alpha;
	double beta;
	double tau;		//timestep
	double value;	//current value
};


//Number of steps
unsigned N = 100;


//parameters of steel
double lambda = 46;
double rho = 7800;
double c = 460;
double a = lambda / (rho * c);

//init values
double t_left = 300;
double t_right = 100;
double t_begin = 20;

//time
double time_end = 60;
double time_step_num = 100;
double tau = time_end / time_step_num;

//size
double L = 0.1;
double h = L / N;

//constant
double b = a * tau / (2 * h * h);

State init(Space1DEx1::size_type x)
{
	if (x == 0)
		return{x, 0, 0, t_left, 0, t_left };
	else if (x == N)
		return{x, 0, 0, 0, 0, t_right };
	else
		return{x, a, 0, 0, tau, t_begin };
}

//left-side boundary condition (1st type)
State& recountForward1(IteratorEx1& arg)
{
	State& curr = *arg;
	return *arg;
};

//
State& recountBackward1(IteratorEx1& arg)
{
	State& curr = *arg;
	return *arg;
};

//heat transmition in not boundary layer
State& recountForward2(IteratorEx1& arg)
{
//	static const double part = 1 + 2 * b;
//	--arg;
//	State& prev = *arg;
//	double denom = part - b * prev.alpha;
//	double temp = prev.beta + prev.value;
//	State& curr = *++arg;
//	State& next = *++arg; 
//	curr.alpha = b / denom;
//	curr.beta = (b * (temp - 2 * curr.value + next.value) + curr.value) / denom;
//-------------------TEST----------------------------------------------------------
	State& curr = *arg;
	--arg;
	State& prev = *arg;	
	double A  = lambda / (h * h);
	double C = A;
	double B = 2 * lambda / (h * h)  + rho * c / tau;
	double F = - rho * c / tau * curr.value;
	curr.alpha = A / (B - C * prev.alpha);
	curr.beta = (C * prev.beta - F) / (B - C * prev.alpha);
//-----------------------------------------------------------------------------------
	return curr;
};

State& recountBackward2(IteratorEx1& arg)
{
	State& curr = *arg;
	State& prev = *--arg;
	curr.value = curr.alpha * prev.value + curr.beta;
	return curr;
};

//right-side boundary condition
State& recountForward3(IteratorEx1& arg)
{
	State& curr = *arg;
	return *arg;
};

State& recountBackward3(IteratorEx1& arg)
{
	State& curr = *arg;
	return *arg;
};


void run()
{
	Space1DEx1 space(N + 1, init);
	std::vector<LayerEx1*> v;
	v.push_back(new LayerEx1(space.getIterator(0), space.getIterator(0), recountForward1, recountBackward1));
	v.push_back(new LayerEx1(space.getIterator(1), space.getIterator(N - 1), recountForward2, recountBackward2));
	v.push_back(new LayerEx1(space.getIterator(N), space.getIterator(N), recountForward3, recountBackward3));

	double current_time = 0;
	while (current_time < time_end)
	{
		for (auto it = v.begin(); it != v.end(); ++it)
		{
			LayerEx1& curr = **it;
			curr.resetForward();
			while (!curr.forward_recount_step()){};
		};

		for (auto rit = v.rbegin(); rit != v.rend(); ++rit)
		{
			LayerEx1& curr = **rit;
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

void runExample1()
{
	Example1::run();
}