// -*- C++ -*-
// $Id: remez_gmp.h,v 1.1 2005-02-01 21:23:10 edwards Exp $
/*! \file
 *  \brief Remez algorithm for finding nth roots
 */

#ifndef __remez_gmp_h__
#define __remez_gmp_h__

#include "chromabase.h"
#include "actions/ferm/fermacts/bigfloat.h"

namespace Chroma
{

  //! Remez algorithm 
  /*!
   * The Remez algorithm is used for generating the nth root
   * rational approximation.
   *
   * Note this class can only be used if
   * the gnu multiprecision (GNU MP) library is present.
   *
   */
  class RemezGMP
  {
  public:
    //! Default constructor
    RemezGMP();

    //! Constructor
    RemezGMP(const Real& lower, const Real& upper, long prec);

    //! Destructor
    ~RemezGMP() {}

    //! Reset the bounds of the approximation
    void setBounds(const Real& lower, const Real& upper);

    //! Generate the rational approximation x^(pnum/pden)
    Real generateApprox(int num_degree, int den_degree, unsigned long power_num, unsigned long power_den);
    Real generateApprox(int degree, unsigned long power_num, unsigned long power_den);

    //! Return the partial fraction expansion of the approximation x^(pnum/pden)
    int getPFE(multi1d<Real>& res, multi1d<Real>& pole, Real& norm);

    //! Return the partial fraction expansion of the approximation x^(-pnum/pden)
    int getIPFE(multi1d<Real>& res, multi1d<Real>& pole, Real& norm);

  private:
    // The approximation parameters
    multi1d<bigfloat> param, roots, poles;
    bigfloat norm;

    //! The numerator and denominator degree (n=d)
    int n, d;
  
    //! The bounds of the approximation
    bigfloat apstrt, apwidt, apend;

    //! the numerator and denominator of the power we are approximating
    unsigned long power_num; 
    unsigned long power_den;

    //! Flag to determine whether the arrays have been allocated
    bool alloc;

    //! Variables used to calculate the approximation
    int nd1, iter;
    multi1d<bigfloat> xx, mm, step;
    bigfloat delta, spread, tolerance;

    //! The number of equations we must solve at each iteration (n+d+1)
    int neq;

    //! The precision of the GNU MP library
    long prec;

    //! Initial values of maximal and minmal errors
    void initialGuess();

    //! Solve the equations
    void equations();

    //! Search for error maxima and minima
    void search(multi1d<bigfloat>& step); 

    //! Initialise step sizes
    void stpini(multi1d<bigfloat>& step);

    //! Calculate the roots of the approximation
    int root();

    //! Evaluate the polynomial
    bigfloat polyEval(const bigfloat& x, const multi1d<bigfloat>& poly, long size);

    //! Evaluate the differential of the polynomial
    bigfloat polyDiff(const bigfloat& x, const multi1d<bigfloat>& poly, long size);

    //! Newton's method to calculate roots
    bigfloat rtnewt(const multi1d<bigfloat>& poly, long i, 
		    const bigfloat& x1, const bigfloat& x2, const bigfloat& xacc);

    // Evaluate the partial fraction expansion of the rational function
    // with res roots and poles poles.  Result is overwritten on input
    // arrays.
    void pfe(multi1d<bigfloat>& res, multi1d<bigfloat>& poles, const bigfloat& norm);

    // Evaluate the rational form P(x)/Q(x) using coefficients from the
    // solution vector param
    bigfloat approx(const bigfloat& x);

    //! Calculate function required for the approximation
    bigfloat func(const bigfloat& x);

    //! Compute size and sign of the approximation error at x
    bigfloat getErr(const bigfloat& x, int& sign);

    //! Solve the system AX=B
    int simq(multi1d<bigfloat>& A, multi1d<bigfloat>& B, multi1d<bigfloat>& X, int n);

    //! Free memory and reallocate as necessary
    void allocate(int num_degree, int den_degree);
  };

}  // namespace Chroma

#endif  // include guard



