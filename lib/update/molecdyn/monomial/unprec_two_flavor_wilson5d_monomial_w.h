// -*- C++ -*-
// $Id: unprec_two_flavor_wilson5d_monomial_w.h,v 1.1 2005-01-13 15:55:04 bjoo Exp $

/*! @file
 * @brief Two-flavor collection of unpreconditioned 5D ferm monomials
 */

#ifndef UNPREC_TWO_FLAVOR_WILSON_TYPE_MONOMIAL5D_H
#define UNPREC_TWO_FLAVOR_WILSON_TYPE_MONOMIAL5D_H

#include "chromabase.h"

#include "update/molecdyn/field_state.h"
#include "update/molecdyn/abs_monomial.h"

namespace Chroma 
{

  namespace UnprecTwoFlavorWilsonTypeFermMonomial5DEnv 
  {
    extern const bool registered;
  };

  // Parameter structure
  struct UnprecTwoFlavorWilsonTypeFermMonomial5DParams 
  {
    // Base Constructor
    UnprecTwoFlavorWilsonTypeFermMonomial5DParams();

    // Read monomial from some root path
    UnprecTwoFlavorWilsonTypeFermMonomial5DParams(XMLReader& in, const std::string&  path);
    InvertParam_t inv_param; // Inverter Parameters
    string ferm_act;
    string predictor_xml;
  };

  void read(XMLReader& xml, const string& path, UnprecTwoFlavorWilsonTypeFermMonomial5DParams& param);

  void write(XMLWriter& xml, const string& path, const UnprecTwoFlavorWilsonTypeFermMonomial5DParams& params);

  //! Wrapper class for 5D 2-flavor unprec ferm monomials
  /*!
   * Monomial is expected to be the same for these fermacts
   */
  class UnprecTwoFlavorWilsonTypeFermMonomial5D :
    public  TwoFlavorExactUnprecWilsonTypeFermMonomial5D< 
    multi1d<LatticeColorMatrix>,
    multi1d<LatticeColorMatrix>,
    LatticeFermion>
    {
    public: 
      // Construct out of a parameter struct. Check against the desired FermAct name
      UnprecTwoFlavorWilsonTypeFermMonomial5D(const string& fermact_name, 
					      const UnprecTwoFlavorWilsonTypeFermMonomial5DParams& param_);

      // Construct from a fermact handle and inv params
      // FermAct already holds BC-s
//      UnprecTwoFlavorWilsonTypeFermMonomial5D(const std::string& FermAct, 
//					      Handle< const UnprecWilsonTypeFermAct5D >& fermact_, 
//					      const InvertParam_t& inv_param_) : 
//	fermact(fermact_), inv_param(inv_param_) {init(FermAct);}

      // Copy Constructor
      UnprecTwoFlavorWilsonTypeFermMonomial5D(const UnprecTwoFlavorWilsonTypeFermMonomial5D& m) : phi(m.phi), fermact(m.fermact), inv_param(m.inv_param), chrono_predictor(m.chrono_predictor) {}

      const multi1d<LatticeFermion>& debugGetPhi(void) const {
	return getPhi();
      }

      void debugGetX(multi1d<LatticeFermion>& X, const AbsFieldState<multi1d<LatticeColorMatrix>, multi1d<LatticeColorMatrix> >& s) const {
	getX(X,s);
      }

      const UnprecWilsonTypeFermAct5D< LatticeFermion, multi1d<LatticeColorMatrix> >& debugGetFermAct(void) const { 
	return getFermAct();
      }
      

    protected:

      multi1d<LatticeFermion>& getPhi(void) {
	return phi;
      }

      const multi1d<LatticeFermion>& getPhi(void) const {
	return phi;
      }

      const UnprecWilsonTypeFermAct5D< LatticeFermion, multi1d<LatticeColorMatrix> >& getFermAct(void) const { 
	return *fermact;
      }

      //! Do inversion M^dag M X = phi
      int getX(multi1d<LatticeFermion>& X, 
		const AbsFieldState<multi1d<LatticeColorMatrix>, multi1d<LatticeColorMatrix> >& s) const ;

      
      //! Get X = (PV^dag*PV)^{-1} eta
      int getXPV(multi1d<LatticeFermion>& X, const multi1d<LatticeFermion>& eta, 
		  const AbsFieldState<multi1d<LatticeColorMatrix>, multi1d<LatticeColorMatrix> >& s) const;


      //! Get X = (A^dag*A)^{-1} eta
      int invert(multi1d<LatticeFermion>& X, 
		 const LinearOperator< multi1d<LatticeFermion> >& A,
		 const multi1d<LatticeFermion>& eta) const;

     
      AbsChronologicalPredictor5D< LatticeFermion >& getMDSolutionPredictor(void) {
	return *chrono_predictor;
      }

    private:
      // Hide empty constructor and =
      UnprecTwoFlavorWilsonTypeFermMonomial5D();
      void operator=(const UnprecTwoFlavorWilsonTypeFermMonomial5D&);

      // Pseudofermion field phi
      multi1d<LatticeFermion> phi;

      // A handle for the UnprecWilsonFermAct
      Handle<const UnprecWilsonTypeFermAct5D< LatticeFermion, multi1d<LatticeColorMatrix> > > fermact;

      // The parameters for the inversion
      InvertParam_t inv_param;

      // Chrono Predictor
      Handle < AbsChronologicalPredictor5D<LatticeFermion> > chrono_predictor;
    };


} //end namespace chroma



#endif
