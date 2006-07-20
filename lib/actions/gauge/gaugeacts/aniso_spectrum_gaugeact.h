// -*- C++ -*-
// $Id: aniso_spectrum_gaugeact.h,v 1.2 2006-07-20 19:53:00 edwards Exp $
/*! \file
 *  \brief Anisotropic gaugeact useful for spectrum from hep-lat/9911003
 *
 *  Tree-level LW with tapole improvement, missing 1x2 in time, also including
 *  2-plaq term. Taken from Morningstar-Peardon, hep-lat/9911003
 */

#ifndef __aniso_spectrum_gaugeact_h__
#define __aniso_spectrum_gaugeact_h__

#include "gaugeact.h"
#include "gaugebc.h"
#include "actions/gauge/gaugeacts/plaq_gaugeact.h"
#include "actions/gauge/gaugeacts/rect_gaugeact.h"
#include "actions/gauge/gaugeacts/spatial_two_plaq_gaugeact.h"


namespace Chroma
{

  /*! @ingroup gaugeacts */
  namespace AnisoSpectrumGaugeActEnv 
  {
    extern const string name;
    extern const bool registered;
  }

  //! Parameter structure
  /*! @ingroup gaugeacts */
  struct AnisoSpectrumGaugeActParams 
  {
    // Base Constructor
    AnisoSpectrumGaugeActParams();
    
    // Read params from some root path
    AnisoSpectrumGaugeActParams(XMLReader& xml_in, const std::string& path);

    Real beta;  
    Real u0s;
    Real u0t;
    Real omega;
    AnisoParam_t aniso;
  };
  
  /*! @ingroup gaugeacts */
  void read(XMLReader& xml, const string& path, AnisoSpectrumGaugeActParams& param);
  
  /*! @ingroup gaugeacts */
  void write(XMLWriter& xml, const string& path, const AnisoSpectrumGaugeActParams& param);
  

  //! MP gauge action
  /*! \ingroup gaugeacts
   *
   *  Anisotropic gaugeact useful for spectrum from hep-lat/9911003
   *
   *  Tree-level LW with tapole improvement, missing 1x2 in time, also including
   *  2-plaq term. Taken from Morningstar-Peardon, hep-lat/9911003
   */

  class AnisoSpectrumGaugeAct : public LinearGaugeAction
  {
  public:
    // Typedefs to save typing
    typedef multi1d<LatticeColorMatrix>  P;
    typedef multi1d<LatticeColorMatrix>  Q;

    //! Read beta from a param struct
    AnisoSpectrumGaugeAct(Handle< CreateGaugeState<P,Q> > cgs_, 
			  const AnisoSpectrumGaugeActParams& p) :
      param(p) {init(cgs_);}

    //! Is anisotropy used?
    bool anisoP() const {return param.aniso.anisoP; }

    //! Anisotropy factor
    const Real anisoFactor() const {return param.aniso.xi_0;}

    //! Anisotropic direction
    int tDir() const {return param.aniso.t_dir;}

    //! Return the set on which the gauge action is defined
    /*! Defined on the even-off (red/black) set */
    const OrderedSet& getSet() const {return rb;}

    //! Compute staple
    /*! Default version. Derived class should override this if needed. */
    void staple(LatticeColorMatrix& result,
		const Handle< GaugeState<P,Q> >& state,
		int mu, int cb) const
      {
	plaq->staple(result,state,mu,cb);

	LatticeColorMatrix tmp;
	rect->staple(tmp,state,mu,cb);     // This may fail I think in aniso mode
	result += tmp;
	two_plaq->staple(tmp,state,mu,cb); // This will fail I think
	result += tmp;
      }

    //! Compute dS/dU
    void deriv(multi1d<LatticeColorMatrix>& result,
	       const Handle< GaugeState<P,Q> >& state) const
      {
	plaq->deriv(result,state);

	multi1d<LatticeColorMatrix> tmp;
	rect->deriv(tmp,state);
	result += tmp;

	two_plaq->deriv(tmp,state);
	result += tmp;

      }

    //! Compute the actions
    Double S(const Handle< GaugeState<P,Q> >& state) const
      {
	return plaq->S(state) + rect->S(state) + two_plaq->S(state);
      }

    //! Produce a gauge create state object
    const CreateGaugeState<P,Q>& getCreateState() const {return plaq->getCreateState();}

    //! Destructor is automatic
    ~AnisoSpectrumGaugeAct() {}

    // Accessors -- non mutable members.
    const Real getBeta(void) const { return param.beta; }
    
    const Real getU0S(void) const { return param.u0s; }
    
    const Real getU0T(void) const { return param.u0t; }

    const Real getOmega(void) const { return param.omega; }


  protected:
    //! Private initializer
    void init(Handle< CreateGaugeState<P,Q> > cgs);

    //! Hide assignment
    void operator=(const AnisoSpectrumGaugeAct& a) {}

  private:
    AnisoSpectrumGaugeActParams    param;    /*!< The couplings and anisotropy*/
    Handle<PlaqGaugeAct>           plaq;     /*!< Hold a plaquette gaugeact */
    Handle<RectGaugeAct>           rect;     /*!< Hold a rectangle gaugeact */
    Handle<SpatialTwoPlaqGaugeAct> two_plaq; /*!< Hold spatial plaquettes separated in time type gaugeact */
  };

};


#endif
