#include "Space.h"
#include "Layer.h"

double init(long unsigned int x)
{
	return x;
}


int main()
{
	Layer<double>();
	Space1D<double> space(100, init);
    return 0;
}