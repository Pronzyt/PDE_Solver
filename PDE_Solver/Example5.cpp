#include "Examples.h"


/*В этом примере будет рассмотрен способ решения системы нелинейных уравнений методом итерации
Задача та же, что и в примере №3 (изменена постоянная sigma в соответствии с законом)
----------------------------------------------------------------------------------------------
В этом примере будет произведена оценка точности решения, а постоянное количество итераций будет
заменено проверкой сходимости*/

namespace Example5{

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
		double prev_value;
		double last_value; //value from previous time step
		double error;	//error 
	};

	//parameters of materials
	double a1 = 4.02e-2;
	double a2 = 1.7e-3;
	double a3 = 9.3e-3;

	double lambda1 = 40;
	double lambda2 = 1.2;
	double lambda3 = 7.3;

	//sigma
	double sigma = 5.67e-8;

	//init values
	double t_left = 1403;
	double t_right = 303;
	double t_begin = 303;

	//time
	double time_end = 50;
	double time_step_num = 1000;
	double tau = time_end / time_step_num;


	//Number of steps

	//size
	double L = 0.2;
	double h = 0.001;
	size_type N1 = 60;
	size_type N2 = 160;
	size_type N3 = 200;


	const double MAX_ITERATION_NUM = 100;
	const double EPSILON = 0.1;

	//
	State init(size_type num)
	{
		if (num == 0)
			return{ num, t_left, t_left, t_left, 0 };
		else if (num == N3)
			return{ num, t_right, t_right, t_right, 0 };
		else
			return{ num, t_begin, t_begin, t_begin, 0 };
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
	recount_func generator(double a, double h)
	{
		double k = a * tau / (h * h);
		return[k](Iterator& arg)->State&
		{
			State& next = *++arg;
			State& curr = *--arg;
			State& prev = *--arg;
			curr.prev_value = curr.value;
			curr.value = (k * (next.value + prev.value) + curr.last_value) / (1 + 2 * k);
			double test = std::abs(curr.value - prev.value);
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
			curr.value = prev.value + sigma * h * (std::pow(next.value, 4) - std::pow(curr.value, 4)) / lambda;
			double test = std::abs(curr.value - prev.value);
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
			curr.value = next.value + sigma * h * (std::pow(prev.value, 4) - std::pow(curr.value, 4)) / lambda;
			double test = std::abs(curr.value - prev.value);
			return curr;
		};
	};


	State& storeNewValues(Iterator& arg)
	{
		State& next = *++arg;
		State& curr = *--arg;
		State& prev = *--arg;
		curr.last_value = curr.value;
		return *arg;
	};


	//Оценка происходит путем подстановки полученных значений в систему
	//и вычисления модуля отклонения

	recount_func generatorError(double a, double h)
	{
		double k = a * tau / (h * h);
		return[k](Iterator& arg)->State&
		{
			State& next = *++arg;
			State& curr = *--arg;
			State& prev = *--arg;
			curr.error = std::abs(curr.value - (k * (next.value + prev.value) + curr.last_value) / (1 + 2 * k));
			return curr;
		};
	};

	recount_func generatorBoundaryLeftError(double h, double lambda)
	{
		return[h, lambda](Iterator& arg)->State&
		{
			State& next = *++arg;
			State& curr = *--arg;
			State& prev = *--arg;
			curr.error = std::abs(curr.value - (prev.value + sigma * h * (std::pow(next.value, 4) - std::pow(curr.value, 4)) / lambda));
			return curr;
		};
	};
	
	recount_func generatorBoundaryRightError(double h, double lambda)
	{
		return[h, lambda](Iterator& arg)->State&
		{
			State& next = *++arg;
			State& curr = *--arg;
			State& prev = *--arg;
			curr.error = std::abs(curr.value - (next.value + sigma * h * (std::pow(prev.value, 4) - std::pow(curr.value, 4)) / lambda));
			return curr;
		};
	};

	void run()
	{
		Space1D space(N3 + 1, init);
		std::vector<Layer*> recounter;

		//Слои для пересчета значения 
		recounter.push_back(new Layer(space.getIterator(0), space.getIterator(0), constValueForward, empty));
		recounter.push_back(new Layer(space.getIterator(1), space.getIterator(N1 - 1), generator(a1, h), empty));
		recounter.push_back(new Layer(space.getIterator(N1), space.getIterator(N1), generatorBoundaryLeft(h, lambda1), empty));
		recounter.push_back(new Layer(space.getIterator(N1 + 1), space.getIterator(N1 + 1), generatorBoundaryRight(h, lambda2), empty));
		recounter.push_back(new Layer(space.getIterator(N1 + 2), space.getIterator(N2 - 1), generator(a2, h), empty));
		recounter.push_back(new Layer(space.getIterator(N2), space.getIterator(N2), generatorBoundaryLeft(h, lambda2), empty));
		recounter.push_back(new Layer(space.getIterator(N2 + 1), space.getIterator(N2 + 1), generatorBoundaryRight(h, lambda3), empty));
		recounter.push_back(new Layer(space.getIterator(N2 + 2), space.getIterator(N3 - 1), generator(a3, h), empty));
		recounter.push_back(new Layer(space.getIterator(N3), space.getIterator(N3), constValueForward, empty));

		//Слои для оценки ошибки
		std::vector<Layer*> error_estimator;
		error_estimator.push_back(new Layer(space.getIterator(1), space.getIterator(N1 - 1), generatorError(a1, h), empty));
		error_estimator.push_back(new Layer(space.getIterator(N1), space.getIterator(N1), generatorBoundaryLeftError(h, lambda1), empty));
		error_estimator.push_back(new Layer(space.getIterator(N1 + 1), space.getIterator(N1 + 1), generatorBoundaryRightError(h, lambda2), empty));
		error_estimator.push_back(new Layer(space.getIterator(N1 + 2), space.getIterator(N2 - 1), generatorError(a2, h), empty));
		error_estimator.push_back(new Layer(space.getIterator(N2), space.getIterator(N2), generatorBoundaryLeftError(h, lambda2), empty));
		error_estimator.push_back(new Layer(space.getIterator(N2 + 1), space.getIterator(N2 + 1), generatorBoundaryRightError(h, lambda3), empty));
		error_estimator.push_back(new Layer(space.getIterator(N2 + 2), space.getIterator(N3 - 1), generatorError(a3, h), empty));
		
		//Один общий слой для сохранения значений
		Layer restorer(space.getIterator(1), space.getIterator(N3 - 1), storeNewValues, empty);

		double current_time = 0;
		while (current_time < time_end)
		{
			int i = 0;
			while (i < 1)
			{
				for (auto it = recounter.begin(); it != recounter.end(); ++it)
				{
					Layer& curr = **it;
					curr.resetForward();
					while (!curr.forward_recount_step()){};
				};
				++i;
			};

			for (auto it = error_estimator.begin(); it != error_estimator.end(); ++it)
			{
				Layer& curr = **it;
				curr.resetForward();
				while (!curr.forward_recount_step()){};
			};

			restorer.resetForward();
			while (!restorer.forward_recount_step()){};

			current_time += tau;
			std::cout << current_time << "\n";
		};

		double distance = 0;
		std::ofstream stream("example5.txt");
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

void runExample5()
{
	Example5::run();
}

