// $Id: unprec_w12_fermact_w.cc,v 3.1 2006-09-19 18:08:39 edwards Exp $
/*! \file
 *  \brief Unpreconditioned W12 fermion action
 */

#include "chromabase.h"
#include "actions/ferm/fermacts/unprec_w12_fermact_w.h"
#include "actions/ferm/linop/unprec_w12_linop_w.h"

#include "actions/ferm/fermacts/fermact_factory_w.h"
#include "actions/ferm/fermstates/ferm_createstate_reader_w.h"

namespace Chroma
{

  //! Hooks to register the class with the fermact factory
  namespace UnprecW12FermActEnv
  {
    //! Callback function
    WilsonTypeFermAct<LatticeFermion,
		      multi1d<LatticeColorMatrix>,
		      multi1d<LatticeColorMatrix> >* createFermAct4D(XMLReader& xml_in,
								     const std::string& path)
    {
      return new UnprecW12FermAct(CreateFermStateEnv::reader(xml_in, path), 
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
    const std::string name = "UNPRECONDITIONED_W12";

    //! Register all the factories
    bool registerAll()
    {
      bool foo = true;
      foo &= Chroma::TheFermionActionFactory::Instance().registerObject(name, createFermAct);
      foo &= Chroma::TheWilsonTypeFermActFactory::Instance().registerObject(name, createFermAct4D);
      return foo;
    }

    //! Register the fermact
    const bool registered = registerAll();
  }


  //! Produce a linear operator for this action
  /*!
   * The operator acts on the entire lattice
   *
   * \param state	    gauge field     	       (Read)
   */
  UnprecLinearOperator<LatticeFermion,
		       multi1d<LatticeColorMatrix>,
		       multi1d<LatticeColorMatrix> >*
  UnprecW12FermAct::linOp(Handle< FermState<T,P,Q> > state) const
  {
    if (param.anisoParam.anisoP)
    {
      QDPIO::cerr << "UnprecW12FermAct::linOp - currently no aniso support" << endl;
      QDP_abort(1);
    }

    return new UnprecW12LinOp(state, param); 
  }

}
