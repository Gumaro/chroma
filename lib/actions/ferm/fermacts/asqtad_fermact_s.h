// -*- C++ -*-
// $Id: asqtad_fermact_s.h,v 1.6 2004-01-06 13:59:49 bjoo Exp $
/*! \file
 *  \brief Asqtad staggered fermion action
 */

#ifndef __asqtad_fermact_s_h__
#define __asqtad_fermact_s_h__

#include <fermact.h>
#include "actions/ferm/fermacts/asqtad_state.h"

using namespace QDP;

//! Asqtad staggered fermion action
/*! \ingroup fermact
 *
 */
class AsqtadFermAct : public EvenOddStaggeredTypeFermAct<LatticeFermion>
{
public:
  //! General FermBC
  AsqtadFermAct(Handle< FermBC<LatticeFermion> > fbc_, 
		const Real Mass_, const Real u0_) : 
    fbc(fbc_), Mass(Mass_), u0(u0_) {}
  
  //! Copy constructor
  AsqtadFermAct(const AsqtadFermAct& a) : 
    fbc(a.fbc), Mass(a.Mass), u0(a.u0) {}

  //! Assignment
  AsqtadFermAct& operator=(const AsqtadFermAct& a)
    {fbc=a.fbc; Mass=a.Mass, u0=a.u0; return *this;}

  //! Return the fermion BC object for this action
  const FermBC<LatticeFermion>& getFermBC() const {return *fbc;}

  //! Create state should apply the BC
  const AsqtadConnectStateBase<LatticeFermion>* createState(const multi1d<LatticeColorMatrix>& u_) const;

  //! Produce a linear operator for this action
  const EvenOddLinearOperator<LatticeFermion>* linOp( Handle< const ConnectState> state_) const;

  //! Produce a linear operator M^dag.M for this action
  
  const LinearOperator<LatticeFermion>* lMdagM(Handle< const ConnectState >  state_) 
    const;

  //! accessors 
  const Real getQuarkMass() const { 
    return Mass;
  }

  
  Real getU0() { 
    return u0;
  }

  //! Compute dS_f/dU      DO I NEED THIS?? -- No, a default version is supplied in fermact.h

  //! Destructor is automatic
  ~AsqtadFermAct() {}

private:
  AsqtadFermAct() {} //hide default constructor
  
private:
  Handle< FermBC<LatticeFermion> >  fbc;
  Real Mass;
  Real u0;
};

#endif
