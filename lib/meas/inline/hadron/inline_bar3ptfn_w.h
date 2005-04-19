// -*- C++ -*-
// $Id: inline_bar3ptfn_w.h,v 1.3 2005-04-19 20:05:22 edwards Exp $
/*! \file
 * \brief Inline measurement of bar3ptfn
 *
 * Form-factors
 */

#ifndef __inline_bar3ptfn_h__
#define __inline_bar3ptfn_h__

#include "chromabase.h"
#include "meas/inline/abs_inline_measurement.h"

namespace Chroma 
{ 
  /*! \ingroup inlinehadron */
  namespace InlineBar3ptfnEnv 
  {
    extern const std::string name;
    extern const bool registered;
  }

  //! Parameter structure
  /*! \ingroup inlinehadron */
  struct InlineBar3ptfnParams 
  {
    InlineBar3ptfnParams();
    InlineBar3ptfnParams(XMLReader& xml_in, const std::string& path);
    void write(XMLWriter& xml_out, const std::string& path);

    unsigned long      frequency;

    struct Param_t
    {
      int              mom2_max;          // (mom)^2 <= mom2_max. mom2_max=7 in szin.
      multi1d<int>     nrow;
    } param;

    struct Prop_t
    {
      string           prop_file;  // The files is expected to be in SciDAC format!
      multi1d<string>  seqprop_files;  // The files is expected to be in SciDAC format!
      string           bar3ptfn_file;
    } prop;

  };


  //! Inline measurement of Wilson loops
  /*! \ingroup inlinehadron */
  class InlineBar3ptfn : public AbsInlineMeasurement 
  {
  public:
    ~InlineBar3ptfn() {}
    InlineBar3ptfn(const InlineBar3ptfnParams& p) : params(p) {}
    InlineBar3ptfn(const InlineBar3ptfn& p) : params(p.params) {}

    unsigned long getFrequency(void) const {return params.frequency;}

    //! Do the measurement
    void operator()(const multi1d<LatticeColorMatrix>& u,
		    XMLBufferWriter& gauge_xml,
		    const unsigned long update_no,
		    XMLWriter& xml_out); 

  private:
    InlineBar3ptfnParams params;
  };

};

#endif
