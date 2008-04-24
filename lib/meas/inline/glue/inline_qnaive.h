/*! \file
 *  \brief Inline naive topological charge
 */

#ifndef INLINE_QNAIVE_H
#define INLINE_QNAIVE_H

#include "chromabase.h"
#include "meas/inline/abs_inline_measurement.h"
#include "io/xml_group_reader.h"

namespace Chroma 
{ 
  /*! \ingroup inlineglue */
  namespace InlineQTopEnv 
  {
    extern const std::string name;
    bool registerAll();
  }


  /*! \ingroup inlineglue */
  struct InlineQTopParams 
  {
    InlineQTopParams();
    InlineQTopParams(XMLReader& xml_in, const std::string& path);

    unsigned long frequency;

    struct Param_t
    {
      GroupXML_t    cgs;      /*!< Gauge State */
      Real          k5;
    } param;

    struct NamedObject_t
    {
      std::string   gauge_id;
    } named_obj;
  };


  /*! \ingroup inlineglue */
  class InlineQTop : public AbsInlineMeasurement 
  {
  public:
    ~InlineQTop() {}
    InlineQTop(const InlineQTopParams& p) : params(p) {}
    InlineQTop(const InlineQTop& p) : params(p.params) {}

    unsigned long getFrequency(void) const {return params.frequency;}

    void operator()(unsigned long update_no,
		    XMLWriter& xml_out); 

  private:
    InlineQTopParams params;
  };

};

#endif
