// $Id: util_compute_meson_s.cc,v 2.2 2006-02-02 16:25:00 egregory Exp $
/*! \file
 * \brief Wrapper code to compute staggered meson correlators.
 *
 * Spectrum calculations
 */

#include "handle.h"
#include "actions/ferm/fermbcs/fermbcs.h"
#include "actions/ferm/fermacts/fermacts_s.h"
#include "meas/hadron/hadron_s.h"
#include "meas/smear/fuzz_smear.h"
#include "meas/sources/srcfil.h"
#include "util/ferm/transf.h"
#include "meas/hadron/pion_local_s.h"


#include "util_compute_quark_prop_s.h"

namespace Chroma 
{ 


  void compute_vary_ps(
		       LatticeStaggeredPropagator & quark_propagator_Lsink_Lsrc,
		       LatticeStaggeredPropagator & quark_propagator_Fsink_Lsrc,
		       LatticeStaggeredPropagator & quark_propagator_Lsink_Fsrc,
		       LatticeStaggeredPropagator & quark_propagator_Fsink_Fsrc,
		       const multi1d<LatticeColorMatrix> & u , 
		       XMLWriter & xml_out,
		       int j_decay,int t_length, int t_source)
{


  printf("begin pion\n");fflush(stdout);
  staggered_local_pion pion(t_length,u) ;
  printf("done pion\n");fflush(stdout);

push(xml_out,"Meson_correlators");

  // ---------- LL ----------
  pion.compute(quark_propagator_Lsink_Lsrc,
		  quark_propagator_Lsink_Lsrc,j_decay);
  push(xml_out, "Lsink_Lsrc");
  pion.dump(t_source,xml_out);
  pop(xml_out);
printf("a little dumping\n");fflush(stdout);
  
  // ---------- FL ----------
  pion.compute(quark_propagator_Lsink_Lsrc,
		  quark_propagator_Fsink_Lsrc,j_decay);
  push(xml_out, "Fsink_Lsrc");
  pion.dump(t_source,xml_out);
  pop(xml_out);

printf("some dumping\n");fflush(stdout);
  
  // ---------- LF ----------
  pion.compute(quark_propagator_Lsink_Lsrc,
		  quark_propagator_Lsink_Fsrc,j_decay);
  push(xml_out, "Lsink_Fsrc");
  pion.dump(t_source,xml_out);
  pop(xml_out);
  

  // ---------- FF ----------
  pion.compute(quark_propagator_Lsink_Lsrc,
		  quark_propagator_Fsink_Fsrc,j_decay);
  push(xml_out, "Fsink_Fsrc");
  pion.dump(t_source,xml_out);
  pop(xml_out);

  // ---------------------------------------
  pop(xml_out);

printf("done dumping\n");fflush(stdout);

  QDPIO::cout << "Computed variational fuzzed mesons"  << endl;

}



} // end of namespace
