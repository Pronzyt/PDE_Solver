#ifndef TIMETESTER_H
#define TIMETESTER_H

#include <functional>
#include <chrono>

//https://stackoverflow.com/questions/2808398/easily-measure-elapsed-time

template<typename TimeTy = std::chrono::milliseconds>
class TimeTest{
public:

	typedef TimeTy rep;

	template<typename FuncTy, typename ...Args>
	static rep execute(FuncTy&& func, Args&&... args)
	{
		auto start = std::chrono::system_clock::now();
		std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
		auto duration = std::chrono::duration_cast<TimeTy>(std::chrono::system_clock::now() - start);
		return duration;
	};
};

#endif