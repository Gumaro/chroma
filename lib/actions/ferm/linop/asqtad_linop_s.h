// -*- C++ -*-
// $Id: asqtad_linop_s.h,v 1.6 2004-11-06 11:30:45 mcneile Exp $
/*! \file
 *  \brief Unpreconditioned Wilson fermion linear operator
 */
// NEW $Id: asqtad_linop_s.h 2003/11/13 steve 
// Asqtad Staggered fermion linear operator

#ifndef __asqtad_linop_s_h__
#define __asqtad_linop_s_h__

#include "linearop.h"
#include "linop.h"
using namespace QDP;

//! Asqtad Staggered-Dirac operator
/*!
 * \ingroup linop
 *
 * This routine is specific to Staggered fermions!
 */

class AsqtadLinOp : public EvenOddLinearOperator<LatticeStaggeredFermion>
{
public:
  //! Partial constructor - Must use create later
  AsqtadLinOp() {}

  //! Full constructor
  AsqtadLinOp(const multi1d<LatticeColorMatrix>& u_fat_, const multi1d<LatticeColorMatrix>& u_triple_, const Real& Mass_) 
  {
    create(u_fat_, u_triple_, Mass_);
  }


  void create(const multi1d<LatticeColorMatrix>& u_fat_, const multi1d<LatticeColorMatrix>& u_triple_, const Real& Mass_) {
    //u_fat = u_fat_;
    // u_triple = u_triple_;
    Mass = Mass_;
    D.create(u_fat_, u_triple_);
  };
  //! Destructor is automatic
  ~AsqtadLinOp() {}

  //! Apply the the even-even block onto a source vector
  inline void evenEvenLinOp(LatticeStaggeredFermion& chi, const LatticeStaggeredFermion& psi, 
		     enum PlusMinus isign) const 
  {
    chi[ rb[0] ] = 2*Mass*psi;
  }
  
  //! Apply the the even-odd block onto a source vector
  void evenOddLinOp(LatticeStaggeredFermion& chi, const LatticeStaggeredFermion& psi, 
		    enum PlusMinus isign) const;

  //! Apply the the odd-even block onto a source vector
  void oddEvenLinOp(LatticeStaggeredFermion& chi, const LatticeStaggeredFermion& psi, 
		    enum PlusMinus isign) const;

  //! Apply the the odd-odd block onto a source vector
  inline void oddOddLinOp(LatticeStaggeredFermion& chi, const LatticeStaggeredFermion& psi, 
		   enum PlusMinus isign) const
  {
    chi[ rb[1] ] = 2*Mass*psi;
  }

private:
  Real Mass;

  // These are really only needed for D. I bring back D here where
  // Steve originally had it.  We don't need u_fat and u_triple here
  // they are kept in the action now.
  // multi1d<LatticeColorMatrix> u_fat;
  // multi1d<LatticeColorMatrix> u_triple;
  AsqtadDslash D;
};

#endif
