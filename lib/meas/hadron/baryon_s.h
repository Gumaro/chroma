#ifndef baryon_s_h
#define baryon_s_h

#include "chromabase.h"

void baryon_s(LatticeStaggeredPropagator & quark_propagator_in, 
	      multi1d<Complex> & barprop,
	      multi1d<int> & t_source,
	      int j_decay, int bc_spec) ;

#endif
