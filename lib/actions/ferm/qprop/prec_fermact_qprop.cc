// $Id: prec_fermact_qprop.cc,v 1.9 2004-12-12 21:22:17 edwards Exp $
/*! \file
 *  \brief Propagator solver for a generic even-odd preconditioned fermion operator
 *
 *  Solve for the propagator of a even-odd non-preconditioned fermion operator
 */

#include "chromabase.h"
#include "fermact.h"
#include "invtype.h"
#include "actions/ferm/invert/invcg2.h"

using namespace QDP;
namespace Chroma { 

//! Propagator of a generic even-odd preconditioned fermion linear operator
/*! \ingroup qprop
 *
 * This routine is actually generic to all even-odd preconditioned fermions
 *
 * \param psi      initial solution ( Modify )
 * \param state    gauge field ( Read )
 * \param chi      source ( Read )
 * \param invType  inverter type ( Read (
 * \param RsdCG    CG (or MR) residual used here ( Read )
 * \param MaxCG    maximum number of CG iterations ( Read )
 * \param ncg_had  number of CG iterations ( Write )
 */

template<typename T>
void qprop_t(const EvenOddPrecWilsonTypeFermActBase<T>& me,
	     T& psi, 
	     Handle<const ConnectState> state, 
	     const T& chi, 
	     const InvertParam_t& invParam,
	     int& ncg_had)
{
  START_CODE();

  int n_count;
  
  /* Construct the linear operator */
  /* This allocates field for the appropriate action */
  Handle<const EvenOddPrecLinearOperatorBase<T> > A(me.linOp(state));

  /* Step (i) */
  /* chi_tmp =  chi_o - D_oe * A_ee^-1 * chi_o */
  T chi_tmp;
  {
    T tmp1, tmp2;

    A->evenEvenInvLinOp(tmp1, chi, PLUS);
    A->oddEvenLinOp(tmp2, tmp1, PLUS);
    chi_tmp[rb[1]] = chi - tmp2;
  }

  switch(invParam.invType)
  {
  case CG_INVERTER: 
  {
    /* tmp = A_dag(u) * chi_tmp */
    T  tmp;
    (*A)(tmp, chi_tmp, MINUS);
    
    /* psi = (M^dag * M)^(-1) chi */
    InvCG2(*A, tmp, psi, invParam.RsdCG, invParam.MaxCG, n_count);
  }
  break;
  
#if 0
  case MR_INVERTER:
    /* psi = M^(-1) chi_tmp */
    InvMR(*A, chi_tmp, psi, 
	  invParam.MRover, 
	  invParam.RsdCG, 
	  invParam.MaxCG, n_count);
    break;

  case BICG_INVERTER:
    /* psi = M^(-1) chi_tmp */
    InvBiCG(*A, chi_tmp, psi, 
	    invParam.RsdCG, 
	    invParam.MaxCG, n_count);
    break;
#endif
  
  default:
    QDP_error_exit("Unknown inverter type", invParam.invType);
  }
  
  if ( n_count == invParam.MaxCG )
    QDP_error_exit("no convergence in the inverter", n_count);
  
  ncg_had = n_count;
  
  /* Step (ii) */
  /* psi_e = A_ee^-1 * [chi_e  -  D_eo * psi_o] */
  {
    T tmp1, tmp2;

    A->evenOddLinOp(tmp1, psi, PLUS);
    tmp2[rb[0]] = chi - tmp1;
    A->evenEvenInvLinOp(psi, tmp2, PLUS);
  }
  
  END_CODE();
}


template<>
void 
EvenOddPrecWilsonTypeFermActBase<LatticeFermion>::qpropT(LatticeFermion& psi, 
							 Handle<const ConnectState> state, 
							 const LatticeFermion& chi, 
							 const InvertParam_t& invParam,
							 int& ncg_had) const
{
  qprop_t(*this, psi, state, chi, invParam, ncg_had);
}

template<>
void 
EvenOddPrecWilsonTypeFermActBase<LatticeFermion>::qprop(LatticeFermion& psi, 
							Handle<const ConnectState> state, 
							const LatticeFermion& chi, 
							const InvertParam_t& invParam,
							int& ncg_had) const
{
  qprop_t(*this, psi, state, chi, invParam, ncg_had);
}

}; // namespace Chroma 
