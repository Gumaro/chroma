// -*- C++ -*-
// $Id: barseqsrc_w.h,v 1.4 2005-03-07 02:55:20 edwards Exp $
/*! \file
 *  \brief Construct baryon sequential sources.
 */

#ifndef __barseqsrc_w_h__
#define __barseqsrc_w_h__

namespace Chroma 
{

  LatticePropagator barNuclUUnpol(const LatticePropagator& quark_propagator_1, 
				  const LatticePropagator& quark_propagator_2,
				  const LatticePropagator& quark_propagator_3);

  LatticePropagator barNuclDUnpol(const LatticePropagator& quark_propagator_1, 
				  const LatticePropagator& quark_propagator_2,
				  const LatticePropagator& quark_propagator_3);

  LatticePropagator barNuclUPol(const LatticePropagator& quark_propagator_1, 
				const LatticePropagator& quark_propagator_2,
				const LatticePropagator& quark_propagator_3);

  LatticePropagator barNuclDPol(const LatticePropagator& quark_propagator_1, 
				const LatticePropagator& quark_propagator_2,
				const LatticePropagator& quark_propagator_3);

  LatticePropagator barDeltaUUnpol(const LatticePropagator& quark_propagator_1, 
				   const LatticePropagator& quark_propagator_2,
				   const LatticePropagator& quark_propagator_3);

  LatticePropagator barDeltaDUnpol(const LatticePropagator& quark_propagator_1, 
				   const LatticePropagator& quark_propagator_2,
				   const LatticePropagator& quark_propagator_3);

  LatticePropagator barNuclUUnpolNR(const LatticePropagator& quark_propagator_1, 
				    const LatticePropagator& quark_propagator_2,
				    const LatticePropagator& quark_propagator_3);

  LatticePropagator barNuclDUnpolNR(const LatticePropagator& quark_propagator_1, 
				    const LatticePropagator& quark_propagator_2,
				    const LatticePropagator& quark_propagator_3);

  LatticePropagator barNuclUPolNR(const LatticePropagator& quark_propagator_1, 
				  const LatticePropagator& quark_propagator_2,
				  const LatticePropagator& quark_propagator_3);

  LatticePropagator barNuclDPolNR(const LatticePropagator& quark_propagator_1, 
				  const LatticePropagator& quark_propagator_2,
				  const LatticePropagator& quark_propagator_3);

  LatticePropagator barNuclUMixedNR(const LatticePropagator& quark_propagator_1, 
				    const LatticePropagator& quark_propagator_2,
				    const LatticePropagator& quark_propagator_3);

  LatticePropagator barNuclDMixedNR(const LatticePropagator& quark_propagator_1, 
				    const LatticePropagator& quark_propagator_2,
				    const LatticePropagator& quark_propagator_3);

}  // end namespace Chroma


#endif
