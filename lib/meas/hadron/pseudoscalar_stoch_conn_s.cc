// $Id: pseudoscalar_stoch_conn_s.cc,v 3.1 2007-02-22 21:11:49 bjoo Exp $
#include "chromabase.h"
#include "pseudoscalar_stoch_conn_s.h"
#include "util/gauge/stag_phases_s.h"

namespace Chroma {

// Standard Time Slicery
class TimeSliceFunc : public SetFunc
{
public:
  TimeSliceFunc(int dir): dir_decay(dir) {}
 
  int operator() (const multi1d<int>& coordinate) const {return coordinate[dir_decay];}
  int numSubsets() const {return Layout::lattSize()[dir_decay];}
 
  int dir_decay;
 
private:
  TimeSliceFunc() {}  // hide default constructor
};


  /* README:
   * this code hopefully does the measurements necessary to compute 
   * connected correlators (for 4-link taste singlet pseudoscalar mesons.
   * it does not  compute the correlator itself. Two quantities are 
   * written out for each pair of noise source vectors; call them C1 and C2.
   * the connected correlator should be <<C1(x)*C2(y)>>, where <<>> is an 
   * average over noise sources, This is in contrast to the disconnected
   * correlator, which should be assembled as <<D1(x)*>><<D1(y)>>. Of course
   * and average over configurations is assumed.
   */



void fourlink_pseudoscalar_stoch_conn::compute(
      				    LatticeStaggeredFermion & q_source1,
	       			    LatticeStaggeredFermion & q_source2,
				    LatticeStaggeredFermion & psi1, 
				    LatticeStaggeredFermion & psi2,
				    int isample){
  // Array to describe shifts in cube
  multi1d<int> delta(Nd);

  Set timeslice;
  timeslice.make(TimeSliceFunc(Nd-1));

  LatticeStaggeredFermion psi_eta4_1 ;
  LatticeStaggeredFermion psi_eta4_2 ;

  delta = 0;
  delta[0] = delta[1] = delta[2] = delta[3] = 1;
 
  psi_eta4_1 = shift_deltaProp(delta, psi1);
  psi_eta4_2 = shift_deltaProp(delta, psi2);

  LatticeComplex TrG_eta4 ; 

  using namespace StagPhases;

  TrG_eta4 = 
    -alpha(0)*alpha(1)*alpha(2)*alpha(3)*localInnerProduct(q_source1, 
							   psi_eta4_2);

  corr_fn1[isample] = sumMulti(TrG_eta4, timeslice);
  corr1 += corr_fn1[isample] ;

  TrG_eta4 = 
    -alpha(0)*alpha(1)*alpha(2)*alpha(3)*localInnerProduct(q_source2, 
							   psi_eta4_1);
  corr_fn2[isample] = sumMulti(TrG_eta4, timeslice);
  corr2 += corr_fn2[isample] ;
}


}  // end namespace Chroma
