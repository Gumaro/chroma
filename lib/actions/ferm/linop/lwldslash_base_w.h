// -*- C++ -*-
// $Id: lwldslash_base_w.h,v 1.1 2004-12-20 03:59:31 edwards Exp $
/*! \file
 *  \brief Wilson Dslash linear operator
 */

#ifndef __lwldslash_base_h__
#define __lwldslash_base_h__

#include "linearop.h"

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

  class WilsonDslashBase : public DslashLinearOperator<LatticeFermion>
  {
  public:
    //! No real need for cleanup here
    virtual ~WilsonDslashBase() {}

    //! Subset is all here
    const OrderedSubset& subset() const {return all;}

    //! Take deriv of D
    /*!
     * \param chi     left vector                                 (Read)
     * \param psi     right vector                                (Read)
     * \param isign   D'^dag or D'  ( MINUS | PLUS ) resp.        (Read)
     *
     * \return Computes   chi^dag * \dot(D} * psi  
     */
    virtual void deriv(multi1d<LatticeColorMatrix>& ds_u, 
		       const LatticeFermion& chi, const LatticeFermion& psi, 
		       enum PlusMinus isign) const;


    //! Take deriv of D
    /*!
     * \param chi     left vector on cb                           (Read)
     * \param psi     right vector on 1-cb                        (Read)
     * \param isign   D'^dag or D'  ( MINUS | PLUS ) resp.        (Read)
     * \param cb      Checkerboard of chi vector                  (Read)
     *
     * \return Computes   chi^dag * \dot(D} * psi  
     */
    virtual void deriv(multi1d<LatticeColorMatrix>& ds_u, 
		       const LatticeFermion& chi, const LatticeFermion& psi, 
		       enum PlusMinus isign, int cb) const;

  protected:
    //! Get the u field
    virtual const multi1d<LatticeColorMatrix>& getU() const = 0;

  };


}; // End Namespace Chroma

using namespace Chroma;

#endif
