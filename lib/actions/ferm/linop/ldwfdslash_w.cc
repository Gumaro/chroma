// $Id: ldwfdslash_w.cc,v 1.2 2003-11-09 22:35:19 edwards Exp $
/*! \file
 *  \brief DWF Dslash linear operator
 */

#include "chromabase.h"
#include "actions/ferm/linop/ldwfdslash_w.h"

using namespace QDP;

//! The even-odd preconditioned 5D DWF dslash operator
/*!
 * \ingroup linop
 *
 * This routine is specific to Wilson fermions!
 *
 */


//! Creation routine
void DWDslash::create(const multi1d<LatticeColorMatrix>& u_, const Real& WilsonMass_)
{
  u = u_;
  WilsonMass = WilsonMass_;

//    CoeffWilsr_s = (AnisoP) ? Wilsr_s / xiF_0 : 1;
}


//! Domain-wall dslash linear operator
/*! 
 * \ingroup linop
 * Domain-wall dslash
 *
 * Arguments:
 *
 *  \param psi	      Pseudofermion field				(Read)
 *  \param isign      D'^dag or D' ( MINUS | PLUS ) resp.		(Read)
 *  \param cb	      Checkerboard of OUTPUT vector			(Read) 
 */
LatticeDWFermion DWDslash::apply (const LatticeDWFermion& psi, enum PlusMinus isign, int cb) const
{
  START_CODE("lDWDslash");

  LatticeDWFermion chi;

  /*     F 
   *   a2  (x)  :=  U  (x) (1 - isign gamma  ) psi(x)
   *     mu          mu                    mu
   */
  /*     B           +
   *   a2  (x)  :=  U  (x-mu) (1 + isign gamma  ) psi(x-mu)
   *     mu          mu                       mu
   */
  // Recontruct the bottom two spinor components from the top two
  /*                        F           B
   *   chi(x) :=  sum_mu  a2  (x)  +  a2  (x)
   *                        mu          mu
   */

  // WARNING NOT FULLY IMPLEMENTED


  /* Why are these lines split? An array syntax would help, but the problem is deeper.
   * The expression templates require NO variable args (like int's) to a function
   * and all args must be known at compile time. Hence, the function names carry
   * (as functions usually do) the meaning (and implicit args) to a function.
   */
  switch (isign)
  {
  case PLUS:
    chi[rb[cb]] = spinReconstructDir0Minus(u[0] * shift(spinProjectDir0Minus(psi), FORWARD, 0))
                + spinReconstructDir0Plus(shift(adj(u[0]) * spinProjectDir0Plus(psi), BACKWARD, 0))
#if QDP_ND >= 2
                + spinReconstructDir1Minus(u[1] * shift(spinProjectDir1Minus(psi), FORWARD, 1))
                + spinReconstructDir1Plus(shift(adj(u[1]) * spinProjectDir1Plus(psi), BACKWARD, 1))
#endif
#if QDP_ND >= 3
                + spinReconstructDir2Minus(u[2] * shift(spinProjectDir2Minus(psi), FORWARD, 2))
                + spinReconstructDir2Plus(shift(adj(u[2]) * spinProjectDir2Plus(psi), BACKWARD, 2))
#endif
#if QDP_ND >= 4
                + spinReconstructDir3Minus(u[3] * shift(spinProjectDir3Minus(psi), FORWARD, 3))
                + spinReconstructDir3Plus(shift(adj(u[3]) * spinProjectDir3Plus(psi), BACKWARD, 3))
#endif
#if QDP_ND >= 5
#error "Unsupported number of dimensions"
#endif
    ;
    break;

  case MINUS:
    chi[rb[cb]] = spinReconstructDir0Plus(u[0] * shift(spinProjectDir0Plus(psi), FORWARD, 0))
                + spinReconstructDir0Minus(shift(adj(u[0]) * spinProjectDir0Minus(psi), BACKWARD, 0))
#if QDP_ND >= 2
                + spinReconstructDir1Plus(u[1] * shift(spinProjectDir1Plus(psi), FORWARD, 1))
                + spinReconstructDir1Minus(shift(adj(u[1]) * spinProjectDir1Minus(psi), BACKWARD, 1))
#endif
#if QDP_ND >= 3
                + spinReconstructDir2Plus(u[2] * shift(spinProjectDir2Plus(psi), FORWARD, 2))
                + spinReconstructDir2Minus(shift(adj(u[2]) * spinProjectDir2Minus(psi), BACKWARD, 2))
#endif
#if QDP_ND >= 4
                + spinReconstructDir3Plus(u[3] * shift(spinProjectDir3Plus(psi), FORWARD, 3))
                + spinReconstructDir3Minus(shift(adj(u[3]) * spinProjectDir3Minus(psi), BACKWARD, 3))
#endif
#if QDP_ND >= 5
#error "Unsupported number of dimensions"
#endif
    ;
    break;
  }

  END_CODE("lDWDslash");

  return chi;
}

