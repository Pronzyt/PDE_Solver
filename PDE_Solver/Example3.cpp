#include "Examples.h"



//В этом примере осуществляется попытка повторить вычисления, которые описаны в статье
//Heat transfer in composite materials with Stefan–Boltzmann interface conditions\\ Math. Meth. Appl. Sci. 2008; 31:1297–1314

//На данный момент (20.10.15) получить сходные результаты не удалось

namespace Example3{

	struct State;


	using Bundle = Bundle<State>;
	using Layer = Bundle::Layer;
	using Space1D = Bundle::Space1D;
	using Iterator = Bundle::Iterator;
	using recount_func = Bundle::recount_func;
	using size_type = Space1D::size_type;

	//Создаем структуру, в которой хранятся значения 
	struct State{
		size_type num;
		double alpha;
		double beta;
		double value;	//current value
	};

	//parameters of materials
	double a1 = 4.02e-2;
	double a2 = 1.7e-3;
	double a3 = 9.3e-3;

	double lambda1 = 40;
	double lambda2 = 1.2;
	double lambda3 = 7.3;

	//sigma
	double sigma = 4.88e-4;

	//init values
	double t_left = 1403;
	double t_right = 303;
	double t_begin = 303;

	//time
	double time_end = 0.2;
	double time_step_num = 1000;
	double tau = time_end / time_step_num;


	//Number of steps

	//size
	double L = 0.2;
	double h = 0.001;
	size_type N1 = 60;
	size_type N2 = 160;
	size_type N3 = 200;
	//
	State init(size_type num)
	{
		if (num == 0)
			return{ num,  0, t_left, t_left };
		else if (num == N3)
			return{ num, 0, 0, t_right };
		else
			return{ num, 0, 0, t_begin };
	}

	//left-side boundary condition (1st type)
	State& constValueForward(Iterator& arg)
	{
		State& curr = *arg;
		return *arg;
	};

	State& constValueBackward(Iterator& arg)
	{
		State& curr = *arg;
		return *arg;
	};

	//Возвращает функцию пересчета значений alpha и beta в зависимости от температуропроводности
	//материала и шага в пространстве
	recount_func generator(double a, double lambda, double h)
	{
		return [a, h, lambda](Iterator& arg)->State&
		{
			State& curr = *arg;
			--arg;
			State& prev = *arg;	
			double A  = lambda / (h * h);
			double C = A;
			double B = 2 * lambda / (h * h) + lambda / (a * tau);
			double F = -lambda / (a * tau) * curr.value;
			curr.alpha = A / (B - C * prev.alpha);
			curr.beta = (C * prev.beta - F) / (B - C * prev.alpha);
			return curr;
		};
	};


	State& dynamicValueBackward(Iterator& arg)
	{
		State& curr = *arg;
		State& prev = *--arg;
		curr.value = curr.alpha * prev.value + curr.beta;
		return curr;
	};

	//Возвращает функцию пересчета значений для точек, где значение температуры определяется
	//граничным условием Стефана-Больцмана
	recount_func boundaryGeneratorLeft(double a, double lambda, double h)
	{
		double b = (a * tau) / (h * h); 
		double c = sigma * 2 * h / lambda;
		return [b, c](Iterator& arg)->State&
		{
			--arg;
			State& prev = *arg;
			State& curr = *++arg;
			State& next = *++arg;
			double denom = 1 + 2 * b + b * c * std::pow(curr.value, 3) - 2 * b * prev.alpha;
			curr.alpha = b * c * std::pow(next.value, 3) / denom;
			curr.beta = (2 * b * prev.beta + curr.value) / denom; 
			return curr;
		};
		//double b = sigma * h / lambda;
		//return [b](Iterator& arg)->State&
		//{
		//	--arg;
		//	State& prev = *arg;
		//	State& curr = *++arg;
		//	State& next = *++arg;
		//	double denom = 1 - prev.alpha - b * std::pow(curr.value, 3);
		//	curr.alpha = -b * std::pow(next.value, 3) / denom;
		//	curr.beta = prev.beta / denom; 
		//	return curr;
		//};
	};

	recount_func boundaryGeneratorRight(double a, double lambda, double h)
	{
		double b = (a * tau) / (h * h);
		double c = sigma * 2 * h / lambda;
		return [b, c](Iterator& arg)->State&
		{
			--arg;
			State& prev = *arg;
			State& curr = *++arg;
			double denom = 1 + 2 * b + b * c * std::pow(curr.value, 3) - b * c *  prev.alpha * std::pow(prev.value, 3);
			curr.alpha =  2 * b / denom;
			curr.beta = (b * c * std::pow(prev.value, 3) * prev.beta + curr.value) / denom;
			return curr;
		};
		//double b = sigma * h / lambda;
		//return [b](Iterator& arg)->State&
		//{
		//	--arg;
		//	State& prev = *arg;
		//	State& curr = *++arg;
		//	double denom = -1 - b *  prev.alpha * std::pow(prev.value, 3) + b * std::pow(curr.value, 3);
		//	curr.alpha =  -1 / denom;
		//	curr.beta = (b * std::pow(prev.value, 3) * prev.beta) / denom;
		//	return curr;
		//};
		//double b = a/*as lambda 2*/ / lambda/*as lambda 1*/;
		//return [b](Iterator& arg)->State&
		//{
		//	--arg;
		//	--arg;
		//	State& prevprev = *arg;
		//	State& prev = *++arg;
		//	State& curr = *++arg;
		//	double denom = prev.alpha * (1 - prevprev.alpha) + b;
		//	curr.alpha =  b / denom;
		//	curr.beta = (prevprev.beta - prev.beta * (1 - prevprev.alpha)) / denom;
		//	return curr;
		//};
	};

	void run()
	{
		Space1D space(N3 + 1, init);
		std::vector<Layer*> v;
		v.push_back(new Layer(space.getIterator(0), space.getIterator(0), constValueForward, constValueBackward)); 
		v.push_back(new Layer(space.getIterator(1), space.getIterator(N1 - 1), generator(a1, lambda1, h), dynamicValueBackward));
		v.push_back(new Layer(space.getIterator(N1), space.getIterator(N1), boundaryGeneratorLeft(a1, lambda1, h), dynamicValueBackward));
		v.push_back(new Layer(space.getIterator(N1 + 1), space.getIterator(N1 + 1), boundaryGeneratorRight(a2, lambda2, h), dynamicValueBackward));
		v.push_back(new Layer(space.getIterator(N1 + 2), space.getIterator(N3 - 1), generator(a2, lambda2, h), dynamicValueBackward));
		//v.push_back(new Layer(space.getIterator(N2), space.getIterator(N2), boundaryGeneratorLeft(a2, lambda2, h), dynamicValueBackward));
		//v.push_back(new Layer(space.getIterator(N2 + 1), space.getIterator(N2 + 1), boundaryGeneratorRight(a3, lambda3, h), dynamicValueBackward));
		//v.push_back(new Layer(space.getIterator(N2 + 2), space.getIterator(N3 - 1), generator(a3, h), dynamicValueBackward));
		v.push_back(new Layer(space.getIterator(N3), space.getIterator(N3), constValueForward, constValueBackward));
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
		std::ofstream stream("example3.txt");
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

}

void runExample3()
{
	Example3::run();
}