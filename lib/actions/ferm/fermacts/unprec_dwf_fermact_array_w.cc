// $Id: unprec_dwf_fermact_array_w.cc,v 1.13 2004-12-29 22:13:40 edwards Exp $
/*! \file
 *  \brief Unpreconditioned domain-wall fermion action
 */

#include "chromabase.h"
#include "actions/ferm/fermacts/unprec_dwf_fermact_array_w.h"
#include "actions/ferm/linop/unprec_dwf_linop_array_w.h"

#include "actions/ferm/fermacts/fermact_factory_w.h"
#include "actions/ferm/fermbcs/fermbcs_w.h"

#include "actions/ferm/qprop/quarkprop4_w.h"
#include "actions/ferm/qprop/dwf_quarkprop4_w.h"

namespace Chroma
{

  //! Hooks to register the class with the fermact factory
  namespace UnprecDWFermActArrayEnv
  {
    //! Callback function
    WilsonTypeFermAct5D<LatticeFermion>* createFermAct5D(XMLReader& xml_in,
							 const std::string& path)
    {
      return new UnprecDWFermActArray(WilsonTypeFermBCArrayEnv::reader(xml_in, path), 
				      UnprecDWFermActArrayParams(xml_in, path));
    }

    //! Callback function
    /*! Differs in return type */
    FermionAction<LatticeFermion>* createFermAct(XMLReader& xml_in,
						 const std::string& path)
    {
      return createFermAct5D(xml_in, path);
    }

    //! Name to be used
    const std::string name = "UNPRECONDITIONED_DWF";

    //! Register all the factories
    bool registerAll()
    {
      return Chroma::TheFermionActionFactory::Instance().registerObject(name, createFermAct)
	   & Chroma::TheWilsonTypeFermAct5DFactory::Instance().registerObject(name, createFermAct5D);
    }

    //! Register the fermact
    const bool registered = registerAll();
  }


  //! Read parameters
  UnprecDWFermActArrayParams::UnprecDWFermActArrayParams(XMLReader& xml, 
								   const std::string& path)
  {
    XMLReader paramtop(xml, path);

    // Read the stuff for the action
    read(paramtop, "OverMass", OverMass);
    read(paramtop, "Mass", Mass);
    read(paramtop, "N5", N5);

    if (paramtop.count("a5") != 0) 
      read(paramtop, "a5", a5);
    else
      a5 = 1.0;
  }


  //! Read parameters
  void read(XMLReader& xml, const string& path, UnprecDWFermActArrayParams& param)
  {
    UnprecDWFermActArrayParams tmp(xml, path);
    param = tmp;
  }


  
  //! Produce an unpreconditioned linear operator for this action with arbitrary quark mass
  const UnprecDWLinOpBaseArray<LatticeFermion>* 
  UnprecDWFermActArray::unprecLinOp(Handle<const ConnectState> state, 
				    const Real& m_q) const
  {
    return new UnprecDWLinOpArray(state->getLinks(),OverMass,m_q,N5);
  }

  
  // Given a complete propagator as a source, this does all the inversions needed
  void 
  UnprecDWFermActArray::quarkProp(LatticePropagator& q_sol, 
				  XMLWriter& xml_out,
				  const LatticePropagator& q_src,
				  int t_src, int j_decay,
				  Handle<const ConnectState> state,
				  const InvertParam_t& invParam,
				  bool nonRelProp,
				  bool obsvP,
				  int& ncg_had)
  {
    if (obsvP)
      dwf_quarkProp4(q_sol, xml_out, q_src, t_src, j_decay, *this, state, invParam, ncg_had);
    else
      quarkProp4(q_sol, xml_out, q_src, *this, state, invParam, nonRelProp, ncg_had);
  }

}
