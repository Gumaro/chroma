// $Id: lwldslash_w.cc,v 1.10 2004-12-17 17:45:04 bjoo Exp $
/*! \file
 *  \brief Wilson Dslash linear operator
 */

#include "chromabase.h"
#include "actions/ferm/linop/lwldslash_w.h"

using namespace QDP;

namespace Chroma 
{ 
  //! General Wilson-Dirac dslash
  /*! \ingroup linop
   * DSLASH
   *
   * This routine is specific to Wilson fermions!
   *
   * Description:
   *
   * This routine applies the operator D' to Psi, putting the result in Chi.
   *
   *	       Nd-1
   *	       ---
   *	       \
   *   chi(x)  :=  >  U  (x) (1 - isign gamma  ) psi(x+mu)
   *	       /    mu			  mu
   *	       ---
   *	       mu=0
   *
   *	             Nd-1
   *	             ---
   *	             \    +
   *                +    >  U  (x-mu) (1 + isign gamma  ) psi(x-mu)
   *	             /    mu			   mu
   *	             ---
   *	             mu=0
   *
   */


  //! Creation routine
  void QDPWilsonDslash::create(const multi1d<LatticeColorMatrix>& _u)
  {
    u = _u;

    //    CoeffWilsr_s = (AnisoP) ? Wilsr_s / xiF_0 : 1;
  }


  //! General Wilson-Dirac dslash
  /*! \ingroup linop
   * Wilson dslash
   *
   * Arguments:
   *
   *  \param chi	      Result				                (Write)
   *  \param psi	      Pseudofermion field				(Read)
   *  \param isign      D'^dag or D' ( MINUS | PLUS ) resp.		(Read)
   *  \param cb	      Checkerboard of OUTPUT vector			(Read) 
   */
  void 
  QDPWilsonDslash::apply (LatticeFermion& chi, const LatticeFermion& psi, 
			  enum PlusMinus isign, int cb) const
  {
    START_CODE();

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

    END_CODE();
  }



  //! Take deriv of D
  /*! \return Computes   chi^dag * \dot(D} * psi  */
  void 
  QDPWilsonDslash::deriv(multi1d<LatticeColorMatrix>& ds_u,
			 const LatticeFermion& chi, const LatticeFermion& psi, 
			 enum PlusMinus isign, int cb) const
  {
    START_CODE();

    ds_u.resize(Nd);

    for(int mu = 0; mu < Nd; ++mu)
    {
      switch (isign)
      {
     case PLUS:
	// Undaggered:
        ds_u[mu][rb[cb]]    = u[mu]*traceSpin(outerProduct(shift(psi - Gamma(1 << mu)*psi, FORWARD, mu),chi));	
	ds_u[mu][rb[1-cb]]  = zero;

	// The piece that comes from the U^daggered term. 
	// This piece is just -dagger() of the piece from applying
	// this function on the opposite checkerboard. It therefore
	// only contributes a factor of 2 to the traceless antihermitian
	// part of the result. Which should be swept into the taproj
	// normalisation. Right now until then, I explicitly multiply
	// the result by 0.5 below.

	// ds_u[mu][rb[1-cb]]  = traceSpin(outerProduct(psi + Gamma(1 << mu)*psi,shift(chi, FORWARD, mu)))*adj(u[mu]);
       	// ds_u[mu][rb[1-cb]] *= -Real(1);
	//
	// From factor of 2 that comes from the U^daggered piece.
	// This maybe should be absorbed into the taproj normalisation
	//
	// ds_u[mu] *= Real(0.5);

	break;

      case MINUS:
	// Daggered:
	ds_u[mu][rb[cb]]    = u[mu]*traceSpin(outerProduct(shift(psi + Gamma(1 << mu)*psi, FORWARD, mu),chi));
	
	ds_u[mu][rb[1-cb]] = zero;
	
	// The piece that comes from the U^daggered term. 
	// This piece is just -dagger() of the piece from applying
	// this function on the opposite checkerboard. It therefore
	// only contributes a factor of 2 to the traceless antihermitian
	// part of the result. Which should be swept into the taproj
	// normalisation. Right now until then, I explicitly multiply
	// the result by 0.5 below.
	//
	//        ds_u[mu][rb[1-cb]]  = traceSpin(outerProduct(psi - Gamma(1 << mu)*psi,shift(chi, FORWARD, mu)))*adj(u[mu]);
	//        ds_u[mu][rb[1-cb]] *= -Real(1);
	//	 
	// From factor of 2 that comes from the U^daggered piece.
	// This maybe should be absorbed into the taproj normalisation
	//
	// ds_u[mu] *= Real(0.5);

	break;

      default:
	QDP_error_exit("unknown case");
      }
    }
    
    END_CODE();
  }

}; // End Namespace Chroma

