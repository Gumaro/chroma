// $Id: prec_wilson_fermact_w.cc,v 2.1 2005-12-03 21:19:38 edwards Exp $
/*! \file
 *  \brief Even-odd preconditioned Wilson fermion action
 */

#include "chromabase.h"
#include "actions/ferm/fermacts/prec_wilson_fermact_w.h"
#include "actions/ferm/linop/prec_wilson_linop_w.h"
#include "actions/ferm/linop/lmdagm.h"

#include "actions/ferm/fermacts/fermact_factory_w.h"
#include "actions/ferm/fermbcs/fermbcs_w.h"

namespace Chroma
{
  //! Hooks to register the class with the fermact factory
  namespace EvenOddPrecWilsonFermActEnv
  {
    //! Callback function
    WilsonTypeFermAct<LatticeFermion,multi1d<LatticeColorMatrix> >* createFermAct4D(XMLReader& xml_in,
										    const std::string& path)
    {
      return new EvenOddPrecWilsonFermAct(WilsonTypeFermBCEnv::reader(xml_in, path), 
					  WilsonFermActParams(xml_in, path));
    }

    //! Callback function
    /*! Differs in return type */
    FermionAction<LatticeFermion>* createFermAct(XMLReader& xml_in,
						 const std::string& path)
    {
      return createFermAct4D(xml_in, path);
    }

    //! Name to be used
    const std::string name("WILSON");

    //! Register all the factories
    bool registerAll()
    {
      return Chroma::TheFermionActionFactory::Instance().registerObject(name, createFermAct)
	   & Chroma::TheWilsonTypeFermActFactory::Instance().registerObject(name, createFermAct4D);
    }

    //! Register the fermact
    const bool registered = registerAll();
  }


  //! Produce a linear operator for this action
  /*!
   * The operator acts on the odd subset
   *
   * \param state 	    gauge field     	       (Read)
   */
  const EvenOddPrecLinearOperator< LatticeFermion, multi1d<LatticeColorMatrix> >* 
  EvenOddPrecWilsonFermAct::linOp(Handle<const ConnectState> state) const
  {
    return new EvenOddPrecWilsonLinOp(state->getLinks(),param.Mass,param.anisoParam);
  }

  //! Produce a M^dag.M linear operator for this action
  /*!
   * The operator acts on the odd subset
   *
   * \param state 	    gauge field     	       (Read)
   */
  const LinearOperator<LatticeFermion>* 
  EvenOddPrecWilsonFermAct::lMdagM(Handle<const ConnectState> state) const
  {
    return new lmdagm<LatticeFermion>(linOp(state));
  }

}
