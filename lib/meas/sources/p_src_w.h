/*
 *  P-wave fermion source
 */

#ifndef __p_src_h__
#define __p_src_h__

void p_src(const multi1d<LatticeColorMatrix>& u, 
	   LatticeFermion& chi,
	   int direction);

#endif
