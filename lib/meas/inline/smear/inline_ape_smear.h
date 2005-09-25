// -*- C++ -*-
// $Id: inline_ape_smear.h,v 1.3 2005-09-25 20:41:09 edwards Exp $
/*! \file
 *  \brief Inline APE smearing
 */

#ifndef __inline_ape_smear_h__
#define __inline_ape_smear_h__

#include "chromabase.h"
#include "meas/inline/abs_inline_measurement.h"

namespace Chroma 
{ 
  /*! \ingroup inlinesmear */
  namespace InlineAPESmearEnv 
  {
    extern const std::string name;
    extern const bool registered;
  }

  //! Parameter structure
  /*! \ingroup inlinesmear */
  struct InlineAPESmearParams 
  {
    InlineAPESmearParams();
    InlineAPESmearParams(XMLReader& xml_in, const std::string& path);
    void write(XMLWriter& xml_out, const std::string& path);

    unsigned long frequency;

    struct Param_t
    {
      int link_smear_num;
      Real link_smear_fact;		// Smearing parameters

      int j_decay;			// Decay direction
      multi1d<int> nrow;		// Lattice dimension
    } param;

    struct NamedObject_t
    {
      std::string 	ape_id;	        // memory object ape config
    } named_obj;

  };

  //! Inline measurement of Wilson loops
  /*! \ingroup inlinesmear */
  class InlineAPESmear : public AbsInlineMeasurement 
  {
  public:
    ~InlineAPESmear() {}
    InlineAPESmear(const InlineAPESmearParams& p) : params(p) {}
    InlineAPESmear(const InlineAPESmear& p) : params(p.params) {}

    unsigned long getFrequency(void) const {return params.frequency;}

    //! Do the measurement
    void operator()(const multi1d<LatticeColorMatrix>& u,
		    XMLBufferWriter& gauge_xml,
		    const unsigned long update_no,
		    XMLWriter& xml_out); 

  private:
    InlineAPESmearParams params;
  };

};

#endif
