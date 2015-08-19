#ifndef BUNDLE_H
#define	BUNDLE_H

#include "Space.h"
#include "Layer.h"


//This class is declared only for readability improvment and will be removed soon
template<typename StateTy>
class Bundle{
	public:
		typedef StateTy state_type;
		typedef Space1D<state_type> Space1D;
		typedef Range<state_type> Range;
		typedef BaseLayer<state_type> BaseLayer;
		typedef Layer<state_type> Layer;
		
		
		
	private:
			
}


#endif	/* BUNDLE_H */

