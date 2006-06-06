// $Id: poly_cheb_fermact_w.cc,v 3.1 2006-06-06 19:06:34 kostas Exp $
/*! \file
 *  \brief Chebyshev polynomial fermion action
 */

#include "chromabase.h"
#include "actions/ferm/fermacts/poly_cheb_fermact_w.h"
#include "actions/ferm/linop/polprec_op.h"
#include "actions/ferm/linop/polynomial_op.h"

#include "actions/ferm/fermacts/fermact_factory_w.h"
#include "actions/ferm/fermacts/ferm_createstate_reader_w.h"

namespace Chroma
{
  //! Hooks to register the class with the fermact factory
  namespace PolyChebFermActEnv
  {
    //! Callback function
    WilsonTypeFermAct<LatticeFermion,
		      multi1d<LatticeColorMatrix>,
		      multi1d<LatticeColorMatrix> >* createFermAct4D(XMLReader& xml_in,
								     const std::string& path)
    {
      return new PolyChebFermAct(CreateFermStateEnv::reader(xml_in, path), 
				 PolyChebFermActParams(xml_in, path));
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
    const std::string name = "POLYNOMIAL_CHEBYSHEV";

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


  //! Read parameters
  PolyChebFermActParams::PolyChebFermActParams(XMLReader& xml, const string& path)
  {
    XMLReader paramtop(xml, path);

    // Read the chebyshev params
    {
      XMLReader chebtop(paramtop, "PolyParams");
      read(chebtop, "degree", polyParams.degree);
      read(chebtop, "UpperBound", polyParams.UpperBound);
      read(chebtop, "LowerBound", polyParams.LowerBound);
      read(chebtop, "order", polyParams.order);
    }

    // Read the underlying fermion action
    try 
    { 
      if(paramtop.count("AuxFermAct") == 1 )
      { 
	XMLReader xml_tmp(paramtop, "AuxFermAct");
	std::ostringstream os;
	xml_tmp.print(os);
	AuxFermAct = os.str();
      }
      else
      {
	throw std::string("No auxilliary action");
      }
    }
    catch(const string& e) {
      QDPIO::cerr << PolyChebFermActEnv::name << ": Caught Exception reading params: " << e << endl;
      QDP_abort(1);
    }

  }

  //! Read parameters
  void read(XMLReader& xml, const string& path, PolyChebFermActParams& param)
  {
    PolyChebFermActParams tmp(xml, path);
    param = tmp;
  }


  //! General FermBC
  PolyChebFermAct::PolyChebFermAct(Handle< CreateFermState<T,P,Q> > cfs_, 
				   const PolyChebFermActParams& param_) : 
    cfs(cfs_), param(param_)
  {
    QDPIO::cout << "Constructing PolyChebFermAct from params" << endl;
    std::istringstream  xml_s(param.AuxFermAct);
    XMLReader  fermacttop(xml_s);
    const string fermact_path = "/AuxFermAct";

    try
    {
      string auxfermact;
      read(fermacttop, fermact_path + "/FermAct", auxfermact);
      QDPIO::cout << "AuxFermAct: " << auxfermact << endl;

      // Generic Wilson-Type stuff
      Handle< WilsonTypeFermAct<T,P,Q> >
	S_f(TheWilsonTypeFermActFactory::Instance().createObject(auxfermact,
								 fermacttop,
								 fermact_path));

      fermact = S_f;
    }
    catch (const std::string& e) 
    {
      // General breakage Scenario
      QDPIO::cerr << "Error reading data: " << e << endl;
      throw;
    }

  }

  //! Produce a linear operator for this action
  PolyLinearOperator<LatticeFermion,
		     multi1d<LatticeColorMatrix>,
		     multi1d<LatticeColorMatrix> >* 
  PolyChebFermAct::polyLinOp(Handle< FermState<T,P,Q> > state) const
  {
    Handle< DiffLinearOperator<T,P,Q> > 
      MdagM(fermact->lMdagM(state));

    return new lpoly<T,P,Q>(MdagM,
			    param.polyParams.degree, 
			    param.polyParams.LowerBound, 
			    param.polyParams.UpperBound); 
    //			    param.polyParams.order); // comment this out to use
    // bit reversal ordering
  }

  //! Produce a linear operator for this action
  DiffLinearOperator<LatticeFermion,
		     multi1d<LatticeColorMatrix>,
		     multi1d<LatticeColorMatrix> >* 
  PolyChebFermAct::polyPrecLinOp(Handle< FermState<T,P,Q> > state) const
  {
    Handle< DiffLinearOperator<T,P,Q> > M(fermact->linOp(state));
    Handle< DiffLinearOperator<T,P,Q> > Pol(polyLinOp(state));

    return new PolyPrec<T,P,Q>(M, Pol);
  }

}
