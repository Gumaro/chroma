// $Id: prec_nef_linop_array_w.cc,v 1.1 2004-08-08 10:29:33 kostas Exp $
/*! \file
 *  \brief  4D-style even-odd preconditioned NEF domain-wall linear operator
 */

#include "chromabase.h"
#include "actions/ferm/linop/prec_nef_linop_array_w.h"



//! Creation routine
/*! \ingroup fermact
 *
 * \param u_            gauge field   (Read)
 * \param WilsonMass_   DWF height    (Read)
 * \param b5_           NEF parameter (Read)
 * \param c5_           NEF parameter (Read)
 * \param m_q_          quark mass    (Read)
 * \param N5_           extent of 5D  (Read)
 */
void 
EvenOddPrecNEFDWLinOpArray::create(const multi1d<LatticeColorMatrix>& u_, 
				   const Real& WilsonMass_, const Real &b5_, 
				   const Real &c5_, const Real& m_q_, int N5_)
{
  START_CODE();

  WilsonMass = WilsonMass_;
  m_q = m_q_;
  b5  = b5_;
  c5  = c5_;
  N5  = N5_;

  D.create(u_);

  InvTwoKappa = 1.0 + b5*(4-WilsonMass) ; 
  TwoKappa = 1.0 / InvTwoKappa ;
  Kappa = TwoKappa/2.0 ;
  
  invDfactor =1.0/(1.0  + m_q/pow(InvTwoKappa,N5)) ;

  END_CODE();
}


//! Apply the even-even (odd-odd) coupling piece of the domain-wall fermion operator
/*!
 * \ingroup linop
 *
 * The operator acts on the entire lattice
 *
 * \param psi 	  Pseudofermion field     	       (Read)
 * \param isign   Flag ( PLUS | MINUS )   	       (Read)
 * \param cb      checkerboard ( 0 | 1 )               (Read)
 */
void 
EvenOddPrecNEFDWLinOpArray::applyDiag(multi1d<LatticeFermion>& chi, 
				      const multi1d<LatticeFermion>& psi, 
				      enum PlusMinus isign,
				      const int cb) const
{
  START_CODE();

  switch ( isign ) {
    
  case PLUS:
    {
      for(int s(1);s<N5-1;s++) // 1/2k psi[s] + P_- * psi[s+1] + P_+ * psi[s-1]
	chi[s][rb[cb]] = InvTwoKappa*psi[s] - 
	  0.5*( psi[s+1] + psi[s-1] + GammaConst<Ns,Ns*Ns-1>()*(psi[s-1] - psi[s+1]) ) ;
      
      int N5m1(N5-1) ;
      //s=0 -- 1/2k psi[0] - P_- * psi[1] + mf* P_+ * psi[N5-1]
      chi[0][rb[cb]] = InvTwoKappa*psi[0] - 
	0.5*( psi[1]   - m_q*psi[N5m1] - GammaConst<Ns,Ns*Ns-1>()*(m_q*psi[N5m1] + psi[1]) ) ;
      
      int N5m2(N5-2);
      //s=N5-1 -- 1/2k psi[N5-1] +mf* P_- * psi[0]  -  P_+ * psi[N5-2]
      chi[N5m1][rb[cb]] = InvTwoKappa*psi[N5m1] - 
	0.5*( psi[N5m2] - m_q *psi[0] + GammaConst<Ns,Ns*Ns-1>()*(psi[N5m2] + m_q * psi[0]) );
    }
    break ;

  case MINUS:
    {    
      for(int s(1);s<N5-1;s++) // 1/2k psi[s] - P_+ * psi[s+1] - P_- * psi[s-1]
	chi[s][rb[cb]] = InvTwoKappa*psi[s] - 
	  0.5*( psi[s+1] + psi[s-1] + GammaConst<Ns,Ns*Ns-1>()*(psi[s+1] - psi[s-1]) ) ;
      
      int N5m1(N5-1) ;
      //s=0 -- 1/2k psi[0] - P_+ * psi[1] + mf* P_- * psi[N5-1]
      chi[0][rb[cb]] = InvTwoKappa*psi[0] - 
	0.5*( psi[1]   - m_q*psi[N5m1] + GammaConst<Ns,Ns*Ns-1>()*( psi[1]+m_q*psi[N5m1]) ) ;
      
      int N5m2(N5-2);
      //s=N5-1 -- 1/2k psi[N5-1] + mf* P_+ * psi[0]  -  P_- * psi[N5-2]
      chi[N5m1][rb[cb]] = InvTwoKappa*psi[N5m1] - 
	0.5*( psi[N5m2] - m_q *psi[0] - GammaConst<Ns,Ns*Ns-1>()*(psi[N5m2] + m_q * psi[0]) );
    }
    break ;
  }

  END_CODE();
}


//! Apply the inverse even-even (odd-odd) coupling piece of the domain-wall fermion operator
/*!
 * \ingroup linop
 *
 * The operator acts on the entire lattice
 *
 * \param psi 	  Pseudofermion field     	       (Read)
 * \param isign   Flag ( PLUS | MINUS )   	       (Read)
 * \param cb      checkerboard ( 0 | 1 )               (Read)
 */
void 
EvenOddPrecNEFDWLinOpArray::applyDiagInv(multi1d<LatticeFermion>& chi, 
				      const multi1d<LatticeFermion>& psi, 
				      enum PlusMinus isign,
				      const int cb) const
{
  START_CODE();

  switch ( isign ) {

  case PLUS:
    {
      // Copy and scale by TwoKappa (1/M0)
      for(int s(0);s<N5;s++)
	chi[s][rb[cb]] = TwoKappa * psi[s] ;
      
      // First apply the inverse of Lm 
      Real fact(0.5*m_q*TwoKappa) ;
      for(int s(0);s<N5-1;s++){
	chi[N5-1][rb[cb]] -= fact * (chi[s] - GammaConst<Ns,Ns*Ns-1>()*chi[s])  ;
	fact *= TwoKappa ;
      }
      
      //Now apply the inverse of L. Forward elimination 
      for(int s(1);s<N5;s++)
	chi[s][rb[cb]] += Kappa*(chi[s-1] + GammaConst<Ns,Ns*Ns-1>()*chi[s-1]) ;
      
      //The inverse of D  now
      chi[N5-1][rb[cb]] *= invDfactor ;
      // That was easy....
      
      //The inverse of R. Back substitution...... Getting there! 
      for(int s(N5-2);s>-1;s--)
	chi[s][rb[cb]] += Kappa*(chi[s+1] - GammaConst<Ns,Ns*Ns-1>()*chi[s+1]) ;
      
      //Finally the inverse of Rm 
      LatticeFermion tt;
      fact = 0.5*m_q*TwoKappa;
      tt[rb[cb]] = fact*(chi[N5-1] + GammaConst<Ns,Ns*Ns-1>()*chi[N5-1]);
      for(int s(0);s<N5-1;s++){
	chi[s][rb[cb]] -= tt  ;
	tt[rb[cb]] *= TwoKappa ;
      }
    }
    break ;
    
  case MINUS:
    {
      // Copy and scale by TwoKappa (1/M0)
      for(int s(0);s<N5;s++)
	chi[s][rb[cb]] = TwoKappa * psi[s] ;
      
      // First apply the inverse of Lm 
      Real fact(0.5*m_q*TwoKappa) ;
      for(int s(0);s<N5-1;s++){
	chi[N5-1][rb[cb]] -= fact * (chi[s] + GammaConst<Ns,Ns*Ns-1>()*chi[s])  ;
	fact *= TwoKappa ;
      }
      
      //Now apply the inverse of L. Forward elimination 
      for(int s(1);s<N5;s++)
	chi[s][rb[cb]] += Kappa*(chi[s-1] - GammaConst<Ns,Ns*Ns-1>()*chi[s-1]) ;
      
      //The inverse of D  now
      chi[N5-1][rb[cb]] *= invDfactor ;
      // That was easy....
      
      //The inverse of R. Back substitution...... Getting there! 
      for(int s(N5-2);s>-1;s--)
	chi[s][rb[cb]] += Kappa*(chi[s+1] + GammaConst<Ns,Ns*Ns-1>()*chi[s+1]) ;
      
      //Finally the inverse of Rm 
      LatticeFermion tt;
      tt[rb[cb]] = (0.5*m_q*TwoKappa)*(chi[N5-1] - GammaConst<Ns,Ns*Ns-1>()*chi[N5-1]);
      for(int s(0);s<N5-1;s++){
	chi[s][rb[cb]] -= tt  ;
	tt[rb[cb]] *= TwoKappa ;
      }
    }
    break ;
  }

  //Done! That was not that bad after all....
  //See, I told you so...
  END_CODE();
}
