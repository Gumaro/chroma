// $Id: prec_dwf_fermact_base_w.cc,v 1.3 2003-12-02 15:45:04 edwards Exp $
/*! \file
 *  \brief Base class for even-odd preconditioned domain-wall-like fermion actions
 */

#include "chromabase.h"
#include "actions/ferm/fermacts/prec_dwf_fermact_base_w.h"
#include "actions/ferm/invert/invcg2.h"
#include "actions/ferm/linop/dwffld_w.h"

using namespace QDP;

//! Propagator of an even-odd preconditioned DWF linear operator
/*!
 *
 * \param psi      quark propagator ( Modify )
 * \param state    gauge field ( Read )
 * \param chi      source ( Read )
 * \param invType  inverter type ( Read (
 * \param RsdCG    CG (or MR) residual used here ( Read )
 * \param MaxCG    maximum number of CG iterations ( Read )
 * \param ncg_had  number of CG iterations ( Write )
 */

void 
EvenOddPrecDWFermActBase::qprop(LatticeFermion& psi, 
				const ConnectState& state, 
				const LatticeFermion& chi, 
				enum InvType invType,
				const Real& RsdCG, 
				int MaxCG, int& ncg_had) const
{
  START_CODE("EvenOddPrecDWFermActBase::qprop");

  const Real m_q = quark_mass();

  int n_count;
  
  // Initialize the 5D fields
  //  tmp5 = (chi,0,0,0,..,0)^T
  LatticeDWFermion chi5, tmp5 = zero;
  pokeDW(tmp5, chi, 0);

  // chi5 = P . tmp5
  DwfFld(chi5, tmp5, PLUS);

  // tmp5 = D5(1) . chi5 =  D5(1) . P . (chi,0,0,..,0)^T 
  {
    // Create a Pauli-Villars linop and use it for just this part
    const LinearOperatorProxy<LatticeDWFermion> B(linOpPV(state));

    B(tmp5, chi5, PLUS);
  }

  //  psi5 = (psi,0,0,0,...,0)^T
  LatticeDWFermion psi5 = zero;
  pokeDW(psi5, psi, 0);

  QDPIO::cout << "|psi5|^2 = " << norm2(psi5) << endl;
  QDPIO::cout << "|chi5|^2 = " << norm2(chi5) << endl;

  // Construct the linear operator
  const EvenOddPrecLinearOperatorProxy<LatticeDWFermion> A(linOp(state));

  /* Step (i) */
  /* chi5 = L^(-1) * tmp5 = [ tmp5_o - D_oe*A_ee^-1*tmp5_o ] */
  {
    LatticeDWFermion tmp1, tmp2;

    A.evenEvenInvLinOp(tmp1, tmp5, PLUS);
    A.oddEvenLinOp(tmp2, tmp1, PLUS);
    chi5[rb[0]] = tmp5;
    chi5[rb[1]] = tmp5 - tmp2;
  }


  switch(invType)
  {
  case CG_INVERTER: 
    // tmp5 = D5^\dagger(m) . chi5 =  D5^dagger(m) . L^-1 . D5(1) . P . (chi,0,0,..,0)^T
    A(tmp5, chi5, MINUS);
    
    // psi5 = (D^dag * D)^(-1) chi5
    InvCG2 (A, tmp5, psi5, RsdCG, MaxCG, n_count);
    break;
  
#if 0
  case MR_INVERTER:
    // psi5 = D^(-1) * chi5
    InvMR (A, chi5, psi5, MRover, RsdCG, MaxCG, n_count);
    break;

  case BICG_INVERTER:
    // psi5 = D^(-1) chi5
    InvBiCG (A, chi5, psi5, RsdCG, MaxCG, n_count);
    break;
#endif
  
  default:
    QDP_error_exit("Unknown inverter type", invType);
  }
  
  if ( n_count == MaxCG )
    QDP_error_exit("no convergence in the inverter", n_count);
  
  ncg_had = n_count;
  
  /* Step (ii) */
  /* psi5_e = A_ee^-1 * [chi5_e  -  D_eo * psi5_o] */
  {
    LatticeDWFermion tmp1, tmp2;

    A.evenOddLinOp(tmp1, psi5, PLUS);
    tmp2[rb[0]] = chi5 - tmp1;
    A.evenEvenInvLinOp(psi5, tmp2, PLUS);
  }

  // Overall normalization
  Real ftmp1 = Real(1) / Real(1 - m_q);

  // Project out first slice after  tmp5 <- P^(-1) . psi5
  DwfFld(tmp5, psi5, MINUS);

  // Normalize and remove contact term
  psi = ftmp1*(peekDW(tmp5, 0) - chi);

  END_CODE("EvenOddPrecDWFermActBase::qprop");
}

