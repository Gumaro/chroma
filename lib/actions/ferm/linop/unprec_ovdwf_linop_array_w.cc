// $Id: unprec_ovdwf_linop_array_w.cc,v 1.4 2003-11-16 01:36:12 edwards Exp $
/*! \file
 *  \brief Unpreconditioned Overlap-DWF (Borici) linear operator
 */

#include "chromabase.h"
#include "actions/ferm/linop/unprec_ovdwf_linop_array_w.h"

//! Creation routine
/*! \ingroup fermact
 *
 * \param u_            gauge field   (Read)
 * \param WilsonMass_   DWF height    (Read)
 * \param m_q_          quark mass    (Read)
 */
void 
UnprecOvDWLinOpArray::create(const multi1d<LatticeColorMatrix>& u_, 
			     const Real& WilsonMass_, const Real& m_q_, int N5_)
{
  WilsonMass = WilsonMass_;
  m_q = m_q_;
  a5  = 1.0;
  N5  = N5_;

  D.create(u_);
//    CoeffWilsr_s = (AnisoP) ? Wilsr_s / xiF_0 : 1;
}


//-----------------------------------------------------------------------------
// HACK
/* Terrible implementation just to get the silly thing going */
static inline LatticeFermion 
chiralProjectPlus(const LatticeFermion& l)
{
  return 0.5*(l + Gamma(15)*l);
}

static inline LatticeFermion 
chiralProjectMinus(const LatticeFermion& l)
{
  return 0.5*(l - Gamma(15)*l);
}


//-----------------------------------------------------------------------------



//! Apply the operator onto a source vector
/*!
 * The operator acts on the entire lattice
 *
 * \param psi 	  Pseudofermion field     	       (Read)
 * \param isign   Flag ( PLUS | MINUS )   	       (Read)
 */
multi1d<LatticeFermion> 
UnprecOvDWLinOpArray::operator() (const multi1d<LatticeFermion>& psi, 
				  enum PlusMinus isign) const
{
  multi1d<LatticeFermion> chi(N5);   // probably should check psi.size() == N5

  START_CODE("UnprecOvDWLinOpArray");

  //
  //  Chi   =  D' Psi
  //
  Real fact1 = a5*(Nd - WilsonMass);
  Real fact2 = -0.5*a5;

  if (isign == PLUS)
  {
    LatticeFermion  tmp;

    for(int n=0; n < N5; ++n)
    {
      if (n == 0)
      {
	tmp    = psi[n] - m_q*chiralProjectPlus(psi[N5-1]) + chiralProjectMinus(psi[1]);
	chi[n] = fact1*tmp + fact2*D(tmp, isign) + psi[n] 
	       + m_q*chiralProjectPlus(psi[N5-1]) - chiralProjectMinus(psi[1]);
      }
      else if (n == N5-1)
      {
	tmp    = psi[n] + chiralProjectPlus(psi[N5-2]) - m_q*chiralProjectMinus(psi[0]);
	chi[n] = fact1*tmp + fact2*D(tmp, isign) + psi[n] 
	       - chiralProjectPlus(psi[N5-2]) + m_q*chiralProjectMinus(psi[0]);
      }
      else
      {
	tmp    = psi[n] + chiralProjectPlus(psi[n-1]) + chiralProjectMinus(psi[n+1]);
	chi[n] = fact1*tmp + fact2*D(tmp, isign) + psi[n] 
	       - chiralProjectPlus(psi[n-1]) - chiralProjectMinus(psi[n+1]);
      }
    }          
  }
  else   // isign = MINUS   case
  {
    multi1d<LatticeFermion>  tmp(N5);   // should be more clever and reduce temporaries

    for(int n=0; n < N5; ++n)
      tmp[n] = fact1*psi[n] + fact2*D(psi[n], isign);

    for(int n=0; n < N5; ++n)
    {
      if (n == 0)
      {
	chi[0] = tmp[0] + psi[0] + chiralProjectPlus(tmp[1]) - chiralProjectPlus(psi[1])
  	       - m_q*(chiralProjectMinus(tmp[N5-1]) - chiralProjectMinus(psi[N5-1]));
      }
      else if (n == N5-1)
      {
	chi[n] = tmp[n] + psi[n] + chiralProjectMinus(tmp[N5-2]) - chiralProjectMinus(psi[N5-2])
  	       - m_q*(chiralProjectPlus(tmp[0]) - chiralProjectPlus(psi[0]));
      }
      else
      {
	chi[n] = tmp[n] + psi[n] + chiralProjectMinus(tmp[n-1]) - chiralProjectMinus(psi[n-1])
  	       + chiralProjectPlus(tmp[n+1]) - chiralProjectPlus(psi[n+1]);
      }
    }          
  }

  END_CODE("UnprecOvDWLinOpArray");

  return chi;
}
