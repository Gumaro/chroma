// $Id: invcg2_array.cc,v 1.3 2003-11-14 16:02:27 edwards Exp $
/*! \file
 *  \brief Conjugate-Gradient algorithm for a generic Linear Operator
 */

#include "chromabase.h"
#include "actions/ferm/invert/invcg2_array.h"


//---------------- HACK ----------------------------
// WARNING - Inefficient; improve later - move into QDP
#if 1
namespace QDP {

template<class T>
inline typename UnaryReturn<OLattice<T>, FnNorm2>::Type_t
norm2(const multi1d< OLattice<T> >& s1, const Subset& s)
{
  typename UnaryReturn<OLattice<T>, FnNorm2>::Type_t  d;

  d = norm2(s1[0],s);
  for(int n=1; n < s1.size(); ++n)
    d += norm2(s1[n],s);

  return d;
}
}
#endif
//---------------------------------------------------



//! Conjugate-Gradient (CGNE) algorithm for a generic Linear Operator
/*! \ingroup invert
 * This subroutine uses the Conjugate Gradient (CG) algorithm to find
 * the solution of the set of linear equations
 *
 *   	    Chi  =  A . Psi
 *
 * where       A = M^dag . M
 *
 * Algorithm:

 *  Psi[0]  :=  initial guess;    	       Linear interpolation (argument)
 *  r[0]    :=  Chi - M^dag . M . Psi[0] ;     Initial residual
 *  p[1]    :=  r[0] ;	       	       	       Initial direction
 *  IF |r[0]| <= RsdCG |Chi| THEN RETURN;      Converged?
 *  FOR k FROM 1 TO MaxCG DO    	       CG iterations
 *      a[k] := |r[k-1]|**2 / <Mp[k],Mp[k]> ;
 *      Psi[k] += a[k] p[k] ;   	       New solution vector
 *      r[k] -= a[k] M^dag . M . p[k] ;        New residual
 *      IF |r[k]| <= RsdCG |Chi| THEN RETURN;  Converged?
 *      b[k+1] := |r[k]|**2 / |r[k-1]|**2 ;
 *      p[k+1] := r[k] + b[k+1] p[k];          New direction
 *
 * Arguments:
 *
 *  \param M       Linear Operator    	       (Read)
 *  \param chi     Source	               (Read)
 *  \param psi     Solution    	    	       (Modify)
 *  \param RsdCG   CG residual accuracy        (Read)
 *  \param MaxCG   Maximum CG iterations       (Read)
 *  \param n_count Number of CG iteration      (Write)
 *
 * Local Variables:
 *
 *  p   	       Direction vector
 *  r   	       Residual vector
 *  cp  	       | r[k] |**2
 *  c   	       | r[k-1] |**2
 *  k   	       CG iteration counter
 *  a   	       a[k]
 *  b   	       b[k+1]
 *  d   	       < p[k], A.p[k] >
 *  Mp  	       Temporary for  M.p
 *
 * Subroutines:
 *                             +               
 *  A       Apply matrix M or M  to vector
 *
 * Operations:
 *
 *  2 A + 2 Nc Ns + N_Count ( 2 A + 10 Nc Ns )
 */

template<typename T>
void InvCG2_a(const LinearOperator< multi1d<T> >& M,
	      const multi1d<T> & chi,
	      multi1d<T>& psi,
	      const Real& RsdCG, 
	      int MaxCG, 
	      int& n_count)
{
  const int N = psi.size();
  const OrderedSubset& s = M.subset();

  Real chi_sq =  Real(norm2(chi,s));

  QDPIO::cout << "chi_norm = " << sqrt(chi_sq) << endl;
  Real rsd_sq = (RsdCG * RsdCG) * chi_sq;

  //                                            +
  //  r[0]  :=  Chi - A . Psi[0]    where  A = M  . M
    
  //                      +
  //  r  :=  [ Chi  -  M(u)  . M(u) . psi ]
  multi1d<T> r(N);
  multi1d<T> tmp = M(M(psi, PLUS), MINUS);   // extra memory traversal
  for(int n=0; n < N; ++n)
    r[n][s] = chi[n] - tmp[n];

  //  p[1]  :=  r[0]
  multi1d<T> p(N);
  for(int n=0; n < N; ++n)
    p[n][s] = r[n];
  
  //  Cp = |r[0]|^2
  Double cp = norm2(r, s);   	       	   /* 2 Nc Ns  flops */

  QDPIO::cout << "WlInvCG: k = 0  cp = " << cp << "  rsd_sq = " << rsd_sq << endl;

  //  IF |r[0]| <= RsdCG |Chi| THEN RETURN;
  if ( toBool(cp  <=  rsd_sq) )
  {
    n_count = 0;
    return;
  }

  //
  //  FOR k FROM 1 TO MaxCG DO
  //
  multi1d<T>  mp(N);
  Real a, b;
  Double c, d;
  
  for(int k = 1; k <= MaxCG; ++k)
  {
    //  c  =  | r[k-1] |**2
    c = cp;

    //  a[k] := | r[k-1] |**2 / < p[k], Ap[k] > ;
    //      	       	       	       	       	  +
    //  First compute  d  =  < p, A.p >  =  < p, M . M . p >  =  < M.p, M.p >
    //  Mp = M(u) * p

//    mp[s] = M(p, PLUS);
    mp = M(p, PLUS);       // WARNING - removed subset here 

    //  d = | mp | ** 2
    d = norm2(mp, s);	/* 2 Nc Ns  flops */

    a = Real(c)/Real(d);

    //  Psi[k] += a[k] p[k]
    for(int n=0; n < N; ++n)
      psi[n][s] += a * p[n];	/* 2 Nc Ns  flops */

    //  r[k] -= a[k] A . p[k] ;
    //      	       +            +
    //  r  =  r  -  M(u)  . Mp  =  M  . M . p  =  A . p

    tmp = M(mp, MINUS);            // WARNING - removed subset here
    for(int n=0; n < N; ++n)
      r[n][s] -= a * tmp[n];       // WARNING - extra memory traversal

    //  IF |r[k]| <= RsdCG |Chi| THEN RETURN;

    //  cp  =  | r[k] |**2
    cp = norm2(r, s);	                /* 2 Nc Ns  flops */

    QDPIO::cout << "WlInvCG: k = " << k << "  cp = " << cp << endl;

    if ( toBool(cp  <=  rsd_sq) )
    {
      n_count = k;
      return;
    }

    //  b[k+1] := |r[k]|**2 / |r[k-1]|**2
    b = Real(cp) / Real(c);

    //  p[k+1] := r[k] + b[k+1] p[k]
    for(int n=0; n < N; ++n)
      p[n][s] = r[n] + b*p[n];	/* Nc Ns  flops */
  }
  n_count = MaxCG;
  QDP_error_exit("too many CG iterations: count = %d", n_count);
}


// Fix here for now
template<>
void InvCG2(const LinearOperator< multi1d<LatticeFermion> >& M,
	    const multi1d<LatticeFermion>& chi,
	    multi1d<LatticeFermion>& psi,
	    const Real& RsdCG, 
	    int MaxCG, 
	    int& n_count)
{
  InvCG2_a(M, chi, psi, RsdCG, MaxCG, n_count);
}

