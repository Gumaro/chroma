// $Id: one_flavor_rat_monomial_params_w.cc,v 3.2 2006-07-04 02:55:52 edwards Exp $
/*! @file
 * @brief One-flavor monomial params
 */

#include "update/molecdyn/monomial/one_flavor_rat_monomial_params_w.h"
#include "io/param_io.h"

namespace Chroma 
{ 
 
  //! Remez input
  void read(XMLReader& xml, const string& path, 
	    OneFlavorWilsonTypeFermRatMonomialParams::Remez_t& input)
  {
    XMLReader inputtop(xml, path);

    read(inputtop, "lowerMin", input.lowerMin);
    read(inputtop, "upperMax", input.upperMax);
    read(inputtop, "forceDegree", input.forceDegree);
    read(inputtop, "actionDegree", input.actionDegree);

    if (inputtop.count("digitPrecision") != 0)
      read(inputtop, "digitPrecision", input.digitPrecision);
    else
      input.digitPrecision = 50;
  }


  // Read the parameters
  OneFlavorWilsonTypeFermRatMonomialParams::OneFlavorWilsonTypeFermRatMonomialParams(XMLReader& xml_in, const string& path)
  {
    // Get the top of the parameter XML tree
    XMLReader paramtop(xml_in, path);
    
    try {
      // Read the inverter Parameters
      inv_param = readXMLGroup(paramtop, "InvertParam", "invType");
      read(paramtop, "Remez", remez);
      read(paramtop, "expNumPower", expNumPower);
      read(paramtop, "expDenPower", expDenPower);
      read(paramtop, "nthRoot", nthRoot);
      fermact = readXMLGroup(paramtop, "FermionAction", "FermAct");
    }
    catch(const string& s) {
      QDPIO::cerr << "Caught Exception while reading parameters: " << s <<endl;
      QDP_abort(1);
    }

    QDPIO::cout << "OneFlavorWilsonTypeFermRatMonomialParams: read \n" << fermact.id << endl;
  }

  // Read the parameters
  OneFlavorWilsonTypeFermRatMonomialParams::OneFlavorWilsonTypeFermRatMonomialParams(
    XMLReader& xml_in, const string& path, int expNumPower_, int expDenPower_)
  {
    // Get the top of the parameter XML tree
    XMLReader paramtop(xml_in, path);
    
    expNumPower = expNumPower_;
    expDenPower = expDenPower_;

    try {
      // Read the inverter Parameters
      inv_param = readXMLGroup(paramtop, "InvertParam", "invType");
      read(paramtop, "Remez", remez);
      read(paramtop, "nthRoot", nthRoot);
      fermact = readXMLGroup(paramtop, "FermionAction", "FermAct");
    }
    catch(const string& s) {
      QDPIO::cerr << "Caught Exception while reading parameters: " << s <<endl;
      QDP_abort(1);
    }

    QDPIO::cout << "OneFlavorWilsonTypeFermRatMonomialParams: read \n" << fermact.id << endl;
  }

  //! Read Parameters
  void read(XMLReader& xml, const std::string& path,
	    OneFlavorWilsonTypeFermRatMonomialParams& params) 
  {
    OneFlavorWilsonTypeFermRatMonomialParams tmp(xml, path);
    params = tmp;
  }

  //! Write Parameters
  void write(XMLWriter& xml, const std::string& path,
	     const OneFlavorWilsonTypeFermRatMonomialParams& params) 
  {
    // Not implemented
  }
  
} //end namespace Chroma


