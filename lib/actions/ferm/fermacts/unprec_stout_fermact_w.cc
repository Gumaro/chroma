#include "chromabase.h"
#include "actions/ferm/fermacts/unprec_stout_fermact_w.h"
#include "actions/ferm/fermacts/stout_fermact_params.h"
#include "actions/ferm/fermacts/fermact_factory_w.h"
#include "actions/ferm/fermbcs/fermbcs_w.h"

#include "io/param_io.h"

namespace Chroma {

  namespace UnprecStoutWilsonTypeFermActEnv {
       //! Callback function
    WilsonTypeFermAct<LatticeFermion,multi1d<LatticeColorMatrix> >* createFermAct4D(XMLReader& xml_in,
										    const std::string& path)
    {
      
      return new UnprecStoutWilsonTypeFermAct(WilsonTypeFermBCEnv::reader(xml_in, path), 
					      StoutFermActParams(xml_in, path));
    }

  

    //! Callback function
    /*! Differs in return type */
    FermionAction<LatticeFermion>* createFermAct(XMLReader& xml_in,
						 const std::string& path)
    {
      return createFermAct4D(xml_in, path);
    }
  
    //! Name to be used
    const std::string name = "UNPRECONDITIONED_STOUT";
    
    //! Register all the factories
    bool registerAll()
    {
      return Chroma::TheFermionActionFactory::Instance().registerObject(name, createFermAct)
	& Chroma::TheWilsonTypeFermActFactory::Instance().registerObject(name, createFermAct4D);
    }

    const bool registered = registerAll();

  }

};
