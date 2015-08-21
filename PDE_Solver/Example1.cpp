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

State init(Space1DEx1::size_type x)
{
	if (x == 0)
		return {0, 0, t_left, 0, t_left};
	else if (x == N - 1)
		return {0, 0, 0, 0, t_right};
	else
		return {a, 0, 0, tau, t_begin};
}


void runExample1()
{
	Space1DEx1 space(N, init);	
	std::vector<LayerEx1*> v = std::vector<LayerEx1*>(3);
//	v()
	
	
	
	
};
		
		





