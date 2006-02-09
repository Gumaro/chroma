// $Id: prec_constdet_two_flavor_polynomial_monomial_w.cc,v 2.1 2006-02-09 22:26:41 edwards Exp $
/*! @file
 * @brief Two-flavor collection of even-odd preconditioned 4D ferm monomials
 */

#include "chromabase.h"
#include "update/molecdyn/monomial/prec_constdet_two_flavor_polynomial_monomial_w.h"
#include "update/molecdyn/monomial/monomial_factory.h"

#include "actions/ferm/fermacts/fermact_factory_w.h"

#include "actions/ferm/fermacts/prec_wilson_fermact_w.h"
#include "actions/ferm/fermacts/prec_parwilson_fermact_w.h"

#include "update/molecdyn/predictor/chrono_predictor.h"
#include "update/molecdyn/predictor/chrono_predictor_factory.h"

//#include "update/molecdyn/predictor/zero_guess_predictor.h"


namespace Chroma 
{ 
 
  namespace EvenOddPrecConstDetTwoFlavorPolynomialWilsonTypeFermMonomialEnv 
  {
    //! Callback function for the factory
    Monomial< multi1d<LatticeColorMatrix>,
	      multi1d<LatticeColorMatrix> >* createMonomialWilson(XMLReader& xml, const string& path) 
    {
      QDPIO::cout << "Create Monomial: " << EvenOddPrecWilsonFermActEnv::name << endl;

      return new EvenOddPrecConstDetTwoFlavorPolynomialWilsonTypeFermMonomial(
	EvenOddPrecWilsonFermActEnv::name,
	TwoFlavorWilsonTypeFermMonomialParams(xml, path));
    }
    
    //! Callback function for the factory
    Monomial< multi1d<LatticeColorMatrix>,
	      multi1d<LatticeColorMatrix> >* createMonomialParWilson(XMLReader& xml, const string& path) 
    {
      QDPIO::cout << "Create Monomial: " << EvenOddPrecParWilsonFermActEnv::name << endl;

      return new EvenOddPrecConstDetTwoFlavorPolynomialWilsonTypeFermMonomial(
	EvenOddPrecParWilsonFermActEnv::name,
	TwoFlavorWilsonTypeFermMonomialParams(xml, path));
    }

#if 0
    //! Callback function for the factory
    Monomial< multi1d<LatticeColorMatrix>,
	      multi1d<LatticeColorMatrix> >* createMonomialStout(XMLReader& xml, const string& path) 
    {
      QDPIO::cout << "Create Monomial: " << EvenOddPrecStoutWilsonTypeFermActEnv::name << endl;

      return new EvenOddPrecConstDetTwoFlavorPolynomialWilsonTypeFermMonomial(
	EvenOddPrecStoutWilsonTypeFermActEnv::name,
	TwoFlavorWilsonTypeFermMonomialParams(xml, path));
    }
#endif
 
    //! Register all the objects
    bool registerAll()
    {
      bool foo = true;
      const std::string prefix = "TWO_FLAVOR_";
      const std::string suffix = "_POLYNOMIAL_FERM_MONOMIAL";

      // Use a pattern to register all the qualifying fermacts
      foo &= EvenOddPrecWilsonFermActEnv::registered;
      foo &= TheMonomialFactory::Instance().registerObject(prefix+EvenOddPrecWilsonFermActEnv::name+suffix, 
							   createMonomialWilson);

      foo &= EvenOddPrecParWilsonFermActEnv::registered;
      foo &= TheMonomialFactory::Instance().registerObject(prefix+EvenOddPrecParWilsonFermActEnv::name+suffix, 
							   createMonomialParWilson);

#if 0
      foo &= EvenOddPrecStoutWilsonTypeFermActEnv::registered;
      foo &= TheMonomialFactory::Instance().registerObject(prefix+EvenOddPrecStoutWilsonTypeFermActEnv::name+suffix,
							   createMonomialStout);
#endif
      return foo;
    }

    //! Register the fermact
    const bool registered = registerAll();
  }; //end namespace EvenOddPrec TwoFlavorWilsonFermMonomialEnv


  // Constructor
  EvenOddPrecConstDetTwoFlavorPolynomialWilsonTypeFermMonomial::EvenOddPrecConstDetTwoFlavorPolynomialWilsonTypeFermMonomial(
    const string& name_,
    const TwoFlavorWilsonTypeFermMonomialParams& param_) 
  {
    inv_param = param_.inv_param;

    {
      std::istringstream is(param_.ferm_act);
      XMLReader fermact_reader(is);
      
      // Get the name of the ferm act
      std::string fermact_string;
      try { 
	read(fermact_reader, "/FermionAction/FermAct", fermact_string);
	if ( fermact_string != name_ ) { 
	  QDPIO::cerr << "Fermion action is not " << name_
		      << " but is: " << fermact_string << endl;
	  QDP_abort(1);
	}
      }
      catch( const std::string& e) { 
	QDPIO::cerr << "Error grepping the fermact name: " << e<<  endl;
	QDP_abort(1);
      }
      
      QDPIO::cout << "EvanOddPrecTwoFlavorPolynomialWilsonTypeFermMonomial: construct " << fermact_string << endl;
      
      
      const FermionAction<LatticeFermion>* tmp_act = TheFermionActionFactory::Instance().createObject(fermact_string, fermact_reader, "/FermionAction");
      
      
      const EvenOddPrecWilsonTypeFermAct< LatticeFermion, multi1d<LatticeColorMatrix> >* downcast=dynamic_cast<const EvenOddPrecWilsonTypeFermAct< LatticeFermion, multi1d<LatticeColorMatrix> >*>(tmp_act);
      
      // Check success of the downcast 
      if( downcast == 0x0 ) {
	QDPIO::cerr << "Unable to downcast FermAct to EvenOddPrecWilsonTypeFermAct in EvenOddPrecConstDetTwoFlavorPolynomialWilsonTypeFermMonomial()" << endl;
	QDP_abort(1);
      }
      
      fermact = downcast;    
    }
    
  }


  
}; //end namespace Chroma


