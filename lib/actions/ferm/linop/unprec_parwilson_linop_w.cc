// $Id: unprec_parwilson_linop_w.cc,v 1.5 2005-01-11 19:45:49 edwards Exp $
/*! \file
 *  \brief Unpreconditioned Wilson linear operator with parity breaking term
 */

#include "chromabase.h"
#include "actions/ferm/linop/unprec_parwilson_linop_w.h"

namespace Chroma 
{ 
  //! Creation routine
  /*! \ingroup fermact
   *
   * \param u_ 	   gauge field     	       (Read)
   * \param Mass_    fermion kappa   	       (Read)
   * \param H__      parity breaking term	       (Read)
   */
  void UnprecParWilsonLinOp::create(const multi1d<LatticeColorMatrix>& u_, 
				    const Real& Mass_, const Real& H_)
  {
    Mass = Mass_;
    H = H_;
    u = u_;
    D.create(u);

//    CoeffWilsr_s = (AnisoP) ? Wilsr_s / xiF_0 : 1;
  }


  //! Apply unpreconditioned Wilson fermion linear operator with parity breaking term
  /*!
   * \ingroup linop
   *
   * The operator acts on the entire lattice
   *
   * \param chi 	  Pseudofermion field     	       (Write)
   * \param psi 	  Pseudofermion field     	       (Read)
   * \param isign   Flag ( PLUS | MINUS )   	       (Read)
   */
  void UnprecParWilsonLinOp::operator() (LatticeFermion& chi, const LatticeFermion& psi, 
					 enum PlusMinus isign) const
  {
    START_CODE();

    //
    //  Chi   =  (Nd+Mass)*Psi  -  (1/2) * D' Psi
    //
    LatticeFermion tmp;
    Real fact1 = Nd + Mass;
    Real fact2 = -0.5;

    // D is a Dslash - must apply to both CB-s
    D(tmp, psi, isign);

    // 
    chi = fact1*psi + fact2*tmp;

    switch (isign)
    {
    case PLUS:
      chi += Gamma(Ns*Ns-1)*(H*timesI(psi));
      break;

    case MINUS:
      chi -= Gamma(Ns*Ns-1)*(H*timesI(psi));
      break;
    }

    END_CODE();
  }


  //! Derivative of unpreconditioned ParWilson dM/dU
  /*!
   * \param chi     left vector on cb                           (Read)
   * \param psi     right vector on 1-cb                        (Read)
   * \param isign   D'^dag or D'  ( MINUS | PLUS ) resp.        (Read)
   * \param cb	    Checkerboard of chi vector                  (Read)
   *
   * \return Computes   chi^dag * \dot(D} * psi  
   */
  void 
  UnprecParWilsonLinOp::deriv(multi1d<LatticeColorMatrix>& ds_u,
			      const LatticeFermion& chi, const LatticeFermion& psi, 
			      enum PlusMinus isign) const
  {
    START_CODE();

    // This does both parities
    D.deriv(ds_u, chi, psi, isign);

    // Factor from the -1/2 in front of the dslash
    for(int mu = 0; mu < Nd; ++mu)
      ds_u[mu] *= Real(-0.5);

    END_CODE();
  }


}; // End Namespace Chroma


