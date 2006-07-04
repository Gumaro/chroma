// $Id: prec_constdet_two_flavor_monomial_w.cc,v 3.1 2006-07-04 02:55:52 edwards Exp $
/*! @file
 * @brief Two-flavor collection of even-odd preconditioned 4D ferm monomials
 */

#include "chromabase.h"
#include "update/molecdyn/monomial/prec_constdet_two_flavor_monomial_w.h"
#include "update/molecdyn/monomial/monomial_factory.h"

#include "actions/ferm/fermacts/fermact_factory_w.h"
#include "actions/ferm/fermacts/fermacts_aggregate_w.h"

#include "update/molecdyn/predictor/chrono_predictor.h"
#include "update/molecdyn/predictor/chrono_predictor_factory.h"

#include "update/molecdyn/predictor/zero_guess_predictor.h"


namespace Chroma 
{ 
 
  namespace EvenOddPrecConstDetTwoFlavorWilsonTypeFermMonomialEnv 
  {
    //! Callback function for the factory
    Monomial< multi1d<LatticeColorMatrix>,
	      multi1d<LatticeColorMatrix> >* createMonomial(XMLReader& xml, const string& path) 
    {
      QDPIO::cout << "Create Monomial: " << name << endl;

      return new EvenOddPrecConstDetTwoFlavorWilsonTypeFermMonomial(
	TwoFlavorWilsonTypeFermMonomialParams(xml, path));
    }
    
    const std::string name("TWO_FLAVOR_EOPREC_CONSTDET_FERM_MONOMIAL");

    //! Register all the objects
    bool registerAll()
    {
      bool foo = true;

      foo &= WilsonTypeFermActs4DEnv::registered;
      foo &= TheMonomialFactory::Instance().registerObject(name, createMonomial);

      return foo;
    }

    //! Register the fermact
    const bool registered = registerAll();
  }; //end namespace EvenOddPrec TwoFlavorWilsonFermMonomialEnv



  // Constructor
  EvenOddPrecConstDetTwoFlavorWilsonTypeFermMonomial::EvenOddPrecConstDetTwoFlavorWilsonTypeFermMonomial(
    const TwoFlavorWilsonTypeFermMonomialParams& param) 
  {
    inv_param = param.inv_param;

    std::istringstream is(param.fermact.xml);
    XMLReader fermact_reader(is);
    QDPIO::cout << "EvanOddPrecConstDetTwoFlavorWilsonTypeFermMonomial: construct " << param.fermact.id << endl;

    WilsonTypeFermAct<T,P,Q>* tmp_act = 
      TheWilsonTypeFermActFactory::Instance().createObject(param.fermact.id, fermact_reader, param.fermact.path);

    EvenOddPrecWilsonTypeFermAct<T,P,Q>* downcast =
      dynamic_cast<EvenOddPrecWilsonTypeFermAct<T,P,Q>*>(tmp_act);

    // Check success of the downcast 
    if( downcast == 0x0 ) {
      QDPIO::cerr << "Unable to downcast FermAct to EvenOddPrecWilsonTypeFermAct in EvenOddPrecConstDetTwoFlavorWilsonTypeFermMonomial()" << endl;
      QDP_abort(1);
    }

    fermact = downcast;    

    // Get Chronological predictor
    AbsChronologicalPredictor4D<LatticeFermion>* tmp = 0x0;
    if( param.predictor.xml == "" ) {
      // No predictor specified use zero guess
       tmp = new ZeroGuess4DChronoPredictor();
    }
    else 
    {
      try 
      { 
	std::istringstream chrono_is(param.predictor.xml);
	XMLReader chrono_xml(chrono_is);
	tmp = The4DChronologicalPredictorFactory::Instance().createObject(param.predictor.id, 
									  chrono_xml, 
									  param.predictor.path);
      }
      catch(const std::string& e ) { 
	QDPIO::cerr << "Caught Exception Reading XML: " << e << endl;
	QDP_abort(1);
      }
    }
     
    if( tmp == 0x0 ) { 
      QDPIO::cerr << "Failed to create ZeroGuess4DChronoPredictor" << endl;
      QDP_abort(1);
    }
    chrono_predictor = tmp;

    
  }
  
}; //end namespace Chroma


