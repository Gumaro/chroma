// -*- C++ -*-
// $Id: schr_sf_fermbc_w.h,v 3.0 2006-04-03 04:58:48 edwards Exp $
/*! @file
 * @brief Schroedinger ferm boundary conditions
 */

#ifndef __schr_sf_fermbc_w_h__
#define __schr_sf_fermbc_w_h__

#include "actions/ferm/fermbcs/schroedinger_fermbc_w.h"

namespace Chroma
{

  //! Abstract class for SOME Schroedinger ferm BC
  /*! @ingroup fermbcs
   *
   *  Schroedinger BC implies periodic in dirs orthog to decay dir, and some
   *  kind of fixed BC in the decay dir.
   */
  class SchrSFFermBC : public SchrFermBC
  {
  public:
    //! Virtual destructor
    virtual ~SchrSFFermBC() {}

    //! Decay direction
    virtual int getDir() const = 0;

  protected:
    //! Construct the mask and boundary fields
    virtual void initBnd(multi1d<LatticeColorMatrix>& SFBndFld,
			 multi1d<LatticeBoolean>& lSFmask,
			 LatticeBoolean& lSFmaskF,
			 const multi1d<LatticeColorMatrix>& SFBndFldG,
			 const multi1d<LatticeBoolean>& lSFmaskG) const;

    //! Get the angles on the boundaries
    virtual const multi1d<Real>& getTheta() const = 0;
  };

}



#endif
