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
	Range<double> range = space.getRange(0, 50);
	auto r = range.to_begin();
//	for(auto r = range.to_begin(); range.in_end(); ++range)
//	{
//		std::cout<<r.getValue();
//	};	
	
//	++range;
	//Holder<double, std::vector<int>::iterator> holder(v.begin(), v.end());
	//range.setRange(v.begin(), v.end());
	
	//Layer<double> layer;
	//layer.setRange(range);
    return 0;
}