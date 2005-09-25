// -*- C++ -*-
// $Id: abs_inline_measurement.h,v 2.0 2005-09-25 21:04:36 edwards Exp $
/*! \file
 * \brief Abstract inline measurements
 */

#ifndef ABS_INLINE_MEASUREMENT_H
#define ABS_INLINE_MEASUREMENT_H

#include "chromabase.h"

namespace Chroma 
{ 

  /*! \ingroup inline */
  class AbsInlineMeasurement 
  {
  public:

    // Virtual Destructor
    virtual ~AbsInlineMeasurement(void) {}

    // Tell me how often I should measure this beastie
    virtual unsigned long getFrequency(void) const = 0;

    // Do the measurement
    virtual void operator()(const multi1d<LatticeColorMatrix>& u,
			    XMLBufferWriter& gauge_xml,
			    unsigned long update_no,
			    XMLWriter& xml_out) = 0;
  };

}; // End namespace

#endif

