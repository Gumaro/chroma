// $Id: prec_wilson_fermact_w.cc,v 1.1 2003-11-22 21:34:01 edwards Exp $
/*! \file
 *  \brief Even-odd preconditioned Wilson fermion action
 */

#include "chromabase.h"
#include "actions/ferm/linop/prec_wilson_linop_w.h"
#include "actions/ferm/fermacts/prec_wilson_fermact_w.h"
#include "actions/ferm/linop/lmdagm_w.h"

//! Creation routine
/*!
 * \param Mass_        fermion mass    (Read)
 */
void EvenOddPrecWilsonFermAct::create(const Real& Mass_)
{
  Mass = Mass_;
}

//! Produce a linear operator for this action
/*!
 * The operator acts on the odd subset
 *
 * \param u 	    gauge field     	       (Read)
 */
const EvenOddPrecLinearOperator<LatticeFermion>* 
EvenOddPrecWilsonFermAct::linOp(const multi1d<LatticeColorMatrix>& u) const
{
  return new EvenOddPrecWilsonLinOp(u,Mass);
}

//! Produce a M^dag.M linear operator for this action
/*!
 * The operator acts on the odd subset
 *
 * \param u 	    gauge field     	       (Read)
 */
const LinearOperator<LatticeFermion>* 
EvenOddPrecWilsonFermAct::lMdagM(const multi1d<LatticeColorMatrix>& u) const
{
  LinearOperator<LatticeFermion>* mdagm = 
    new lmdagm<LatticeFermion>(EvenOddPrecWilsonLinOp(u,Mass));
  return mdagm;
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
 * \param u        gauge field ( Read )
 * \param psi      solution to linear system ( Read )
 */

void
EvenOddPrecWilsonFermAct::dsdu(multi1d<LatticeColorMatrix>& ds_u,
			       const multi1d<LatticeColorMatrix>& u, 
			       const LatticeFermion& psi) const
{
  START_CODE("EvenOddPrecWilsonFermAct::dsdu");
  
#if 1
  QDPIO::cerr << "EvenOddPrecWilsonFermAct::dsdu not implemented" << endl;
  QDP_abort(1);
#else

#error "Not quite correct implementation"

  LatticeColorMatrix utmp_1;
  LatticeFermion phi;
  LatticeFermion rho;
  LatticeFermion sigma;
  LatticeFermion ftmp_1;
  LatticeFermion ftmp_2;
  Double ddummy;
  Real dummy;
  int cb;

  // Do the usual Wilson fermion dS_f/dU
  const LinearOperator* A = linOp(u);

  WilsonDslash  D(u);

  //  phi = M(u)*psi
  LatticeFermion phi = A(psi, PLUS);
    
    /* rho = Dslash(0<-1) * psi */
  rho = D.apply(psi, PLUS, 1);
    
  /* phi = (KappaMD^2)*phi = -(KappaMD^2)*M*psi */
  dummy = -(KappaMD*KappaMD);
  phi *= dummy;
    
    /* sigma = Dslash_dag(0 <- 1) * phi */
  sigma = D.apply(phi, MINUS, 1);
    
  for(int mu = 0; mu < Nd; ++mu)
  {
    cb = 0;

    /* ftmp_2 = (gamma(mu))*psi */
    ftmp_2 = Gamma(1<<mu) * psi;
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

  delete A;
#endif

  END_CODE("EvenOddPrecWilsonFermAct::dsdu");
}
