// $Id: wlmesferm_w.cc,v 1.1 2005-01-13 15:25:29 bjoo Exp $

#error "NOT FULLY CONVERTED"

#include "chromabase.h"

using namespace QDP;

/* This routine is specific to Wilson fermions! */

//! MESFERM -- Measures the pseudofermion contribution to the Hamiltonian.
/*!
 * \ingroup molecdyn
 *
 */

/*            THIS CODE USES THE SOLUTION PSI PASSED TO IT. IT DOES NOT */
/*            RECOMPUTE THE CG SOLUTION.  */

/* u      -- gauge field (Read) */
/* chi    -- Gaussian random noise for pseudo-fermion source (Read) */
/* psi    -- (M_dagM)^(-1) * chi (Read) */
/* w_ferm -- Pseudo fermion energy (Write) */
/* NOTES: */
/* w_ferm = chi_dag * psi = chi_*(M_dagM)^(-1)*chi = |(1/M_dag)*chi|^2 */

void WlMesFerm(const multi1d<LatticeColorMatrix>& u,
	       const LatticeFermion& chi,
	       const LatticeFermion& psi,
	       Double& w_ferm,
	       const OrderedSubset& sub)
{
  START_CODE();
  
  w_ferm = 0;

  if ( FermiP == YES )
  {
    /* chi_bar*(1/(M_dag*M))*chi = chi_bar*psi */
    w_ferm = innerProduct(chi,psi,sub) / Double(Layout::vol()*Nc*Ns);
  }
  
  END_CODE();
}
