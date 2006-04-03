// $Id: prec_fermact_qprop_array.cc,v 3.0 2006-04-03 04:58:53 edwards Exp $
// $Log: prec_fermact_qprop_array.cc,v $
// Revision 3.0  2006-04-03 04:58:53  edwards
// Major overhaul of fermion and gauge action interface. Basically,
// all fermacts and gaugeacts now carry out  <T,P,Q>  template parameters. These are
// the fermion type, the "P" - conjugate momenta, and "Q" - generalized coordinates
// in the sense of Hamilton's equations. The fermbc's have been rationalized to never
// be over multi1d<T>. The "createState" within the FermionAction is now fixed meaning
// the "u" fields are now from the coordinate type. There are now "ConnectState" that
// derive into FermState<T,P,Q> and GaugeState<P,Q>.
//
// Revision 2.1  2005/10/06 18:09:23  flemingg
// Fixed a malfeature exposed by GCC 4.0.
//
// Revision 2.0  2005/09/25 21:04:30  edwards
// Moved to version 2.0
//
// Revision 1.16  2005/05/18 15:41:56  bjoo
// Speeded up CFZ Op
//
// Revision 1.15  2005/03/02 16:27:15  bjoo
// Removed chi.resize() stuff from LinopArrays
//
// Revision 1.14  2005/02/21 19:28:59  edwards
// Changed initHeader's to be instead the appropriate default constructor
// for the ChiralParam, AnisoParam and QQQ structs. Removed all
// calls to initHeader. Changed quarkProp routines to instead take
// the appropriate SystemSolver instead of fermact. Added AnisoParam
// support for DWF and SSE DWF.
//
// Revision 1.13  2005/01/07 05:00:10  edwards
// Fixed up dwf_fermact to use specialized qpropT. Now, have exposed
// a typedef version of DWFQpropT and the optimized version is defined
// to this if --enable-dwf-cg is on.
//
// Revision 1.12  2005/01/02 05:21:10  edwards
// Rearranged top-level fermion actions and linear operators.
// The deriv is pushed up much higher. This has the effect that
// now the "P" type (conjugate momentum type) is carried around
// in DiffFermAct4D<T,P> and below carry this "P" template param.
// All fermacts now have at least default deriv support (possibly
// Not Implemented error). Made qprop and qpropT now go through
// factory functions.
//
// Revision 1.11  2004/12/29 22:13:41  edwards
// Rearranged the top-level FermionAction structure. Moved up
// 5d-style actions to be a  FermAct5D and there is a FermAct4D.
// Consolidated quarkprop routines and fermact factories.
//
// Revision 1.10  2004/12/12 21:22:17  edwards
// Major overhaul of fermact and linearop class structure. Merged ApproxLinOp
// into LinOp. Removed dsdu stuff - now in linops.  Introduced levels of
// fermacts (base or not) that have derivs. All low level fermacts/linops
// have an additional template param for type of conjugate momenta in
// derivative. Made all fermacts and linops within chroma namespace.
//
// Revision 1.9  2004/11/17 16:52:24  edwards
// Improved some comments
//
// Revision 1.8  2004/10/08 13:20:15  bjoo
// IBM Fixes
//
// Revision 1.7  2004/09/08 02:48:26  edwards
// Big switch-over of fermact IO. New fermact startup mechanism -
// now using Singleton Factory object. Moved  quarkprop4 to be
// a virtual func with top level FermionAction. Disconnected
// zolo4d and 5d fermacts temporarily. Removing usage of old
// fermact and subsequently md,gaugeact  IO mechanisms.
//
// Revision 1.6.2.1  2004/09/02 16:00:07  bjoo
// Trolled beneath the fermact mountains and changed the invocations
// of the inverters to conform to the new inverter interface (invParam -- vs RsdCG, MaxCG and friends) - also in the qprop_files too.
//
// I HAVE REMOVED ZOLOTAREV4D and ZOLOTAREV5D from the build as these
// will need extensive reworking to keep those params inside them
// (simply too many).  I will get them to conform later. Use the
// production branch if you need access to those
//
// I have added the various LOKI based files (Alexandrescu) to the
// Makefile.am so that things install correctly.
//
// I made the propagator code go through the factory invokation to
// do a propagator calculation with prec wilson fermions.
//
// Interestingly the linkage_hack appeared to be optimised away in its
// old form. I turned it into a function that returns the "foo" within,
// so it cannot be compiled away. Interestingly, it still doesn't actually
// need to be CALLED.
//
// Main achievements:
// 	Library compiles
// 	Propagator runs with suitable fermacts (I should check DWF etc)
//
// Revision 1.6  2004/07/28 02:38:02  edwards
// Changed {START,END}_CODE("foo") to {START,END}_CODE().
//
// Revision 1.5  2004/02/05 20:01:47  kostas
// Fixed the chi_tmp bug to all inverters
//
/*! \file
 *  \brief Propagator solver for a generic even-odd preconditioned fermion operator
 *
 *  Solve for the propagator of a even-odd non-preconditioned fermion operator
 */

#include "chromabase.h"
#include "actions/ferm/qprop/prec_fermact_qprop_array.h"
#include "actions/ferm/invert/invcg2_array.h"

namespace Chroma 
{
  //! Propagator of a generic even-odd preconditioned fermion linear operator
  /*!
   * \param psi      quark propagator ( Modify )
   * \param chi      source ( Read )
   * \return number of CG iterations
   */
  template <> 
  int PrecFermAct5DQprop<LatticeFermion, 
			 multi1d<LatticeColorMatrix>,
			 multi1d<LatticeColorMatrix> >::operator()(multi1d<LatticeFermion>& psi, 
								   const multi1d<LatticeFermion>& chi) const
  {
QDPIO::cerr << __PRETTY_FUNCTION__ << ": line = " << __LINE__ << endl;
    START_CODE();

    int n_count;
    const int N5 = size();
  
    if (psi.size() != size() && chi.size() != size())
      QDP_error_exit("PrecFA5DQprop: sizes wrong");

QDPIO::cerr << __PRETTY_FUNCTION__ << ": line = " << __LINE__ << endl;
    /* Step (i) */
    /* chi_tmp_o =  chi_o - D_oe * A_ee^-1 * chi_e */
    multi1d<LatticeFermion> chi_tmp(N5);
    {
      multi1d<LatticeFermion> tmp1(N5);
      multi1d<LatticeFermion> tmp2(N5);

QDPIO::cerr << __PRETTY_FUNCTION__ << ": line = " << __LINE__ << endl;
      A->evenEvenInvLinOp(tmp1, chi, PLUS);
QDPIO::cerr << __PRETTY_FUNCTION__ << ": line = " << __LINE__ << endl;
      A->oddEvenLinOp(tmp2, tmp1, PLUS);
QDPIO::cerr << __PRETTY_FUNCTION__ << ": line = " << __LINE__ << endl;
      for(int n=0; n < N5; ++n)
	chi_tmp[n][rb[1]] = chi[n] - tmp2[n];
QDPIO::cerr << __PRETTY_FUNCTION__ << ": line = " << __LINE__ << endl;
    }
QDPIO::cerr << __PRETTY_FUNCTION__ << ": line = " << __LINE__ << endl;

    QDPIO::cout << "InvType is " << invParam.invType << endl << flush;
    switch(invParam.invType)
    {
    case CG_INVERTER: 
    {
QDPIO::cerr << __PRETTY_FUNCTION__ << ": line = " << __LINE__ << endl;
      /* tmp = M_dag(u) * chi_tmp */
      multi1d<LatticeFermion> tmp(N5);
QDPIO::cerr << __PRETTY_FUNCTION__ << ": line = " << __LINE__ << endl;

      (*A)(tmp, chi_tmp, MINUS);
QDPIO::cerr << __PRETTY_FUNCTION__ << ": line = " << __LINE__ << endl;

      
      /* psi = (M^dag * M)^(-1) chi_tmp */
      InvCG2 (*A, tmp, psi, invParam.RsdCG, invParam.MaxCG, n_count);
    }
    break;
  
#if 0
    case MR_INVERTER:
      /* psi = M^(-1) chi */
      InvMR (*A, chi_tmp, psi, invParam.MRover, invParam.RsdCG, invParam.MaxCG, n_count);
      break;

    case BICG_INVERTER:
      /* psi = M^(-1) chi */
      InvBiCG (*A, chi_tmp, psi, invParam.RsdCG, invParam.MaxCG, n_count);
      break;
#endif
  
    default:
      QDP_error_exit("Unknown inverter type", invParam.invType);
    }
  
    if ( n_count == invParam.MaxCG )
      QDP_error_exit("no convergence in the inverter", n_count);
   
    
    /* Step (ii) */
    /* psi_e = A_ee^-1 * [chi_e  -  D_eo * psi_o] */
 
    {
      multi1d<LatticeFermion> tmp1(N5);
      multi1d<LatticeFermion> tmp2(N5);
      
      A->evenOddLinOp(tmp1, psi, PLUS);
      
      for(int n=0; n < N5; ++n)
	tmp2[n][rb[0]] = chi[n] - tmp1[n];
      

      A->evenEvenInvLinOp(psi, tmp2, PLUS);
      
    }
    
QDPIO::cerr << __PRETTY_FUNCTION__ << ": line = " << __LINE__ << endl;
    END_CODE();

    return n_count;
  }


  typedef LatticeFermion LF;
  typedef multi1d<LatticeColorMatrix> LCM;

  //! Propagator of a generic even-odd preconditioned fermion linear operator
  template<>
  SystemSolverArray<LF>* 
  EvenOddPrecWilsonTypeFermAct5D<LF,LCM,LCM>::qpropT(Handle< FermState<LF,LCM,LCM> > state,
						     const InvertParam_t& invParam) const
  {
QDPIO::cerr << __PRETTY_FUNCTION__ << ": line = " << __LINE__ << endl;
    return new PrecFermAct5DQprop<LF,LCM,LCM>(
      Handle< EvenOddPrecLinearOperatorArray<LF,LCM,LCM> >(linOp(state)), invParam);
  }
  

} // namespace Chroma
