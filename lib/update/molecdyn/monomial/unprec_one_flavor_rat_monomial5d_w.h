// -*- C++ -*-
// $Id: unprec_one_flavor_rat_monomial5d_w.h,v 3.0 2006-04-03 04:59:09 edwards Exp $
/*! @file
 * @brief One-flavor collection of unpreconditioned 5D ferm monomials
 */

#ifndef __unprec_one_flavor_rat_monomial5d_w_h__
#define __unprec_one_flavor_rat_monomial5d_w_h__

#include "update/molecdyn/field_state.h"
#include "update/molecdyn/monomial/one_flavor_rat_monomial5d_w.h"
#include "update/molecdyn/monomial/one_flavor_rat_monomial5d_params_w.h"

namespace Chroma 
{

  /*! @ingroup monomial */
  namespace UnprecOneFlavorWilsonTypeFermRatMonomial5DEnv 
  {
    extern const std::string name;
    extern const bool registered;
  };


  //! Wrapper class for 5D 2-flavor unprec ferm monomials
  /*! @ingroup monomial
   *
   * Monomial is expected to be the same for these fermacts
   */
  class UnprecOneFlavorWilsonTypeFermRatMonomial5D :
    public  OneFlavorRatExactUnprecWilsonTypeFermMonomial5D< 
    multi1d<LatticeColorMatrix>,
    multi1d<LatticeColorMatrix>,
    LatticeFermion>
    {
    public: 
      // Typedefs to save typing
      typedef LatticeFermion               T;
      typedef multi1d<LatticeColorMatrix>  P;
      typedef multi1d<LatticeColorMatrix>  Q;

      // Construct out of a parameter struct. Check against the desired FermAct name
      UnprecOneFlavorWilsonTypeFermRatMonomial5D(const OneFlavorWilsonTypeFermRatMonomial5DParams& param_);

      // Copy Constructor
      UnprecOneFlavorWilsonTypeFermRatMonomial5D(const UnprecOneFlavorWilsonTypeFermRatMonomial5D& m) : phi(m.phi), fermact(m.fermact), inv_param(m.inv_param), nthRoot(m.nthRoot), nthRootPV(m.nthRootPV) {}


    protected:

      const UnprecWilsonTypeFermAct5D<T,P,Q>& getFermAct(void) const { 
	return *fermact;
      }

      //! Get parameters for the inverter
      const InvertParam_t getInvParams(void) const { 
	return inv_param;
      }

      //! Accessor for pseudofermion (read only)
      const multi1d< multi1d<T> >& getPhi(void) const {return phi;}

      //! mutator for pseudofermion
      multi1d< multi1d<T> >& getPhi(void) {return phi;}

      //! Accessor for PV pseudofermion (read only)
      const multi1d< multi1d<T> >& getPhiPV(void) const {return phiPV;}

      //! mutator for PV pseudofermion 
      multi1d< multi1d<T> >& getPhiPV(void) {return phiPV;}

      //! Return number of roots in used
      int getNthRoot() const {return nthRoot;}
      
      //! Return number of roots used in PV
      int getNthRootPV() const { return nthRootPV; }

      //! Return the partial fraction expansion for the force calc
      const RemezCoeff_t& getFPFE() const {return fpfe;}

      //! Return the partial fraction expansion for the action calc
      const RemezCoeff_t& getSPFE() const {return spfe;}

      //! Return the partial fraction expansion for the heat-bath
      const RemezCoeff_t& getSIPFE() const {return sipfe;}

      //! Return the partial fraction expansion for the force calc in PV
      const RemezCoeff_t& getFPVPFE() const {return fpvpfe;}

      //! Return the partial fraction expansion for the action calc in PV
      const RemezCoeff_t& getSPVPFE() const {return spvpfe;}

      //! Return the partial fraction expansion for the heat-bath in PV
      const RemezCoeff_t& getSIPVPFE() const {return sipvpfe;}

    private:
      // Hide empty constructor and =
      UnprecOneFlavorWilsonTypeFermRatMonomial5D();
      void operator=(const UnprecOneFlavorWilsonTypeFermRatMonomial5D&);

      // Pseudofermion field phi
      multi1d< multi1d<T> > phi;

      // Pseudofermion field phi
      multi1d< multi1d<T> > phiPV;

      // A handle for the UnprecWilsonFermAct
      Handle<const UnprecWilsonTypeFermAct5D<T,P,Q> > fermact;

      // The parameters for the inversion
      InvertParam_t inv_param;

      // Number of nth-roots
      int nthRoot;

      // Number of nth-roots in PV
      int nthRootPV;

      // Coefficients and roots of partial fractions
      RemezCoeff_t  fpfe;
      RemezCoeff_t  spfe;
      RemezCoeff_t  sipfe;

      RemezCoeff_t  fpvpfe;
      RemezCoeff_t  spvpfe;
      RemezCoeff_t  sipvpfe;
  };


} //end namespace chroma



#endif
