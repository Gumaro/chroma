// $Id: prec_clover_fermact_w.cc,v 3.4 2006-08-21 07:32:36 edwards Exp $
/*! \file
 *  \brief Even-odd preconditioned Clover fermion action
 */

#include "chromabase.h"
#include "actions/ferm/linop/prec_clover_linop_w.h"
#include "actions/ferm/fermacts/prec_clover_fermact_w.h"

#include "actions/ferm/fermacts/fermact_factory_w.h"
#include "actions/ferm/fermacts/ferm_createstate_reader_w.h"

#include "actions/ferm/fermacts/ferm_createstate_aggregate_w.h"

#include "actions/ferm/fermbcs/fermbcs_reader_w.h"

namespace Chroma
{

  //! Hooks to register the class with the fermact factory
  namespace EvenOddPrecCloverFermActEnv
  {
    //! Callback function
    WilsonTypeFermAct<LatticeFermion,
		      multi1d<LatticeColorMatrix>,
		      multi1d<LatticeColorMatrix> >* createFermAct4D(XMLReader& xml_in,
								     const std::string& path)
    {
      return new EvenOddPrecCloverFermAct(CreateFermStateEnv::reader(xml_in, path), 
					  CloverFermActParams(xml_in, path));
    }

    //! Callback function
    /*! Differs in return type */
    FermionAction<LatticeFermion,
		  multi1d<LatticeColorMatrix>,
		  multi1d<LatticeColorMatrix> >* createFermAct(XMLReader& xml_in,
							       const std::string& path)
    {
      return createFermAct4D(xml_in, path);
    }

    //! Name to be used
    const std::string name = "CLOVER";

    //! Register all the factories
    bool registerAll()
    {
      bool foo = true;
      foo &= CreateFermStateEnv::registered;

      foo &= Chroma::TheFermionActionFactory::Instance().registerObject(name, createFermAct);
      foo &= Chroma::TheWilsonTypeFermActFactory::Instance().registerObject(name, createFermAct4D);
    }

    //! Register the fermact
    const bool registered = registerAll();
  }


  //! Produce a linear operator for this action
  /*!
   * The operator acts on the odd subset
   *
   * \param state	    gauge field     	       (Read)
   */
  EvenOddPrecLogDetLinearOperator<LatticeFermion,
				  multi1d<LatticeColorMatrix>,
				  multi1d<LatticeColorMatrix> >* 
  EvenOddPrecCloverFermAct::linOp(Handle< FermState<T,P,Q> > state) const
  {
    return new EvenOddPrecCloverLinOp(state,param);
  }

}

