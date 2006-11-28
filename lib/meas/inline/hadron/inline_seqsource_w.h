// -*- C++ -*-
// $Id: inline_seqsource_w.h,v 3.2 2006-11-28 19:30:30 edwards Exp $
/*! \file
 * \brief Inline construction of sequential sources
 *
 * Sequential source construction
 */

#ifndef __inline_seqsource_w_h__
#define __inline_seqsource_w_h__

#include "chromabase.h"
#include "meas/inline/abs_inline_measurement.h"
#include "io/qprop_io.h"

namespace Chroma 
{ 
  /*! \ingroup inlinehadron */
  namespace InlineSeqSourceEnv 
  {
    extern const std::string name;
    bool registerAll();
  }

  //! Parameter structure
  /*! \ingroup inlinehadron */
  struct InlineSeqSourceParams 
  {
    InlineSeqSourceParams();
    InlineSeqSourceParams(XMLReader& xml_in, const std::string& path);
    void write(XMLWriter& xml_out, const std::string& path);

    unsigned long      frequency;

    SeqSource_t        param;
    PropSinkSmear_t    sink_header;

    struct NamedObject_t
    {
      std::string            gauge_id;
      multi1d<std::string>   prop_ids;
      std::string            seqsource_id;
    } named_obj;
  };

  //! Compute a sequential source
  /*! \ingroup inlinehadron */
  class InlineSeqSource : public AbsInlineMeasurement 
  {
  public:
    ~InlineSeqSource() {}
    InlineSeqSource(const InlineSeqSourceParams& p) : params(p) {}
    InlineSeqSource(const InlineSeqSource& p) : params(p.params) {}

    unsigned long getFrequency(void) const {return params.frequency;}

    //! Do the measurement
    void operator()(const unsigned long update_no,
		    XMLWriter& xml_out); 

  private:
    InlineSeqSourceParams params;
  };

};

#endif
