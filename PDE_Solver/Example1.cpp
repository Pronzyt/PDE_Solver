#include <vector>
#include <string>
#include <fstream>
#include "Bundle.h"


//Steel plate with left and right boundary conditions of 1st type
//Стальная пластина с левым и правым граничным условием 1го типа


//Создаем структуру, в которой хранятся значения 
struct State{
	double a;		// lambda / (rho * c )
	double alpha;
	double beta;
	double tau;		//timestep
	double value;	//current value
};

using BundleEx1 = Bundle<State>;
using LayerEx1 = BundleEx1::Layer;
using Space1DEx1 = BundleEx1::Space1D;
using IteratorEx1 = BundleEx1::Iterator;


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
		return{ 0, 0, t_left, 0, t_left };
	else if (x == N - 1)
		return{ 0, 0, 0, 0, t_right };
	else
		return{ a, 0, 0, tau, t_begin };
}

//left-side boundary condition (1st type)
State& recountForward1(IteratorEx1& arg)
{
	return *arg;
};
//
State& recountBackward1(IteratorEx1& arg)
{
	return *arg;
};

//heat transmition in not boundary layer
State& recountForward2(IteratorEx1& arg)
{
	static const double part = 1 + 2 * b;
	--arg;
	State& prev = *arg;
	double denom = part - b * prev.alpha;
	double temp = prev.beta + prev.value;
	State& curr = *++arg;
	curr.alpha = b / denom;
	curr.beta = (b * (temp - 2 * curr.value + (*++arg).value) + curr.value) / denom;
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
	return *arg;
};

State& recountBackward3(IteratorEx1& arg)
{
	return *arg;
};


void runExample1()
{
	Space1DEx1 space(N + 1, init);
	std::vector<LayerEx1*> v;
	v.push_back(new LayerEx1(space.getIterator(0), space.getIterator(0), recountForward1, recountBackward1));
	v.push_back(new LayerEx1(space.getIterator(1), space.getIterator(N - 1), recountForward2, recountBackward2));
	v.push_back(new LayerEx1(space.getIterator(N), space.getIterator(N), recountForward3, recountBackward3));

	for (auto it = v.begin(); it != v.end(); ++it)
	{
		LayerEx1& curr = **it;
		while (!curr.forward_recount_step()){};
	};

	for (auto rit = v.rbegin(); rit != v.rend(); ++rit)
	{
		LayerEx1& curr = **rit;
		while (!curr.backward_recount_step()){};
	};

	//double distance = 0;
	//std::ofstream stream("test.txt");
	//for (auto it = space.begin(); it != space.end(); ++it)
	//{
	//	stream << std::to_string(distance) << " " << std::to_string(it->value) << "\n";
	//	distance += h;
	//};

	//stream.close();

};


int main()
{
	runExample1();
	return 0;
}






