// -*- C++ -*-
// $Id: gauge_monomial.cc,v 1.1 2005-01-12 04:43:33 edwards Exp $
/*! \file
 *  \brief Generic gauge action monomial wrapper
 */

#include "chromabase.h"

#include "update/molecdyn/gauge_monomial.h"
#include "update/molecdyn/monomial_factory.h"

#include "actions/gauge/wilson_gaugeact.h"
#include "actions/gauge/plaq_gaugeact.h"
#include "actions/gauge/rect_gaugeact.h"
#include "actions/gauge/pg_gaugeact.h"

#include <string>

namespace Chroma 
{ 
  namespace GaugeMonomialEnv 
  {
    //! Callback function for the factory
    Monomial< multi1d<LatticeColorMatrix>,
	      multi1d<LatticeColorMatrix> >*
    createMonomialWilson(XMLReader& xml, const string& path) 
    {
      QDPIO::cout << "Create Monomial: " << WilsonGaugeActEnv::name << endl;

      return new GaugeMonomial(WilsonGaugeActEnv::name,
			       GaugeMonomialParams(xml, path));
    }
    
    //! Callback function for the factory
    Monomial< multi1d<LatticeColorMatrix>,
	      multi1d<LatticeColorMatrix> >*
    createMonomialPlaq(XMLReader& xml, const string& path) 
    {
      QDPIO::cout << "Create Monomial: " << PlaqGaugeActEnv::name << endl;

      return new GaugeMonomial(PlaqGaugeActEnv::name,
			       GaugeMonomialParams(xml, path));
    }
    
    //! Callback function for the factory
    Monomial< multi1d<LatticeColorMatrix>,
	      multi1d<LatticeColorMatrix> >*
    createMonomialRect(XMLReader& xml, const string& path) 
    {
      QDPIO::cout << "Create Monomial: " << RectGaugeActEnv::name << endl;

      return new GaugeMonomial(RectGaugeActEnv::name,
			       GaugeMonomialParams(xml, path));
    }
    
    //! Callback function for the factory
    Monomial< multi1d<LatticeColorMatrix>,
	      multi1d<LatticeColorMatrix> >*
    createMonomialPg(XMLReader& xml, const string& path) 
    {
      QDPIO::cout << "Create Monomial: " << PgGaugeActEnv::name << endl;

      return new GaugeMonomial(PgGaugeActEnv::name,
			       GaugeMonomialParams(xml, path));
    }
    
    //! Register all the objects
    bool registerAll()
    {
      bool foo = true;
      const std::string suffix("_MONOMIAL");

      // Use a pattern to register all the qualifying gaugeacts
      foo &= WilsonGaugeActEnv::registered;
      foo &= TheMonomialFactory::Instance().registerObject(WilsonGaugeActEnv::name+suffix, 
							   createMonomialWilson);
      foo &= PlaqGaugeActEnv::registered;
      foo &= TheMonomialFactory::Instance().registerObject(PlaqGaugeActEnv::name+suffix, 
							   createMonomialPlaq);
      foo &= RectGaugeActEnv::registered;
      foo &= TheMonomialFactory::Instance().registerObject(RectGaugeActEnv::name+suffix, 
							   createMonomialRect);
      foo &= PgGaugeActEnv::registered;
      foo &= TheMonomialFactory::Instance().registerObject(PgGaugeActEnv::name+suffix, 
							   createMonomialPg);
      return foo;
    }

    //! Register the gaugeact
    const bool registered = registerAll();
  } //end namespace GaugeMonomialEnv



  // Read the parameters
  GaugeMonomialParams::GaugeMonomialParams(XMLReader& xml_in, const string& path)
  {
    // Get the top of the parameter XML tree
    XMLReader paramtop(xml_in, path);
    
    try {
      // Read the inverter Parameters
      XMLReader xml_tmp(paramtop, "./GaugeAction");
      std::ostringstream os;
      xml_tmp.print(os);
      gauge_act = os.str();
   
    }
    catch(const string& s) {
      QDPIO::cerr << "Caught Exception while reading parameters: " << s <<endl;
      QDP_abort(1);
    }

    QDPIO::cout << "GaugeMonomialParams: read \n" << gauge_act << endl;
  }

  //! Read Parameters
  void read(XMLReader& xml, const std::string& path,
	    GaugeMonomialParams& params) {
    GaugeMonomialParams tmp(xml, path);
    params = tmp;
  }

  //! Write Parameters
  void write(XMLWriter& xml, const std::string& path,
	     const GaugeMonomialParams& params) {
    // Not implemented
  }


  // Constructor
  GaugeMonomial::GaugeMonomial(
    const string& name_,
    const GaugeMonomialParams& param_) 
  {
    std::istringstream is(param_.gauge_act);
    XMLReader gaugeact_reader(is);

    // Get the name of the gauge act
    std::string gaugeact_string;
    try { 
      read(gaugeact_reader, "/GaugeAction/Name", gaugeact_string);
      if ( gaugeact_string != name_ ) { 
	QDPIO::cerr << "Gauge action is not " << name_
		    << " but is: " << gaugeact_string << endl;
	QDP_abort(1);
      }
    }
    catch( const std::string& e) { 
      QDPIO::cerr << "Error grepping the gaugeact name: " << e<<  endl;
      QDP_abort(1);
    }

    // Throw an exception if not found
    gaugeact = TheGaugeActFactory::Instance().createObject(gaugeact_string, gaugeact_reader, "./GaugeAction");
  }

}; //end namespace Chroma


