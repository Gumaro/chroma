// -*- C++ -*-
// $Id: unprec_dwf_fermact_w.h,v 1.4 2003-11-09 22:52:39 edwards Exp $
/*! \file
 *  \brief Unpreconditioned domain-wall fermion action
 */

#ifndef __unprec_dwf_fermact_w_h__
#define __unprec_dwf_fermact_w_h__

#include "fermact.h"
#include "actions/ferm/linop/unprec_dwf_linop_w.h"

using namespace QDP;

//! Unpreconditioned domain-wall fermion action
/*! \ingroup fermact
 *
 * Unprecondition domain-wall fermion action. The conventions used here
 * are specified in Phys.Rev.D63:094505,2001 (hep-lat/0005002).
 */

class UnprecDWFermAct : public UnprecWilsonTypeFermAct<LatticeDWFermion>
{
public:
  //! Partial constructor
  UnprecDWFermAct() {}

  //! Full constructor
  UnprecDWFermAct(const Real& WilsonMass, const Real& m_q)
    {create(WilsonMass, m_q);}

  //! Creation routine
  void create(const Real& WilsonMass, const Real& m_q);

  //! Produce a linear operator for this action
  const LinearOperator<LatticeDWFermion>* linOp(const multi1d<LatticeColorMatrix>& u) const;

  //! Produce a linear operator M^dag.M for this action
  const LinearOperator<LatticeDWFermion>* lMdagM(const multi1d<LatticeColorMatrix>& u) const;

  //! Produce a linear operator for this action but with quark mass 1
  const LinearOperator<LatticeDWFermion>* linOpPV(const multi1d<LatticeColorMatrix>& u) const;

  //! A new quark propagator routine for 4D fermions
  void qprop(LatticeFermion& psi, 
	     const multi1d<LatticeColorMatrix>& u, 
	     const LatticeFermion& chi, 
	     enum InvType invType,
	     const Real& RsdCG, 
	     int MaxCG, int& ncg_had) const;

  //! Compute dS_f/dU
  multi1d<LatticeColorMatrix> dsdu(const multi1d<LatticeColorMatrix>& u,
				   const LatticeDWFermion& psi) const;

  //! Destructor is automatic
  ~UnprecDWFermAct() {}

private:
  Real WilsonMass;
  Real m_q;
  Real a5;
};

#endif
