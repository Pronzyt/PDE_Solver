#include "Examples.h"


//В этом примере будет произведено решение уравнения Пуассона для сложого одномерного теплового объекта
//(печи с двуслойной футеровкой). В отличие от предыдущего примера, дискретизация нелинейного граничного условия
//(излучение на границе) будет произведена с помощью ghost-point метода второго порядка точности.


namespace Example6{

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
		double value;	//current value
		double prev_value;	//value from previous iteration step
		double last_value; //value from previous time step
		bool is_enough;	//this flag is using to stop iterations
		double error;	//error 
	};

	//parameters of materials

	double lambda1 = 47;
	double lambda2 = 2;
	double lambda3 = 2;

	//sigma
	double sigma = 5.67e-8;

	//init values
	double t_left = 60;
	double t_right = 2100;
	double t_begin = 60;

	//Number of steps

	double h = 0.0001;
	size_type N1 = 30;
	size_type N2 = 230;
	size_type N3 = 480;

	const double MAX_ITERATION_NUM = 100000;
	const double EPSILON = 0.01;

	//
	State init(size_type num)
	{
		if (num == 0)
			return{ num, t_left, t_left, t_left, true, 0 };
		else if (num == N3)
			return{ num, t_right, t_right, t_right, true, 0 };
		else
			return{ num, t_begin, t_begin, t_begin, false, 0 };
	};

	State& empty(Iterator& arg)
	{
		State& curr = *arg;
		return *arg;
	};

	State& constValueForward(Iterator& arg)
	{
		State& curr = *arg;
		return *arg;
	};

	//Генератор итераций пересчета температуры в слое
	recount_func generator()
	{
		return[](Iterator& arg)->State&
		{
			State& next = *++arg;
			State& curr = *--arg;
			State& prev = *--arg;
			curr.prev_value = curr.value;
			curr.value = 1 / (4 * curr.num) * (next.value - prev.value) + (next.value + prev.value) / 2;
			curr.is_enough = (std::abs(curr.value - curr.prev_value) < EPSILON) && prev.is_enough;
			return curr;
		};
	};

	//Генератор итераций пересчета температуры на левой границе
	recount_func generatorBoundaryLeft(double h, double lambda)
	{
		return[h, lambda](Iterator& arg)->State&
		{
			State& next = *++arg;
			State& curr = *--arg;
			State& prev = *--arg;
			curr.prev_value = curr.value;
			curr.value = (2 * sigma * h / lambda * (std::pow(next.value, 4) - std::pow(curr.value, 4)) + prev.value) * (1 / (4 * curr.num) + 0.5)
				+ prev.value * (0.5 - 1 / (4 * curr.num));
			curr.is_enough = (std::abs(curr.value - curr.prev_value) < EPSILON) && prev.is_enough;
			return curr;
		};
	};

	//Генератор итераций пересчета температуры на правой границе
	recount_func generatorBoundaryRight(double h, double lambda)
	{
		return[h, lambda](Iterator& arg)->State&
		{
			State& next = *++arg;
			State& curr = *--arg;
			State& prev = *--arg;
			curr.prev_value = curr.value;
			curr.value = next.value * (1 / (4 * curr.num) + 0.5) +
				(2 * sigma * h / lambda * (std::pow(prev.value, 4) - std::pow(curr.value, 4)) + next.value) * (0.5 - 1 / (4 * curr.num));
			curr.is_enough = (std::abs(curr.value - curr.prev_value) < EPSILON) && prev.is_enough;
			return curr;
		};
	};

	//Оценка происходит путем подстановки полученных значений в систему
	//и вычисления модуля отклонения

	//recount_func generatorError(double a, double h)
	//{
	//	return[k](Iterator& arg)->State&
	//	{
	//		State& next = *++arg;
	//		State& curr = *--arg;
	//		State& prev = *--arg;
	//		curr.error = std::abs(curr.value - (k * (next.value + prev.value) + curr.last_value) / (1 + 2 * k));
	//		return curr;
	//	};
	//};

	//recount_func generatorBoundaryLeftError(double h, double lambda)
	//{
	//	return[h, lambda](Iterator& arg)->State&
	//	{
	//		State& next = *++arg;
	//		State& curr = *--arg;
	//		State& prev = *--arg;
	//		curr.error = std::abs(curr.value - (prev.value + sigma * h * (std::pow(next.value, 4) - std::pow(curr.value, 4)) / lambda));
	//		return curr;
	//	};
	//};

	//recount_func generatorBoundaryRightError(double h, double lambda)
	//{
	//	return[h, lambda](Iterator& arg)->State&
	//	{
	//		State& next = *++arg;
	//		State& curr = *--arg;
	//		State& prev = *--arg;
	//		curr.error = std::abs(curr.value - (next.value + sigma * h * (std::pow(prev.value, 4) - std::pow(curr.value, 4)) / lambda));
	//		return curr;
	//	};
	//};

	void run()
	{
		Space1D space(N3 + 1, init);
		std::vector<Layer*> recounter;

		//Слои для пересчета значения 
		recounter.push_back(new Layer(space.getIterator(1), space.getIterator(1), generatorBoundaryRight(h, lambda1), empty));
		recounter.push_back(new Layer(space.getIterator(2), space.getIterator(N1 - 1), generator(), empty));
		recounter.push_back(new Layer(space.getIterator(N1), space.getIterator(N1), generatorBoundaryLeft(h, lambda1), empty));
		recounter.push_back(new Layer(space.getIterator(N1 + 1), space.getIterator(N1 + 1), generatorBoundaryRight(h, lambda2), empty));
		recounter.push_back(new Layer(space.getIterator(N1 + 2), space.getIterator(N2 - 1), generator(), empty));
		recounter.push_back(new Layer(space.getIterator(N2), space.getIterator(N2), generatorBoundaryLeft(h, lambda2), empty));
		recounter.push_back(new Layer(space.getIterator(N2 + 1), space.getIterator(N2 + 1), generatorBoundaryRight(h, lambda3), empty));
		recounter.push_back(new Layer(space.getIterator(N2 + 2), space.getIterator(N3 - 2), generator(), empty));
		recounter.push_back(new Layer(space.getIterator(N3 - 1), space.getIterator(N3 - 1), generatorBoundaryLeft(h, lambda3), empty));

		//Слои для оценки ошибки
		//std::vector<Layer*> error_estimator;
		//error_estimator.push_back(new Layer(space.getIterator(1), space.getIterator(N1 - 1), generatorError(a1, h), empty));
		//error_estimator.push_back(new Layer(space.getIterator(N1), space.getIterator(N1), generatorBoundaryLeftError(h, lambda1), empty));
		//error_estimator.push_back(new Layer(space.getIterator(N1 + 1), space.getIterator(N1 + 1), generatorBoundaryRightError(h, lambda2), empty));
		//error_estimator.push_back(new Layer(space.getIterator(N1 + 2), space.getIterator(N2 - 1), generatorError(a2, h), empty));
		//error_estimator.push_back(new Layer(space.getIterator(N2), space.getIterator(N2), generatorBoundaryLeftError(h, lambda2), empty));
		//error_estimator.push_back(new Layer(space.getIterator(N2 + 1), space.getIterator(N2 + 1), generatorBoundaryRightError(h, lambda3), empty));
		//error_estimator.push_back(new Layer(space.getIterator(N2 + 2), space.getIterator(N3 - 1), generatorError(a3, h), empty));

		bool& term_cond = space.getIterator(N3 - 1)->is_enough;
		int i = 0;
		while ((i < MAX_ITERATION_NUM) && !term_cond)
		{
			for (auto it = recounter.rbegin(); it != recounter.rend(); ++it)
			{
				Layer& curr = **it;
				curr.resetForward();
				while (!curr.forward_recount_step()){};
			};
			++i;
			std::cout << " Number of iterations: " << i << "\n";
		};

		//for (auto it = error_estimator.begin(); it != error_estimator.end(); ++it)
		//{
		//	Layer& curr = **it;
		//	curr.resetForward();
		//	while (!curr.forward_recount_step()){};
		//};

		double distance = 0;
		std::ofstream stream("example6.txt");
		std::stringstream sstream;
		for (auto it = space.begin(); it != space.end(); ++it)
		{
			sstream << "Distance: " << distance << " Temperature: " << it->value << " Error: " << it->error << "\n";
			stream << sstream.str();
			sstream.str(std::string());
			sstream.clear();
			//stream << std::to_string(distance) << " " << std::to_string(it->value) << "\n";
			distance += h;
		};
		stream.close();
	};
};

void runExample6()
{
	Example6::run();
}

