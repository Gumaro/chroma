// -*- C++ -*-
// $Id: prec_stout_fermact_array_w.h,v 2.3 2006-01-12 05:45:16 edwards Exp $

/*! @file
 *  @brief Proxy fermion action class instance for unpreconditioned stout fermacts 
 */

#ifndef _prec_stout_fermact_array_w_h_
#define _prec_stout_fermact_array_w_h_

#include "chromabase.h"
#include "fermact.h"
#include "actions/ferm/fermacts/stout_state.h"
#include "actions/ferm/fermacts/stout_fermact_params.h"
#include "actions/ferm/fermacts/fermact_factory_w.h"

namespace Chroma 
{


  //! Name and registration
  namespace EvenOddPrecStoutWilsonTypeFermAct5DEnv {
    extern const std::string name;
    extern const bool registered;
  }


  //! A proxy class for unpreconditioned stout actions
  /*! @ingroup actions 
   *
   * Basically this shell of an action substitutes a call
   * to create state to create a stout state
   *
   * apart from this it does only gymnastics to forward 
   * to an embedded fermion action 
   */
  
  class EvenOddPrecStoutWilsonTypeFermAct5D 
    : public EvenOddPrecWilsonTypeFermAct5D<LatticeFermion , 
                                     multi1d<LatticeColorMatrix> > 
    {

    public:
      // Destructor is automagic
      ~EvenOddPrecStoutWilsonTypeFermAct5D() {}

      //! General FermBC
      EvenOddPrecStoutWilsonTypeFermAct5D(Handle< FermBC<multi1d<LatticeFermion> > > fbc_,
			 const StoutFermActParams& p_):  p(p_) { 

	struct EvenOddPrecCastFailure {
	  EvenOddPrecCastFailure(std::string e) : auxfermact(e) {};
	  const string auxfermact;
	};

	try {
	  // Make an XML Reader from the internal fermact
	  std::istringstream is(p_.internal_fermact);
	  XMLReader fermact_xml(is);
	  
	  // Get to the top of it
	  XMLReader internal_reader(fermact_xml, "/InternalFermionAction");
	  
	  // Get the name of the internal fermion action
	  std::string if_name;
	  read(internal_reader, "FermAct", if_name);
	  
	  // Creaate it using the FermionActionFactory
	  FermionAction<LatticeFermion>* FA_ptr= TheFermionActionFactory::Instance().createObject(if_name, fermact_xml, "/InternalFermionAction"); 

	  // Upcast to EvenOddPrecWilsonTypeFermact
	  EvenOddPrecWilsonTypeFermAct5D< LatticeFermion, multi1d<LatticeColorMatrix> >* S_internal = dynamic_cast< EvenOddPrecWilsonTypeFermAct5D< LatticeFermion, multi1d<LatticeColorMatrix> >* >(FA_ptr);

	  
	  if( S_internal == 0x0 ) { 
	    throw EvenOddPrecCastFailure(p_.internal_fermact);
	  }
	  else {
	    S_w = S_internal;
	  }
	}
	catch(const std::string& e) {
	  QDPIO::cout << "Error creating Internal Fermact: " << e << endl;
	  QDP_abort(1);
	}
	catch(const EvenOddPrecCastFailure& e) {
	  QDPIO::cout << "Internal Fermact cannot be cast to EvenOddPrecWilsonTypeFermAct5D: " << e.auxfermact << endl;
	  QDP_abort(1);
	}

      }

      //! Copy Constructor 
      EvenOddPrecStoutWilsonTypeFermAct5D(const EvenOddPrecStoutWilsonTypeFermAct5D& a) :
							p(a.p),
							S_w(a.S_w) { }

      //! Assignment
      EvenOddPrecStoutWilsonTypeFermAct5D& operator=(const EvenOddPrecStoutWilsonTypeFermAct5D& a) 
      {
	p = a.p;
	S_w = a.S_w;
	return *this;
      }

      const FermBC<multi1d<LatticeFermion> >& getFermBC() const { 
	return S_w->getFermBC();
      }

      //! Produce a linear operator for this action
      const EvenOddPrecLinearOperator< multi1d<LatticeFermion>, multi1d<LatticeColorMatrix> >* linOp(Handle<const ConnectState> state) const 
      {	
	return S_w->linOp(state);
      }

      //! Produce a linear operator M^dag.M for this action
      const DiffLinearOperator< multi1d<LatticeFermion>, multi1d<LatticeColorMatrix> >* lMdagM(Handle<const ConnectState> state) const {
	return S_w->lMdagM(state);
      }

      //! Produce the gamma_5 hermitian operator H_w
      const LinearOperator<multi1d<LatticeFermion> >* hermitianLinOp(Handle< const ConnectState> state) const { 
	return S_w->hermitianLinOp(state);
      }


      const ConnectState* createState(const multi1d<LatticeColorMatrix>& u_thin) const
      {
	multi1d<LatticeColorMatrix> u_tmp(Nd);
	u_tmp = u_thin;

	// Apply BC-s to thin links
	S_w->getFermBC().modifyU(u_tmp);

	// Make a stout state
	return new StoutConnectState(u_tmp, p.rho, p.n_smear, p.orthog_dir);
      }

      const SystemSolver<LatticeFermion>* qprop(Handle< const ConnectState > state, const InvertParam_t& invParam) const
      {
	return S_w->qprop(state, invParam);
      }

      Real getQuarkMass(void) const { 
	return S_w->getQuarkMass();
      }

      const EvenOddPrecLinearOperator<multi1d<LatticeFermion>, 
				 multi1d<LatticeColorMatrix> >* linOpPV(Handle< const ConnectState> state) const 
      {
	
	return S_w->linOpPV(state);
      }
      
 
      const LinearOperator<LatticeFermion>* DeltaLs(Handle<const ConnectState> state, const InvertParam_t& invParam) const { 
	return S_w->DeltaLs(state,invParam);
      }

      const LinearOperator<LatticeFermion>* linOp4D(Handle<const ConnectState> state, const Real& m_q, const InvertParam_t& invParam) const { 
	return S_w->linOp4D(state,m_q,invParam);
      }

      int size(void) const {
	return S_w->size();
      }

    private:
      StoutFermActParams p;

      Handle< EvenOddPrecWilsonTypeFermAct5D<LatticeFermion, 
	                              multi1d<LatticeColorMatrix> > > S_w;

  };
};


#endif
