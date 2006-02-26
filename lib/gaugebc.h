// -*- C++ -*-
// $Id: gaugebc.h,v 2.1 2006-02-26 03:47:51 edwards Exp $
/*! @file
 * @brief Gauge boundary conditions
 */

#ifndef __gaugebc_h__
#define __gaugebc_h__

#include "chromabase.h"
#include "io/enum_io/enum_gaugebc_io.h"


namespace Chroma
{
  //! Base class for all gauge action boundary conditions
  /*! @ingroup gaugebc
   *
   */
  class GaugeBC
  {
  public:
    //! Virtual destructor to help with cleanup;
    virtual ~GaugeBC() {}

    //! Apply the BC onto the U fields in place
    virtual void modify(multi1d<LatticeColorMatrix>& u) const = 0;

    //! Zero the U fields in place on the masked links
    virtual void zero(multi1d<LatticeColorMatrix>& u) const = 0;

#if defined(EXPOSE_THIS_STUFF)
    // NOT SURE THIS STUFF IS ABSOLUTELY REQUIRED - TRY TO AVOID EXPOSING THIS
    //! Mask which lattice sites have fixed gauge links
    virtual const multi1d<LatticeBoolean>& lbmaskU() const = 0;

    //! Fixed gauge links on only the lbmaskU() sites
    virtual const multi1d<LatticeColorMatrix>& lFldU() const = 0;
#endif

    //! Says if there are fixed links within the lattice
    virtual bool nontrivialP() const = 0;
  };


}

#endif
