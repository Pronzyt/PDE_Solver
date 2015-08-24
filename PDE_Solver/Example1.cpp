#include <vector>
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
	State* prev;
	State* next;
};

using BundleEx1 =  Bundle<State>;
using LayerEx1 = BundleEx1::Layer;
using Space1DEx1 = BundleEx1::Space1D;


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
double denom = 1 + 2 * b; 


State init(Space1DEx1::size_type x)
{
	if (x == 0)
		return {0, 0, t_left, 0, t_left, 0, 0};
	else if (x == N - 1)
		return {0, 0, 0, 0, t_right, 0, 0};
	else
		return {a, 0, 0, tau, t_begin, 0, 0};
}

//left-side boundary condition (1st type)
State& recountForward1(State& arg)
{
//	prev = &arg;
	return arg;
};

State& recountBackward1(State& arg)
{
	return arg;
}; 

//heat transmition in not boundary layer
State& recountForward2(State& arg)
{
	arg.alpha = b / (denom - b * arg.alpha);
	
	return arg;
};

State& recountBackward2(State& arg)
{
	return arg;
};

//right-side boundary condition
State& recountForward3(State& arg)
{
	return arg;
};

State& recountBackward3(State& arg)
{
	return arg;
};


void runExample1()
{
	Space1DEx1 space(N, init);	
	std::vector<LayerEx1*> v = std::vector<LayerEx1*>(1);
	Bundle<State>::Space1D space1 = Bundle<State>::Space1D(N, init);
	v.push_back(new LayerEx1(space.getRange(0, 0), recountForward1, recountBackward1)); 
	v.push_back(new LayerEx1(space.getRange(1, N - 2), recountForward2, recountBackward2)); 
	v.push_back(new LayerEx1(space.getRange(N - 1, N - 1), recountForward3, recountBackward3)); 
};


int main()
{
	runExample1();
	return 0;
}
		





