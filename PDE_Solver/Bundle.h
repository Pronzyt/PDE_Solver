#ifndef BUNDLE_H
#define	BUNDLE_H

#include "Space.h"
#include "Layer.h"


template<typename StateTy>
class Bundle{
	public:
		typedef StateTy							state_type;
		typedef Space1D<state_type>				Space1D;
		typedef Range<state_type>				Range;
		typedef IRange<Range, state_type>		Iterator;
		typedef BaseLayer<state_type>			BaseLayer;
		typedef Layer<state_type>				Layer;
		typedef typename Layer::recount_func	recount_func;
	private:
			
};

#endif	/* BUNDLE_H */