// $Id: prec_nef_fermact_array_w.cc,v 2.2 2006-01-12 05:45:16 edwards Exp $
/*! \file
 *  \brief 4D style even-odd preconditioned NEF fermion action
 */

#include "chromabase.h"
#include "actions/ferm/fermacts/prec_nef_fermact_array_w.h"
#include "actions/ferm/linop/unprec_nef_linop_array_w.h"
#include "actions/ferm/linop/prec_nef_linop_array_w.h"

#include "actions/ferm/fermacts/fermact_factory_w.h"
#include "actions/ferm/fermbcs/fermbcs_w.h"

#include "actions/ferm/qprop/quarkprop4_w.h"
#include "actions/ferm/qprop/nef_quarkprop4_w.h"

namespace Chroma
{
  //! Hooks to register the class with the fermact factory
  namespace EvenOddPrecNEFFermActArrayEnv
  {
    //! Callback function
    WilsonTypeFermAct5D< LatticeFermion, multi1d<LatticeColorMatrix> >* createFermAct5D(XMLReader& xml_in,
											const std::string& path)
    {
      return new EvenOddPrecNEFFermActArray(WilsonTypeFermBCArrayEnv::reader(xml_in, path), 
					    EvenOddPrecNEFFermActArrayParams(xml_in, path));
    }

    //! Callback function
    /*! Differs in return type */
    FermionAction<LatticeFermion>* createFermAct(XMLReader& xml_in,
						 const std::string& path)
    {
      return createFermAct5D(xml_in, path);
    }

    //! Name to be used
    const std::string name = "NEF";

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
  EvenOddPrecNEFFermActArrayParams::EvenOddPrecNEFFermActArrayParams(XMLReader& xml, 
							     const std::string& path)
  {
    XMLReader paramtop(xml, path);

    // Read the stuff for the action
    read(paramtop, "OverMass", OverMass);
    read(paramtop, "Mass", Mass);
    read(paramtop, "N5", N5);
    read(paramtop, "b5", b5);
    read(paramtop, "c5", c5);
  }


  //! Read parameters
  void read(XMLReader& xml, const string& path, EvenOddPrecNEFFermActArrayParams& param)
  {
    EvenOddPrecNEFFermActArrayParams tmp(xml, path);
    param = tmp;
  }



  //! Produce an even-odd preconditioned linear operator for this action with arbitrary quark mass
  const EvenOddPrecDWLikeLinOpBaseArray< LatticeFermion, multi1d<LatticeColorMatrix> >*
  EvenOddPrecNEFFermActArray::precLinOp(Handle<const ConnectState> state,
					const Real& m_q) const
  {
    return new EvenOddPrecNEFDWLinOpArray(state->getLinks(),params.OverMass,
					  params.b5,params.c5,m_q,params.N5);
  }

  //! Produce an unpreconditioned linear operator for this action with arbitrary quark mass
  const UnprecDWLikeLinOpBaseArray< LatticeFermion, multi1d<LatticeColorMatrix> >*
  EvenOddPrecNEFFermActArray::unprecLinOp(Handle<const ConnectState> state,
					  const Real& m_q) const
  {
    multi1d<Real> bb5(params.N5);
    multi1d<Real> cc5(params.N5);

    bb5 = params.b5;
    cc5 = params.c5;

    return new UnprecNEFDWLinOpArray(state->getLinks(),params.OverMass,bb5,cc5,m_q,params.N5);
  }

  
  // Given a complete propagator as a source, this does all the inversions needed
  void 
  EvenOddPrecNEFFermActArray::quarkProp(LatticePropagator& q_sol, 
					XMLWriter& xml_out,
					const LatticePropagator& q_src,
					int t_src, int j_decay,
					Handle<const ConnectState> state,
					const InvertParam_t& invParam,
					QuarkSpinType quarkSpinType,
					bool obsvP,
					int& ncg_had)
  {
    if (obsvP)
      nef_quarkProp4(q_sol, xml_out, q_src, t_src, j_decay, *this, state, invParam, ncg_had);
    else
    {
      Handle< const SystemSolver<LatticeFermion> > qprop(this->qprop(state,invParam));
      quarkProp4(q_sol, xml_out, q_src, qprop, quarkSpinType, ncg_had);
    }
  }

}

