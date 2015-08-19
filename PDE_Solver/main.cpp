#include "Space.h"
#include "Layer.h"
#include "Range.h"
#include <iostream>

double init(long unsigned int x)
{
	return x;
}


int main()
{
	Space1D<double> space(100, init);
//	for(auto it = space.begin(); it != space.end(); ++it)
//	{
//		std::cout<<*it<<"\n";
//	};
//	Range<double> range1(space.getRange(0, 50));
//	Range<double> range2(space.getRange(51, 100));
//	range2 = range1;


	for(auto r = space.getRange(0, 10).to_begin(); !r.in_end(); r++)
	{
		std::cout<<r.getValue()<<"\n";
	};	
	
//	++range;
	//Holder<double, std::vector<int>::iterator> holder(v.begin(), v.end());
	//range.setRange(v.begin(), v.end());
	
	//Layer<double> layer;
	//layer.setRange(range);
    return 0;
}