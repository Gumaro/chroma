// $Id: unprec_dwf_fermact_array_w.cc,v 1.1 2003-11-12 22:16:22 edwards Exp $
/*! \file
 *  \brief Unpreconditioned domain-wall fermion action
 */

#include "chromabase.h"
#include "actions/ferm/linop/unprec_dwf_linop_array_w.h"
#include "actions/ferm/fermacts/unprec_dwf_fermact_array_w.h"
#include "actions/ferm/linop/lmdagm_w.h"

//! Creation routine
/*! \ingroup fermact
 *
 * \param WilsonMass   DWF height    (Read)
 * \param m_q          quark mass    (Read)
 */
void UnprecDWFermActArray::create(const Real& WilsonMass_, const Real& m_q_)
{
  WilsonMass = WilsonMass_;
  m_q = m_q_;
  a5  = 1.0;
//    CoeffWilsr_s = (AnisoP) ? Wilsr_s / xiF_0 : 1;
}

//! Produce a linear operator for this action
/*!
 * \ingroup fermact
 *
 * The operator acts on the entire lattice
 *
 * \param u 	    gauge field     	       (Read)
 */
const LinearOperator<multi1d<LatticeFermion> >* 
UnprecDWFermActArray::linOp(const multi1d<LatticeColorMatrix>& u) const
{
  return new UnprecDWLinOpArray(u,WilsonMass,m_q);
}

//! Produce a M^dag.M linear operator for this action
/*!
 * \ingroup fermact
 *
 * The operator acts on the entire lattice
 *
 * \param u 	    gauge field     	       (Read)
 */
const LinearOperator<multi1d<LatticeFermion> >* 
UnprecDWFermActArray::lMdagM(const multi1d<LatticeColorMatrix>& u) const
{
  LinearOperator<multi1d<LatticeFermion> >* mdagm = 
    new lmdagm<multi1d<LatticeFermion> >(UnprecDWLinOpArray(u,WilsonMass,m_q));
  return mdagm;
}

//! Produce a linear operator for this action but with quark mass 1
/*!
 * \ingroup fermact
 *
 * The operator acts on the entire lattice
 *
 * \param u 	    gauge field     	       (Read)
 */
const LinearOperator<multi1d<LatticeFermion> >* 
UnprecDWFermActArray::linOpPV(const multi1d<LatticeColorMatrix>& u) const
{
  return new UnprecDWLinOpArray(u,WilsonMass,1.0);  // fixed to quark mass 1
}

