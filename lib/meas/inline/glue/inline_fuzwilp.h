// -*- C++ -*-
// $Id: inline_fuzwilp.h,v 2.1 2006-02-20 20:42:38 edwards Exp $
/*! \file
 * \brief Inline fuzzed Wilson loops
 */

#ifndef __inline_fuzwilp_h__
#define __inline_fuzwilp_h__

#include "chromabase.h"
#include "meas/inline/abs_inline_measurement.h"

namespace Chroma 
{ 
  /*! \ingroup inlineglue */
  namespace InlineFuzzedWilsonLoopEnv 
  {
    extern const std::string name;
    extern const bool registered;
  }

  //! Parameter structure
  /*! \ingroup inlineglue */
  struct InlineFuzzedWilsonLoopParams 
  {
    InlineFuzzedWilsonLoopParams();
    InlineFuzzedWilsonLoopParams(XMLReader& xml_in, const std::string& path);

    unsigned long frequency;
    int j_decay;
    int n_smear;
    int BlkMax;
    Real sm_fact;
    Real BlkAccu;
  };

  //! Inline measurement of fuzzed Wilson loops
  /*! \ingroup inlineglue */
  class InlineFuzzedWilsonLoop : public AbsInlineMeasurement 
  {
  public:
    ~InlineFuzzedWilsonLoop() {}
    InlineFuzzedWilsonLoop(const InlineFuzzedWilsonLoopParams& p) : params(p) {}
    InlineFuzzedWilsonLoop(const InlineFuzzedWilsonLoop& p) : params(p.params) {}

    unsigned long getFrequency(void) const {return params.frequency;}

    //! Do the measurement
    void operator()(const multi1d<LatticeColorMatrix>& u,
		    XMLBufferWriter& gauge_xml,
		    const unsigned long update_no,
		    XMLWriter& xml_out); 

  private:
    InlineFuzzedWilsonLoopParams params;
  };

};

#endif
