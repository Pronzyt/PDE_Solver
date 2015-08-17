#include "Space.h"
#include "Layer.h"
#include "Range.h"

double init(long unsigned int x)
{
	return x;
}


int main()
{
	Range<double> range;
	std::vector<double> v;
	//Holder<double, std::vector<int>::iterator> holder(v.begin(), v.end());
	range.setRange(v.begin(), v.end());
	//Layer<double>();
	//Space1D<double> space(100, init);
    return 0;
}