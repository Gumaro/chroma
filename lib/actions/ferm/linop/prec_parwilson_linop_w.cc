// $Id: prec_parwilson_linop_w.cc,v 1.3 2004-12-12 21:22:16 edwards Exp $
/*! \file
 *  \brief Even-odd preconditioned Wilson fermion linear operator with parity breaking term
 */

#include "chromabase.h"
#include "actions/ferm/linop/prec_parwilson_linop_w.h"

namespace Chroma 
{ 
  //! Creation routine
  /*!
   * \param u_ 	   gauge field     	       (Read)
   * \param Mass_    fermion mass   	       (Read)
   * \param H_       parity breaking term	       (Read)
   */
  void EvenOddPrecParWilsonLinOp::create(const multi1d<LatticeColorMatrix>& u_, 
					 const Real& Mass_, const Real& H_)
  {
    Mass = Mass_;
    H = H_;
    u = u_;
    D.create(u);

    fact = Nd + Mass;
    Real tmp = 1.0 / (fact*fact + H*H);
    invfact1 = fact * tmp;
    invfact2 = H * tmp;
  }


  //! Apply even-odd linop component
  /*!
   * The operator acts on the entire even sublattice
   *
   * \param chi 	  Pseudofermion field     	       (Write)
   * \param psi 	  Pseudofermion field     	       (Read)
   * \param isign   Flag ( PLUS | MINUS )   	       (Read)
   */
  void 
  EvenOddPrecParWilsonLinOp::evenOddLinOp(LatticeFermion& chi, 
					  const LatticeFermion& psi, 
					  enum PlusMinus isign) const
  {
    START_CODE();

    Real mhalf = -0.5;

    D.apply(chi, psi, isign, 0);
    chi[rb[0]] *= mhalf;
  
    END_CODE();
  }

  //! Apply odd-even linop component
  /*!
   * The operator acts on the entire odd sublattice
   *
   * \param chi 	  Pseudofermion field     	       (Write)
   * \param psi 	  Pseudofermion field     	       (Read)
   * \param isign   Flag ( PLUS | MINUS )   	       (Read)
   */
  void 
  EvenOddPrecParWilsonLinOp::oddEvenLinOp(LatticeFermion& chi, 
					  const LatticeFermion& psi, 
					  enum PlusMinus isign) const
  {
    START_CODE();

    Real mhalf = -0.5;

    D.apply(chi, psi, isign, 1);
    chi[rb[1]] *= mhalf;
  
    END_CODE();
  }


#if 0

#error "Not quite correct implementation"


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
  EvenOddPrecParWilsonFermAct::deriv(multi1d<LatticeColorMatrix>& ds_u,
				     const LatticeFermion& chi, 
				     const LatticeFermion& psi,
				     enum PlusMinus isign) const
  {
    START_CODE();
  
    QDPIO::cerr << "EvenOddPrecParWilsonFermAct::dsdu not implemented" << endl;
    QDP_abort(1);

    const multi1d<LatticeColorMatrix>& u = state->getLinks();
				 
    LatticeColorMatrix utmp_1;
    LatticeFermion phi;
    LatticeFermion rho;
    LatticeFermion sigma;
    LatticeFermion ftmp_1;
    LatticeFermion ftmp_2;
    Double ddummy;
    Real dummy;
    int nu;
    int cb;
  
    /* Do the Wilson fermion dS_f/dU with parity breaking term */
    
//  CONSTRUCT_LINEAR_OPERATOR(A, lwlhmpsi, u, KappaMD);

    /*  phi = M(u)*psi  */
    A (A, psi, phi, 1, PLUS);
    
    /* rho = (1-i H gamma_5) * Dslash(0<-1) * psi */
    dslash (u, psi, ftmp_1, PLUS, 1);
    PARBREAK(ftmp_1, H_parity, rho, MINUS);
    
    /* phi = (KappaMD^2)*phi/(1+h^2) = -(KappaMD^2)*M*psi/(1+h^2) */
    dummy = -(KappaMD*KappaMD) / (WORD_VALUE(WORD_H_parity, ONE) + H_parity*H_parity);
    phi = phi * dummy;
    
    /* sigma = (1+i H gamma_5) * Dslash_dag(0<-1) * phi */
    dslash (u, phi, ftmp_1, MINUS, 1);
    PARBREAK(ftmp_1, H_parity, sigma, PLUS);
    
        
    for(int mu = 0; mu < Nd; ++mu)
    {
      cb = 0;

      /* ftmp_2 = (gamma(mu))*psi */
      SPIN_PRODUCT(psi,(INTEGER_LSHIFT_FUNCTION(1,mu)),ftmp_2);
      /* ftmp_2(x) = -(psi(x) - ftmp_2(x)) = -(1 - gamma(mu))*psi( x )  */
      ftmp_2 -= psi;
      utmp_1 = -(shift(ftmp_2, cb, FORWARD, mu) * adj(sigma));

      /* ftmp_2 = (gamma(mu))*phi */
      SPIN_PRODUCT(phi,(INTEGER_LSHIFT_FUNCTION(1,mu)),ftmp_2);
      /* ftmp_2 = phi + ftmp_2 = (1 + gamma(mu))*phi( x)  */
      ftmp_2 += phi;
      utmp_1 += shift(ftmp_2, cb, FORWARD, mu) * adj(rho);
      ds_u[mu][cb] += u[mu][cb] * utmp_1;
      
      cb = 1;

      /* ftmp_2 = (gamma(mu))*ftmp_1 */
      SPIN_PRODUCT(rho,(INTEGER_LSHIFT_FUNCTION(1,mu)),ftmp_2);
      /* ftmp_2 = -(rho - ftmp_2) = -(1 - gamma(mu))*rho( x )  */
      ftmp_2 -= rho;
      utmp_1 = -(shift(ftmp_2, cb, FORWARD, mu) * adj(phi));
      
      /* ftmp_2 = (gamma(mu))*sigma */
      SPIN_PRODUCT(sigma,(INTEGER_LSHIFT_FUNCTION(1,mu)),ftmp_2);
      /* ftmp_1 = ftmp_1 + ftmp_2 = (1 + gamma(mu))*sigma( x + mu)  */
      ftmp_2 += sigma;
      utmp_1 += shift(ftmp_2, cb, FORWARD, mu) * adj(psi);
      ds_u[mu][cb] += u[mu][cb] * utmp_1;
      
    }
        
    END_CODE();
  }
#endif

} // End Namespace Chroma

