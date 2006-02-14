// -*- C++ -*-
// $Id: polynomial_op.h,v 1.4 2006-02-14 23:15:08 kostas Exp $
/*! \file
 *  \brief Polynomial filter for 4D Dirac operators. It creates an approximation
 *    to 1/Q^2 in the range [\mu, Lambda_max] with Q = \gamma5 M
 *    where M is a dirac operator of some kind (EO preconditioned is accepted).
 *    Can handle any 4D operator but not 5D since gamma_5 hermiticity is more
 *    involved there. 
 *
 *   Initial version Feb. 6, 2006 (Kostas Orginos)
 */

#ifndef _LPOLY_H
#define _LPOLY_H

#include "handle.h"
#include "linearop.h"
#include "polylinop.h"

using namespace QDP::Hints;

namespace Chroma 
{
template<typename T, typename P>
class lpoly: public PolyLinearOperator<T,P>
{
private:
 const Handle<const DiffLinearOperator<T,P> > M;   // this is the operator

 Double LowerBound ;
 Double UpperBound ;

 int degree ;

 // The polynomium is: c_Zero * Prod_i ( Q^2 - root[i])*inv_c[i] 

 multi1d<DComplex> root ;
 multi1d<DComplex> inv_c ;

 Real c_Zero ; // the zeroth order approximation to the inverse  ie. a constant


public:
 // constructor
 // need to modify the contructor to pass down the roots
 lpoly(Handle<const DiffLinearOperator<T,P> > m_,
       int degree_,
       const Real& lower_bound_,
       const Real& upper_bound_, 
       int ord) : M(m_), LowerBound(lower_bound_),  UpperBound(upper_bound_), degree(degree_)
   { //This sets up Chebyshev Polynomials. But we should have a general
     // class that allows us to use different types of polynomials
     
     //Qsq = new lmdagm<T>(*M) ;

     QDPIO::cout<<"lpoly constructor\n" ;

     if(degree_%2 !=0 ){
       QDPIO::cout<<"lpoly: Polynomium degree must be even.\n" ;
       degree_++ ;
       QDPIO::cout<<"lpoly: Using degree:" << degree_<<endl ;       
     }
     //UpperBound = upper_bound_ ;
     root.resize(degree_);
     inv_c.resize(degree_);
     // Arrange the roots in complex conjugate pairs
     
     Double eps = lower_bound_/upper_bound_ ;
     // 2PI/(N+1) ;
     Double www = 6.28318530717958647696/(degree_+1.0) ;
     
     int j(0) ;
     // complex conjugate pairs are at i and N-1-i
     for(int k(1);k<=ord;k++){
       //QDPIO::cout<<"k: "<<k<<endl ;
       for(int i(k-1);i<degree/2;i+=ord){
	 //QDPIO::cout<<"i: "<<i<<endl ;
	 Real ii = i+1.0 ;
	 root[j] = upper_bound_*cmplx(0.5*(1.0+eps)*(1.0-cos(www*ii)) , -sqrt(eps)*sin(www*ii));
	 inv_c[j] = 1.0/(upper_bound_*0.5*(1.0+eps) - root[j]) ;
	 root [degree-1-j] = conj(root[j]    ) ;
	 inv_c[degree-1-j] = conj(inv_c[j]) ;
	 j++ ;
       }
     }
     c_Zero = 2.0/(UpperBound+LowerBound) ;
     // The polynomium is: c_Zero * Prod_i ( Q^2 - root[i])*inv_c[i] 
     
   }
   
   ~lpoly(){}

   //! Subset comes from underlying operator
   inline const OrderedSubset& subset() const {return M->subset();}

   //! Returns  the roots
   multi1d<DComplex> Roots() const {
     return root ;
   }

   //! Returns  the roots
   multi1d<DComplex> MonomialNorm() const {
     return inv_c ;
   }


   //! Apply the underlying (Mdagger M) operator
   void Qsq(T& y, const T& x) const {
     T t ;   moveToFastMemoryHint(t);
     (*M)(t,x,PLUS) ;
     (*M)(y,t,MINUS);
   }

   void applyChebInv(T& x, const T& b) const 
   {
     //Chi has the initial guess as it comes in
     //b is the right hand side
     Real a = 2/(UpperBound-LowerBound) ; 
     Real d = (UpperBound+LowerBound)/(UpperBound-LowerBound) ; 
     T QsqX ;
     //*Qsq(QsqX,x,PLUS) ;
     Qsq(QsqX,x) ;
     T r ;
     r = b - QsqX ;
     T y(x) ;
     int m(1) ;
     Real sigma_m(d) ;
     Real sigma_m_minus_one(1.0) ;
     Real sigma_m_plus_one ;
     
     x = x + a/d*r ;
     //*Qsq(QsqX,x,PLUS) ;
     Qsq(QsqX,x) ;
     r = b - QsqX ;
     T p ;
     while (m<degree+1){
       //QDPIO::cout<<"iter: "<<m<<" 1/sigma: "<<1.0/sigma_m<<endl ;
       sigma_m_plus_one = 2.0*d*sigma_m - sigma_m_minus_one ;
       p = 2.0*sigma_m/sigma_m_plus_one *(d*x+a*r) - 
	 sigma_m_minus_one/sigma_m_plus_one*y ;
       y = x; x= p ;
       //*Qsq(QsqX,x,PLUS) ;
       Qsq(QsqX,x) ;
       r = b - QsqX ;

       m++;
       sigma_m_minus_one = sigma_m ;
       sigma_m = sigma_m_plus_one ;
     }
     QDPIO::cout<<"applyChebInv: iter: "<<m<<" 1/sigma: "<<1.0/sigma_m<<endl ;

   }

   //! Here is your apply function
   // use the Golub algorithm here
   void operator()(T& chi, const T& b, PlusMinus isign) const 
   {
     chi = zero ; // zero the initial guess. This way P(Qsq)*b is produced     
     applyChebInv(chi,b) ;
   }
   
   //! Apply the A or A_dagger piece: P(Qsq) = A_dagger(Qsq) * A(Qsq) 
   // use the root representation here
   void applyA(T& chi,const T& b, PlusMinus isign) const{

     int start, end ;
     chi = b ;
     switch (isign){
     case PLUS:
       start = 0 ;
       end = degree/2 ;
       break ;
     case MINUS:
       start = degree/2  ;
       end   = degree ;
       break ;
     }

     //QDPIO::cout<<start<<" "<<end<<endl ;

     
     Real c0 = sqrt(c_Zero) ;

     T tt = c0*b ;

     for(int i(start);i<end;i++){
       //QDPIO::cout<<" root, norm: "<<root[i]<<" "<<inv_c[i]<<endl ;
       //*Qsq(chi, tt, PLUS);
       Qsq(chi, tt);
       chi -= root[i] * tt;
       chi *= inv_c[i] ;
       tt = chi ;
     }

   }

 //! Apply the derivative of the linop
 void deriv(P& ds_u, const T& chi, const T& psi, PlusMinus isign) const
 {
   // There's a problem here. The interface wants to do something like
   //    chi^dag * \dot(Qsq) \psi
   // the derivs all expect to do the derivative on the Qsq leaving
   // open some color indices. The chi^dag and psi multiplications will
   // trace over the spin indices.
   // However, there is no guarantee in the interface that  chi = psi,
   // so you **MUST** apply all the parts of the monomial each to psi
   // and to chi. Sorry, current limitation. In the future, we could
   // add another deriv function that takes only one fermion and you
   // could optimize here.

   // So do something like
   ds_u.resize(Nd);
   ds_u = zero;
   P  ds_tmp;

   multi1d<T> chi_products(degree+1);
   multi1d<T> psi_products(degree+1);

   multi1d<T> M_chi_products(degree+1);
   multi1d<T> M_psi_products(degree+1);

   // Build up list of products of roots of poly against psi and chi each
   // play some trick trying to pad boundaries

   Real c0 = sqrt(c_Zero) ;

   chi_products[0] = c0*chi;
   psi_products[0] = c0*psi;

   for(int n(1); n < degree+1; ++n)
   {
      Qsq(chi_products[n], chi_products[n-1]);
      chi_products[n] -= root[n] * chi_products[n-1];
      chi_products[n] *= inv_c[n] ;

      Qsq(psi_products[n], psi_products[n-1]);
      psi_products[n] -= root[n] * psi_products[n-1];
      psi_products[n] *= inv_c[n] ;

   }
   for(int n(0); n < degree+1; ++n)
     {
       (*M)(M_chi_products[n],chi_products[n],PLUS);
       (*M)(M_psi_products[n],psi_products[n],PLUS);
     }

   // Now do force computation
   // be more careful than me about bouds
   for(int n(0); n < degree; ++n)
     {
       M->deriv(ds_tmp, M_chi_products[degree-n], psi_products[n], PLUS);
       ds_u += ds_tmp;   // build up force
       M->deriv(ds_tmp, chi_products[degree-n], M_psi_products[n], MINUS);
       ds_u += ds_tmp;   // build up force
     }

 }

 
};

}// End Namespace Chroma

#endif 
