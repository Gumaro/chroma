// $Id: unprec_dwftransf_fermact_w.cc,v 1.9 2005-02-25 03:01:55 edwards Exp $
/*! \file
 *  \brief Unpreconditioned Wilson fermion action
 */

#include "chromabase.h"
#include "actions/ferm/fermacts/unprec_dwftransf_fermact_w.h"
#include "actions/ferm/linop/unprec_dwftransf_linop_w.h"
#include "actions/ferm/linop/lmdagm.h"
#include "io/param_io.h"

#include "actions/ferm/fermacts/fermact_factory_w.h"
#include "actions/ferm/fermbcs/fermbcs_w.h"

#include <string>


namespace Chroma
{

  //! Hooks to register the class with the fermact factory
  namespace UnprecDWFTransfFermActEnv
  {
    //! Callback function
    WilsonTypeFermAct<LatticeFermion,multi1d<LatticeColorMatrix> >* createFermAct4D(XMLReader& xml_in,
										    const std::string& path)
    {
      return new UnprecDWFTransfFermAct(WilsonTypeFermBCEnv::reader(xml_in, path), 
					UnprecDWFTransfFermActParams(xml_in, path));
    }

    //! Callback function
    /*! Differs in return type */
    FermionAction<LatticeFermion>* createFermAct(XMLReader& xml_in,
						 const std::string& path)
    {
      return createFermAct4D(xml_in, path);
    }

    //! Name to be used
    const std::string name = "UNPRECONDITIONED_DWFTRANSF";

    //! Register all the factories
    bool registerAll()
    {
      return Chroma::TheFermionActionFactory::Instance().registerObject(name, createFermAct)
	   & Chroma::TheWilsonTypeFermActFactory::Instance().registerObject(name, createFermAct4D);
    }

    //! Register the fermact
    const bool registered = registerAll();
  }


  //! Read parameters
  UnprecDWFTransfFermActParams::UnprecDWFTransfFermActParams(XMLReader& xml, const string& path)
  {
    try {
      XMLReader paramtop(xml, path);
    
      // Read the stuff for the action
      if (paramtop.count("Mass") != 0) {
	read(paramtop, "Mass", Mass);
	if (paramtop.count("Kappa") != 0) {
	  QDPIO::cerr << "Error: found both a Kappa and a Mass tag" << endl;
	  QDP_abort(1);
	}
      }
      else if (paramtop.count("Kappa") != 0) {
	Real Kappa;
	read(paramtop, "Kappa", Kappa);
	Mass = kappaToMass(Kappa);    // Convert Kappa to Mass
      }
      else {
	QDPIO::cerr << "Error: neither Mass or Kappa found" << endl;
	QDP_abort(1);
      }
      
      // Read b5 c5 and the solver params
      read(paramtop, "b5", b5);
      read(paramtop, "c5", c5);
      // read(paramtop, "invParam", invParam);
      read(paramtop, "RsdCG", invParam.RsdCG);
      read(paramtop, "MaxCG", invParam.MaxCG);
    }
    catch( const string& e) { 
      QDPIO::cerr << "Caught exception reading XML " << e << endl;
      QDP_abort(1);
    }
      
  }


  //! Read parameters
  void read(XMLReader& xml, const string& path, UnprecDWFTransfFermActParams& param)
  {
    UnprecDWFTransfFermActParams tmp(xml, path);
    param = tmp;
  }



  //! Produce a linear operator for this action
  /*!
   * The operator acts on the entire lattice
   *
   * \param state	    gauge field     	       (Read)
   */
  const UnprecLinearOperator< LatticeFermion, multi1d<LatticeColorMatrix> >*
  UnprecDWFTransfFermAct::linOp(Handle<const ConnectState> state) const
  {
    return new UnprecDWFTransfLinOp(state->getLinks(),
				    param.Mass,
				    param.b5,
				    param.c5,
				    param.invParam);
  }


  //! Produce a M^dag.M linear operator for this action
  /*!
   * The operator acts on the entire lattice
   *
   * \param state    gauge field     	       (Read)
   */
  const LinearOperator<LatticeFermion>*
  UnprecDWFTransfFermAct::lMdagM(Handle<const ConnectState> state) const
  {
    return new UnprecDWFTransfMdagMLinOp(state->getLinks(),
					 param.Mass,
					 param.b5,
					 param.c5,
					 param.invParam);
  }

}
