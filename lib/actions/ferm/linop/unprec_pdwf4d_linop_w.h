// -*- C++ -*-
// $Id: unprec_pdwf4d_linop_w.h,v 1.1 2004-11-17 16:53:23 edwards Exp $
/*! \file
 *  \brief Unpreconditioned projected DWF operator to 4D using prec 5D bits
 */

#ifndef __unprec_pdwf4d_linop_w_h__
#define __unprec_pdwf4d_linop_w_h__

#include "linearop.h"
#include "handle.h"
#include "invtype.h"

using namespace QDP;
using namespace Chroma;


namespace Chroma
{
  //! Unpreconditioned projected DWF operator to 4D, using prec. 5D pieces
  /*!
   * \ingroup linop
   */
  template<typename T>
  class UnprecPDWF4DLinOp : public LinearOperator<T>
  {
  public:
    //! Initialize pointer with existing pointer
    /*! Requires that the pointer p is a return value of new */
    UnprecPDWF4DLinOp(const EvenOddPrecLinearOperator< multi1d<T> >* D_, 
		      const LinearOperator< multi1d<T> >* PV_,
		      const InvertParam_t& invParam_) : 
      D(D_), PV(PV_), invParam(invParam_) {}

    //! Copy pointer (one more owner)
    UnprecPDWF4DLinOp(Handle<const EvenOddPrecLinearOperator< multi1d<T> > > D_, 
		      Handle<const LinearOperator< multi1d<T> > > PV_,
		      const InvertParam_t& invParam) : 
      D(D_), PV(PV_), invParam(invParam_) {}

    //! Destructor
    ~UnprecPDWF4DLinOp() {}

    //! Length of internal 5D
    int size() const {return D->size();}

    //! Operator lives on the entire lattice
    inline const OrderedSubset& subset() const {return all;}

    //! Apply the operator onto a source vector
    /*! For this operator, the sign is ignored */
    void operator() (T& chi, const T& psi, enum PlusMinus isign) const;

  private:
    const Handle< const EvenOddPrecLinearOperator< multi1d<T> > > D;
    const Handle< const LinearOperator< multi1d<T> > > PV;
    const InvertParam_t& invParam;
  };

}

using namespace Chroma;

#endif
