// $Id: unprec_wilson_fermact_w.cc,v 1.17 2004-07-28 02:38:01 edwards Exp $
/*! \file
 *  \brief Unpreconditioned Wilson fermion action
 */

#include "chromabase.h"
#include "actions/ferm/fermacts/unprec_wilson_fermact_w.h"
#include "actions/ferm/linop/unprec_wilson_linop_w.h"
#include "actions/ferm/linop/lmdagm.h"


//! Produce a linear operator for this action
/*!
 * The operator acts on the entire lattice
 *
 * \param state	    gauge field     	       (Read)
 */
const LinearOperator<LatticeFermion>*
UnprecWilsonFermAct::linOp(Handle<const ConnectState> state) const
{
  return new UnprecWilsonLinOp(state->getLinks(), Mass); 
}

//! Produce a M^dag.M linear operator for this action
/*!
 * The operator acts on the entire lattice
 *
 * \param state    gauge field     	       (Read)
 */
const LinearOperator<LatticeFermion>*
UnprecWilsonFermAct::lMdagM(Handle<const ConnectState> state) const
{
  return new lmdagm<LatticeFermion>(linOp(state));
}


//! Computes the derivative of the fermionic action respect to the link field
/*!
 *         |  dS      dS_f
 * ds_u -- | ----   + -----   ( Write )
 *         |  dU       dU
 *
 * psi -- [1./(M_dag*M)]*chi_  ( read ) 
 *
 * \param ds_u     result      ( Write )
 * \param state    gauge field ( Read )
 * \param psi      solution to linear system ( Read )
 */

void
UnprecWilsonFermAct::dsdu(multi1d<LatticeColorMatrix> & ds_u,
			  Handle<const ConnectState> state,
			  const LatticeFermion& psi) const
{
  START_CODE();
  
  QDPIO::cerr << "UnprecWilsonFermAct::dsdu not implemented" << endl;
  QDP_abort(1);

  END_CODE();
}
