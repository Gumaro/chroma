// -*- C++ -*-
// $Id: lwldslash_w_sse.h,v 1.11 2004-12-17 17:45:04 bjoo Exp $
/*! \file
 *  \brief Wilson Dslash linear operator
 */

#ifndef __lwldslash_sse_h__
#define __lwldslash_sse_h__

#include "linearop.h"

#include <sse_config.h>

using namespace QDP;

namespace Chroma 
{ 
//! General Wilson-Dirac dslash
/*!
 * \ingroup linop
 *
 * DSLASH
 *
 * This routine is specific to Wilson fermions!
 *
 * Description:
 *
 * This routine applies the operator D' to Psi, putting the result in Chi.
 *
 *	       Nd-1
 *	       ---
 *	       \
 *   chi(x)  :=  >  U  (x) (1 - isign gamma  ) psi(x+mu)
 *	       /    mu			  mu
 *	       ---
 *	       mu=0
 *
 *	             Nd-1
 *	             ---
 *	             \    +
 *                +    >  U  (x-mu) (1 + isign gamma  ) psi(x-mu)
 *	             /    mu			   mu
 *	             ---
 *	             mu=0
 *
 */

                                                                                
typedef PColorMatrix < RComplex <REAL>, Nc > PrimitiveSU3Matrix;

class SSEWilsonDslash : public DslashLinearOperator<LatticeFermion>
{
public:
  //! Empty constructor. Must use create later
  SSEWilsonDslash() {}

  //! Full constructor
  SSEWilsonDslash(const multi1d<LatticeColorMatrix>& u_) {create(u_);}

  //! Creation routine
  void create(const multi1d<LatticeColorMatrix>& u_);

  //! No real need for cleanup here
  ~SSEWilsonDslash();

  //! Subset is all here
  const OrderedSubset& subset() const {return all;}

  /**
   * Apply a dslash
   *
   * \param chi     result                                      (Write)
   * \param psi     source                                      (Read)
   * \param isign   D'^dag or D'  ( MINUS | PLUS ) resp.        (Read)
   * \param cb	    Checkerboard of OUTPUT vector               (Read) 
   *
   * \return The output of applying dslash on psi
   */
  void apply (LatticeFermion& chi, const LatticeFermion& psi, enum PlusMinus isign, int cb) const;


  //! Take deriv of D
  /*!
   * \param chi     left vector                                 (Read)
   * \param psi     right vector                                (Read)
   * \param isign   D'^dag or D'  ( MINUS | PLUS ) resp.        (Read)
   *
   * \return Computes   chi^dag * \dot(D} * psi  
   */
  void deriv(multi1d<LatticeColorMatrix>& ds_u, 
	     const LatticeFermion& chi, const LatticeFermion& psi, 
	     enum PlusMinus isign) const
  {
    ds_u.resize(Nd);

    multi1d<LatticeColorMatrix> ds_tmp(Nd);
    deriv(ds_u, chi, psi, isign, 0);
    deriv(ds_tmp, chi, psi, isign, 1);

    for(int mu=0; mu < Nd; mu++) {
      ds_u[mu] += ds_tmp[mu];
    }
  }


  //! Take deriv of D
  /*!
   * \param chi     left vector on cb                           (Read)
   * \param psi     right vector on 1-cb                        (Read)
   * \param isign   D'^dag or D'  ( MINUS | PLUS ) resp.        (Read)
   * \param cb	    Checkerboard of chi vector                  (Read)
   *
   * \return Computes   chi^dag * \dot(D} * psi  
   */
  void deriv(multi1d<LatticeColorMatrix>& ds_u, 
	     const LatticeFermion& chi, const LatticeFermion& psi, 
	     enum PlusMinus isign, int cb) const;

private:
  multi1d<PrimitiveSU3Matrix> packed_gauge;
  
  multi1d<LatticeColorMatrix> u;   // should not need this

// Real CoeffWilsr_s;
};


}; // End Namespace Chroma

using namespace Chroma;

#endif
