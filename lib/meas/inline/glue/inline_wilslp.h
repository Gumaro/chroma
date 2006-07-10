// -*- C++ -*-
// $Id: inline_wilslp.h,v 3.3 2006-07-10 19:04:47 edwards Exp $
/*! \file
 *  \brief Inline Wilson loops
 */

#ifndef __inline_wilslp_h__
#define __inline_wilslp_h__

#include "chromabase.h"
#include "meas/inline/abs_inline_measurement.h"
#include "io/xml_group_reader.h"

namespace Chroma 
{ 
  /*! \ingroup inlineglue */
  namespace InlineWilsonLoopEnv 
  {
    extern const std::string name;
    extern const bool registered;
  }

  //! Parameter structure
  /*! \ingroup inlineglue */
  struct InlineWilsonLoopParams 
  {
    InlineWilsonLoopParams();
    InlineWilsonLoopParams(XMLReader& xml_in, const std::string& path);

    unsigned long frequency;

    struct Param_t
    {
      int           kind;
      int           j_decay;
      int           t_dir;
      GroupXML_t    gaugebc;
    } param;

    struct NamedObject_t
    {
      std::string   gauge_id;
    } named_obj;
  };


  //! Inline measurement of Wilson loops
  /*! \ingroup inlineglue */
  class InlineWilsonLoop : public AbsInlineMeasurement 
  {
  public:
    ~InlineWilsonLoop() {}
    InlineWilsonLoop(const InlineWilsonLoopParams& p) : params(p) {}
    InlineWilsonLoop(const InlineWilsonLoop& p) : params(p.params) {}

    unsigned long getFrequency(void) const {return params.frequency;}

    //! Do the measurement
    void operator()(const unsigned long update_no,
		    XMLWriter& xml_out); 

  private:
    InlineWilsonLoopParams params;
  };

};

#endif
