// -*- C++ -*-
// $Id: prec_two_flavor_hasenbusch_monomial_w.h,v 2.1 2006-01-02 20:50:17 bjoo Exp $
/*! @file
 * @brief Two-flavor collection of even-odd preconditioned 4D ferm monomials
 */

#ifndef __prec_two_flavor_hasenbusch_monomial_w_h__
#define __prec_two_flavor_hasenbusch_monomial_w_h__

#include "update/molecdyn/field_state.h"
#include "update/molecdyn/monomial/two_flavor_hasenbusch_monomial_w.h"

namespace Chroma 
{

  /*! @ingroup monomial */
  namespace EvenOddPrecTwoFlavorHasenbuschWilsonTypeFermMonomialEnv 
  {
    extern const bool registered;
  };

  // Parameter structure
  /*! @ingroup monomial */
  struct EvenOddPrecTwoFlavorHasenbuschWilsonTypeFermMonomialParams {
    // Base Constructor
    EvenOddPrecTwoFlavorHasenbuschWilsonTypeFermMonomialParams();

    // Read monomial from some root path
    EvenOddPrecTwoFlavorHasenbuschWilsonTypeFermMonomialParams(XMLReader& in, const std::string&  path);
    InvertParam_t inv_param; // Inverter Parameters
    string ferm_act;
    string ferm_act_prec;
    string predictor_xml;   // The Chrono Predictor XML

  };

  void read(XMLReader& xml, const string& path, EvenOddPrecTwoFlavorHasenbuschWilsonTypeFermMonomialParams& param);

  void write(XMLWriter& xml, const string& path, const EvenOddPrecTwoFlavorHasenbuschWilsonTypeFermMonomialParams& params);


  //! Wrapper class for  2-flavor even-odd prec ferm monomials
  /*! @ingroup monomial
   *
   * Monomial is expected to be the same for these fermacts
   */
  class EvenOddPrecTwoFlavorHasenbuschWilsonTypeFermMonomial :
    public  TwoFlavorExactEvenOddPrecHasenbuschWilsonTypeFermMonomial< 
    multi1d<LatticeColorMatrix>,
    multi1d<LatticeColorMatrix>,
    LatticeFermion>
    {
    public: 
      // Construct out of a parameter struct. Check against the desired FermAct name
      EvenOddPrecTwoFlavorHasenbuschWilsonTypeFermMonomial(const string& fermact_name, 
						   const EvenOddPrecTwoFlavorHasenbuschWilsonTypeFermMonomialParams& param_);

      // Construct from a fermact handle and inv params
      // FermAct already holds BC-s
//      EvenOddPrecTwoFlavorHasenbuschWilsonTypeFermMonomial(Handle< const EvenOddPrecWilsonFermAct >& fermact_, const InvertParam_t& inv_param_ ) : fermact(fermact_), inv_param(inv_param_) {}

      // Copy Constructor
      EvenOddPrecTwoFlavorHasenbuschWilsonTypeFermMonomial(const EvenOddPrecTwoFlavorHasenbuschWilsonTypeFermMonomial& m) : phi(m.phi), fermact(m.fermact), fermact_prec(m.fermact_prec), inv_param(m.inv_param), chrono_predictor(m.chrono_predictor) {}

      const EvenOddPrecWilsonTypeFermAct< LatticeFermion, multi1d<LatticeColorMatrix> >& debugGetFermAct(void) const { 
	return getFermAct();
      }
      
 
      //! Even even contribution (eg ln det Clover)
      Double S_even_even(const AbsFieldState<multi1d<LatticeColorMatrix>,
			                     multi1d<LatticeColorMatrix> >& s) {
	return Double(0);
      }


    protected:

      LatticeFermion& getPhi(void) {
	return phi;
      }

      const LatticeFermion& getPhi(void) const {
	return phi;
      }

      const EvenOddPrecWilsonTypeFermAct< LatticeFermion, multi1d<LatticeColorMatrix> >& getFermAct(void) const { 
	return *fermact;
      }

      const EvenOddPrecWilsonTypeFermAct< LatticeFermion, multi1d<LatticeColorMatrix> >& getFermActPrec(void) const { 
	return *fermact_prec;
      }


      AbsChronologicalPredictor4D<LatticeFermion>& getMDSolutionPredictor(void) { 
	return *chrono_predictor;
      };

    //! Get parameters for the inverter
      const InvertParam_t getInvParams(void) const { 
	return inv_param;
      }

    private:
 
      // Hide empty constructor and =
      EvenOddPrecTwoFlavorHasenbuschWilsonTypeFermMonomial();
      void operator=(const EvenOddPrecTwoFlavorHasenbuschWilsonTypeFermMonomial&);

      // Pseudofermion field phi
      LatticeFermion phi;

      // A handle for the EvenOddPrecWilsonFermAct
      Handle<const EvenOddPrecWilsonTypeFermAct< LatticeFermion, multi1d<LatticeColorMatrix> > > fermact;

      Handle<const EvenOddPrecWilsonTypeFermAct< LatticeFermion, multi1d<LatticeColorMatrix> > > fermact_prec;

      // The parameters for the inversion
      InvertParam_t inv_param;

      Handle<AbsChronologicalPredictor4D<LatticeFermion> > chrono_predictor;
    };


}; //end namespace chroma

#endif
