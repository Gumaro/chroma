// -*- C++ -*-
// $Id: inline_spectrum_s.h,v 3.5 2007-05-10 17:46:29 edwards Exp $
/*! \file
 * \brief Inline staggered spectrum calculations
 *
 * Staggered spectrum calculations
 */

#ifndef __inline_spectrum_s_h__
#define __inline_spectrum_s_h__

#include "chromabase.h"
#include "meas/inline/abs_inline_measurement.h"
#include "meas/hadron/enum_loops_s.h"
#include "actions/ferm/invert/syssolver_cg_params.h"

namespace Chroma 
{ 
  /*! \ingroup inlinehadron */
  namespace InlineStaggeredSpectrumEnv 
  {
    extern const std::string name;
    bool registerAll();
  }

  //! Parameter structure
  /*! \ingroup inlinehadron */
  struct InlineStaggeredSpectrumParams 
  {
    InlineStaggeredSpectrumParams();
    InlineStaggeredSpectrumParams(XMLReader& xml_in, const std::string& path);
    void write(XMLWriter& xml_out, const std::string& path);

    unsigned long frequency;

    struct Param_t
    {
      bool Meson_local;                 // Meson spectroscopy
      bool Baryon_local;                // Baryons spectroscopy
      bool Baryon_vary;                 // Baryons spectroscopy variational
      bool LocalPion_vary;              // local pion spectroscopy variational
      bool disconnected_local ;         // disconnected loops local
      bool disconnected_fuzz  ;         // disconnected loops fuzz
      bool ps4link_singlet_conn ;  
      bool ps4link_singlet_conn_fuzz ;  
      bool eight_pions;                 // all pseudoscalar meson tastes
      bool eight_scalars;               // all scalar meson tastes
      bool eight_rhos;                  // all vector meson tastes


      //Parameters for Gauge-fixing
      Real GFAccu;                      // Gauge fixing tolerance 
      Real OrPara;                      // Gauge fixing over-relaxation param
      int  GFMax;                       // Maximum gauge fixing iterations
 
      // parameters for disconnected loops
      int Nsamp;
      int CFGNO ;
      VolSrc_type volume_source ;
      bool gauge_invar_oper ; 
      bool sym_shift_oper ; 
      bool loop_checkpoint ; 

      int fuzz_width ; 

      int src_seperation ;

      multi1d<int> boundary;
      multi1d<int> nrow;
      multi1d<int> t_srce;

    } param;

    struct Quark_Prop_t
    {
      Real         Mass;      // Staggered mass
      Real         u0;        // Tadpole Factor

      SysSolverCGParams  invParam; // inversion parameters

    } prop_param ;

    struct NamedObject_t
    {
      std::string   gauge_id;
    } named_obj;

    std::string xml_file;  // Alternate XML file pattern
  };


  //! Inline measurement of Wilson loops
  /*! \ingroup inlinehadron */
  class InlineStaggeredSpectrum : public AbsInlineMeasurement 
  {
  public:
    ~InlineStaggeredSpectrum() {}
    InlineStaggeredSpectrum(const InlineStaggeredSpectrumParams& p) : params(p) {}
    InlineStaggeredSpectrum(const InlineStaggeredSpectrum& p) : params(p.params) {}

    unsigned long getFrequency(void) const {return params.frequency;}

    //! Do the measurement
    void operator()(const unsigned long update_no,
		    XMLWriter& xml_out); 

  protected:
    //! Do the measurement
    void func(const unsigned long update_no,
	      XMLWriter& xml_out); 

  private:
    InlineStaggeredSpectrumParams params;
  };

};

#endif
