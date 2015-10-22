#include "Examples.h"

/*� ���� ������� ����� ���������� ������ ������� ������� ���������� ��������� ������� ��������
������ �� ��, ��� � � ������� �3 */

namespace Example4{

	struct State;


	using Bundle = Bundle<State>;
	using Layer = Bundle::Layer;
	using Space1D = Bundle::Space1D;
	using Iterator = Bundle::Iterator;
	using recount_func = Bundle::recount_func;
	using size_type = Space1D::size_type;

	//������� ���������, � ������� �������� �������� 
	struct State{
		size_type num;
		double alpha;
		double beta;
		double value;	//current value
		double last_value; //value from previous time step
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
	double t_left = 703;
	double t_right = 303;
	double t_begin = 303;

	//time
	double time_end = 0.2;
	double time_step_num = 1000;
	double tau = time_end / time_step_num;


	//Number of steps

	//size
	double L = 0.2;
	double h = 0.0001;
	size_type N1 = 600;
	size_type N2 = 1600;
	size_type N3 = 2000;

	//
	State init(size_type num)
	{
		if (num == 0)
			return{ num, 0, t_left, t_left, t_left };
		else if (num == N3)
			return{ num, 0, 0, t_right, t_right };
		else
			return{ num, 0, 0, t_begin, t_begin };
	}

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

	//��������� �������� ��������� ����������� � ����
	recount_func generator(double a, double tau, double h)
	{
		double k = a * tau / (h * h);
		return[k](Iterator& arg)->State&
		{
			State& next = *++arg;
			State& curr = *--arg;
			State& prev = *--arg;
			curr.value = (k * (next.value + prev.value) + curr.last_value) / (1 + 2 * k);
			return curr;
		};
	}

	//��������� �������� ��������� ����������� �� ����� �������
	recount_func generatorBoundaryLeft(double h)
	{
		return[h](Iterator& arg)->State&
		{
			State& next = *++arg;
			State& curr = *--arg;
			State& prev = *--arg;
			curr.value = prev.value + sigma * h * (std::pow(next.value, 4) - std::pow(curr.value, 4));
			return curr;
		};
	}

	//��������� �������� ��������� ����������� �� ������ �������
	recount_func generatorBoundaryRight(double h)
	{
		return[h](Iterator& arg)->State&
		{
			State& next = *++arg;
			State& curr = *--arg;
			State& prev = *--arg;
			curr.value = next.value + sigma * h * (std::pow(prev.value, 4) - std::pow(curr.value, 4));
			return curr;
		};
	}

	State& storeNewValues(Iterator& arg)
	{
		State& curr = *arg;
		curr.last_value = curr.value;
		return *arg;
	};
	
	void run()
	{
		Space1D space(N3 + 1, init);
		std::vector<Layer*> v;
		v.push_back(new Layer(space.getIterator(0), space.getIterator(0), constValueForward, empty));
		v.push_back(new Layer(space.getIterator(1), space.getIterator(N1 - 1), generator(a1, lambda1, h), empty));
		v.push_back(new Layer(space.getIterator(N1), space.getIterator(N1), generatorBoundaryLeft(h), empty));
		v.push_back(new Layer(space.getIterator(N1 + 1), space.getIterator(N1 + 1), generatorBoundaryRight(h), empty));
		v.push_back(new Layer(space.getIterator(N1 + 2), space.getIterator(N3 - 1), generator(a2, lambda2, h), empty));
		//v.push_back(new Layer(space.getIterator(N2), space.getIterator(N2), boundaryGeneratorLeft(a2, lambda2, h), dynamicValueBackward));
		//v.push_back(new Layer(space.getIterator(N2 + 1), space.getIterator(N2 + 1), boundaryGeneratorRight(a3, lambda3, h), dynamicValueBackward));
		//v.push_back(new Layer(space.getIterator(N2 + 2), space.getIterator(N3 - 1), generator(a3, h), dynamicValueBackward));
		v.push_back(new Layer(space.getIterator(N3), space.getIterator(N3), constValueForward, empty));
		Layer restorer(space.getIterator(0), space.getIterator(N3), storeNewValues, empty);
		double current_time = 0;
		while (current_time < time_end)
		{
			int i = 0;
			while (i < 100)
			{
				for (auto it = v.begin(); it != v.end(); ++it)
				{
					Layer& curr = **it;
					curr.resetForward();
					while (!curr.forward_recount_step()){};
				};
				++i;
			};
			restorer.resetForward();
			while (!restorer.forward_recount_step()){};
			current_time += tau;
		};

		double distance = 0;
		std::ofstream stream("example4.txt");
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

void runExample4()
{
	Example4::run();
}
