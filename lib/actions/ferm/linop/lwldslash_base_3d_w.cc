// $Id: lwldslash_base_3d_w.cc,v 3.1 2007-12-04 16:04:42 bjoo Exp $
/*! \file
 *  \brief Wilson Dslash linear operator
 */

#include "chromabase.h"
#include "actions/ferm/linop/lwldslash_base_3d_w.h"
#include "io/aniso_io.h"

#if QDP_NS == 4
#if QDP_NC == 3
#if QDP_ND == 4

namespace Chroma 
{ 


  //! Take deriv of D
  /*!
   * \param chi     left vector                                 (Read)
   * \param psi     right vector                                (Read)
   * \param isign   D'^dag or D'  ( MINUS | PLUS ) resp.        (Read)
   *
   * \return Computes   \f$\chi^\dag * \dot(D} * \psi\f$
   */
  void
  WilsonDslash3DBase::deriv(multi1d<LatticeColorMatrix>& ds_u,
			  const LatticeFermion& chi, const LatticeFermion& psi, 
			  enum PlusMinus isign) const
  {
    START_CODE();

    ds_u.resize(Nd);

    multi1d<LatticeColorMatrix> ds_tmp(Nd);
    deriv(ds_u, chi, psi, isign, 0);
    deriv(ds_tmp, chi, psi, isign, 1);
    ds_u += ds_tmp;

    END_CODE();
  }


  //! Take deriv of D
  /*! \return Computes   \f$\chi^\dag * \dot(D} * \psi\f$  */
  void 
  WilsonDslash3DBase::deriv(multi1d<LatticeColorMatrix>& ds_u,
			  const LatticeFermion& chi, const LatticeFermion& psi, 
			  enum PlusMinus isign, int cb) const
  {
    START_CODE();

    ds_u.resize(Nd);

    AnisoParam_t anisoParam = getAnisoParam();
    multi1d<Real> anisoWeights(Nd);
    anisoWeights = 1;

    Real ff = where(anisoParam.anisoP, anisoParam.nu / anisoParam.xi_0, Real(1));

    if (anisoParam.anisoP)
    {
      // Set the weights
      for(int mu=0; mu < Nd; ++mu)
      {
	if (mu != anisoParam.t_dir)
	  anisoWeights[mu] *= ff;
      }
    }

    for(int mu = 0; mu < 3; ++mu) {

      // Break this up to use fewer expressions:
      LatticeFermion temp_ferm1;
      LatticeHalfFermion tmp_h;

      switch (isign) {
	
      case PLUS:
	// Undaggered: Minus Projectors
	{

	  switch(mu) { 
	  case 0:
	    tmp_h[rb[1-cb]] = spinProjectDir0Minus(psi);
	    temp_ferm1[rb[1-cb]] = spinReconstructDir0Minus(tmp_h);
	    break;
	  case 1:
	    tmp_h[rb[1-cb]] = spinProjectDir1Minus(psi);
	    temp_ferm1[rb[1-cb]] = spinReconstructDir1Minus(tmp_h);
	    break;
	  case 2:
	    tmp_h[rb[1-cb]] = spinProjectDir2Minus(psi);
	    temp_ferm1[rb[1-cb]] = spinReconstructDir2Minus(tmp_h);
	    break;
	  default:
	    break;
	  };
	
	}
	break;

      case MINUS:
	{
	  // Daggered: Plus Projectors
	  LatticeHalfFermion tmp_h;
	  switch(mu) { 
	  case 0:
	    tmp_h[rb[1-cb]] = spinProjectDir0Plus(psi);
	    temp_ferm1[rb[1-cb]] = spinReconstructDir0Plus(tmp_h);
	    break;
	  case 1:
	    tmp_h[rb[1-cb]] = spinProjectDir1Plus(psi);
	    temp_ferm1[rb[1-cb]] = spinReconstructDir1Plus(tmp_h);
	    break;
	  case 2:
	    tmp_h[rb[1-cb]] = spinProjectDir2Plus(psi);
	    temp_ferm1[rb[1-cb]] = spinReconstructDir2Plus(tmp_h);
	    break;
	  default:
	    break;
	  };
	}
      break;
      default:
	QDP_error_exit("unknown case");
      }

      // QDP Shifts the whole darn thing anyhow
      LatticeFermion temp_ferm2 = shift(temp_ferm1, FORWARD, mu);
      LatticeColorMatrix temp_mat;
      
      // This step supposedly optimised in QDP++
      temp_mat[rb[cb]] = traceSpin(outerProduct(temp_ferm2,chi));
    
      // Just do the bit we need.
      ds_u[mu][rb[cb]] = anisoWeights[mu] * temp_mat;
      ds_u[mu][rb[1-cb]] = zero;    
    }

    // Remaining directions?
    for(int mu=3; mu < Nd; mu++) { 
      ds_u[mu] = zero;
    }

    getFermBC().zero(ds_u);

    END_CODE();
  }


  //! Return flops performed by the operator()
  unsigned long 
  WilsonDslash3DBase::nFlops() const {return 990;}

} // End Namespace Chroma


#endif // QDP_ND
#endif // QDP_NC
#endif // QDP_NS
