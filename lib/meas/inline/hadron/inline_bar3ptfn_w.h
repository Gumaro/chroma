// -*- C++ -*-
// $Id: inline_bar3ptfn_w.h,v 1.4 2005-09-25 20:41:09 edwards Exp $
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

    struct NamedObject_t
    {
      string           prop_id;
      multi1d<string>  seqprop_ids;
      string           bar3ptfn_file;
    } named_obj;
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
