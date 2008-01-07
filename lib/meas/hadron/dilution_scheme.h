// -*- C++ -*-
// $Id: dilution_scheme.h,v 1.1 2008-01-07 15:21:26 jbulava Exp $
/*! \file
 *  \brief Dilution Schemes
 */

#ifndef __dilution_scheme_h__
#define __dilution_scheme_h__

#include "chromabase.h"

namespace Chroma
{
  //! Abstract dilution scheme
  /*! @ingroup hadron
   *
   * Supports creation of (abstract) dilution schemes used in 
   * stochastic sources and solutions
   */
  template<typename T>
  class DilutionScheme
  {
  
	public:

    //! Virtual destructor to help with cleanup;
    virtual ~DilutionScheme() {}

 
    //! The decay direction
    virtual int getDecayDir() const = 0;

    //! The seed identifies this quark
    virtual const Seed& getSeed() const = 0;

    //! Does this element of the dilution scheme have support on timeslice t0?
    virtual bool hasSupport(int dil, int t0) const = 0;

    //! Return the diluted source vector
    /*! MAYBE THIS SHOULD BE A CONST REFERENCE?? PROBABLY NO */
    virtual T dilutedSource( int dil ) const = 0;
    
    //! Return the solution vector corresponding to the diluted source
    /*! MAYBE THIS SHOULD BE A CONST REFERENCE?? POSSIBLY YES */
    virtual T dilutedSolution( int dil ) const = 0;

  };

} // namespace Chroma


#endif
