
#ifndef STAG_PROPSHIFT_S_H
#define STAG_PROPSHIFT_S_H

/* Forward declarations for the shifting of the staggered propagator
 * index to co-incide with the corect delta for meson spectroscopy
 */

#include "chromabase.h"

int deltaToPropIndex(multi1d<int>& delta);
void PropIndexTodelta(int src_index, multi1d<int>& delta) ;

LatticeStaggeredPropagator shiftDeltaProp(multi1d<int>& delta, 
					  const LatticeStaggeredPropagator& src);

LatticeStaggeredPropagator 
            shiftDeltaPropCov(multi1d<int>& delta,
			   const LatticeStaggeredPropagator& src,
			      multi1d<LatticeColorMatrix> u, bool sym_flag) ; 

LatticeStaggeredFermion shiftDeltaPropCov(multi1d<int>& delta,
					  const LatticeStaggeredFermion& src,
					  multi1d<LatticeColorMatrix> u, bool sym_flag) ;



LatticeStaggeredPropagator shiftDeltaProp(multi1d<int>& delta,
                                 const LatticeStaggeredPropagator& src, 
					  bool sym_flag) ;

#endif
