// -*- C++ -*-
// $Id: inline_list_obj.h,v 1.1 2005-09-24 21:14:28 edwards Exp $
/*! \file
 * \brief Inline task to write an object from a named buffer
 *
 * Named object writing
 */

#ifndef __inline_list_obj_h__
#define __inline_list_obj_h__

#include "chromabase.h"
#include "meas/inline/abs_inline_measurement.h"

namespace Chroma 
{ 
  /*! \ingroup inlineio */
  namespace InlineListNamedObjEnv 
  {
    extern const std::string name;
    extern const bool registered;
  }

  //! Parameter structure
  /*! \ingroup inlineio */
  struct InlineListNamedObjParams 
  {
    InlineListNamedObjParams();
    InlineListNamedObjParams(XMLReader& xml_in, const std::string& path);
    void write(XMLWriter& xml_out, const std::string& path);

    unsigned long frequency;

    struct NamedObject_t
    {
      std::string   object_id;
      std::string   object_type;
    } named_obj;
  };

  //! Inline writing of memory objects
  /*! \ingroup inlineio */
  class InlineListNamedObj : public AbsInlineMeasurement 
  {
  public:
    ~InlineListNamedObj() {}
    InlineListNamedObj(const InlineListNamedObjParams& p) : params(p) {}
    InlineListNamedObj(const InlineListNamedObj& p) : params(p.params) {}

    unsigned long getFrequency(void) const {return params.frequency;}

    //! Do the writing
    void operator()(const multi1d<LatticeColorMatrix>& u,
		    XMLBufferWriter& gauge_xml,
		    const unsigned long update_no,
		    XMLWriter& xml_out); 

  private:
    InlineListNamedObjParams params;
  };

};

#endif
