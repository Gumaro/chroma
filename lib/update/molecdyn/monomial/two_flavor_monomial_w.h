// -*- C++ -*-
// $Id: two_flavor_monomial_w.h,v 3.4 2006-08-25 17:15:59 edwards Exp $

/*! @file
 * @brief Two flavor Monomials - gauge action or fermion binlinear contributions for HMC
 */

#ifndef __two_flavor_monomial_w_h__
#define __two_flavor_monomial_w_h__

#include "update/molecdyn/monomial/abs_monomial.h"
#include "update/molecdyn/predictor/chrono_predictor.h"

#include <typeinfo>
using namespace std;

namespace Chroma
{
  //-------------------------------------------------------------------------------------------
  //! Exact 2 degen flavor fermact monomial
  /*! @ingroup monomial
   *
   * Exact 2 degen flavor fermact monomial. Preconditioning is not
   * specified yet.
   * Can supply a default dsdq and pseudoferm refresh algorithm
   * 
   * CAVEAT: I assume there is only 1 pseudofermion field in the following
   * so called TwoFlavorExact monomial.
   */
  template<typename P, typename Q, typename Phi>
  class TwoFlavorExactWilsonTypeFermMonomial : public ExactWilsonTypeFermMonomial<P,Q,Phi>
  {
  public:
     //! virtual destructor:
    ~TwoFlavorExactWilsonTypeFermMonomial() {}

    //! Compute the total action
    virtual Double S(const AbsFieldState<P,Q>& s) = 0;

    //! Compute dsdq for the system... 
    /*! Monomial of the form  chi^dag*(M^dag*M)*chi */
    virtual void dsdq(P& F, const AbsFieldState<P,Q>& s)
    {
      START_CODE();

      // Self Description/Encapsulation Rule
      XMLWriter& xml_out = TheXMLOutputWriter::Instance();
      push(xml_out, "TwoFlavorExactWilsonTypeFermMonomial");

      /**** Identical code for unprec and even-odd prec case *****/
      
      // S_f  chi^dag*(M^dag*M)^(-1)*chi     
      // Here, M is some operator.
      //
      // Need
      // dS_f/dU = - psi^dag * [d(M^dag)*M + M^dag*dM] * psi,  psi = (M^dag*M)^(-1)*chi
      //
      // In Balint's notation, the result is  
      // \dot{S} = -X^dag*\dot{M}^\dag*Y - Y^dag\dot{M}*X
      // where
      //    X = (M^dag*M)^(-1)*chi   Y = M*X = (M^dag)^(-1)*chi
      // In Robert's notation,  X -> psi .
      //
      const WilsonTypeFermAct<Phi,P,Q>& FA = getFermAct();
      
      // Create a state for linop
      Handle< FermState<Phi,P,Q> > state(FA.createState(s.getQ()));
	
      // Need way to get gauge state from AbsFieldState<P,Q>
      Handle< DiffLinearOperator<Phi,P,Q> > lin(FA.linOp(state));
	
      Phi X, Y;

      // Get X out here
      // (getMDSolutionPredictor())(X);
      int n_count = getX(X,s);
      // (getMDSolutionPredictor()).newVector(X);
      
      (*lin)(Y, X, PLUS);

      lin->deriv(F, X, Y, MINUS);

      // fold M^dag into X^dag ->  Y  !!
      P F_tmp;
      lin->deriv(F_tmp, Y, X, PLUS);
      F += F_tmp;
 
      for(int mu=0; mu < F.size(); ++mu)
	F[mu] *= Real(-1);

      // F now holds derivative with respect to possibly fat links
      // now derive it with respect to the thin links if needs be
      state->deriv(F);
      
      Double F_sq = norm2(F);

      write(xml_out, "n_count", n_count);
      write(xml_out, "F_sq", F_sq);
      pop(xml_out);

      END_CODE();
    }
 
    //! Refresh pseudofermions
    virtual void refreshInternalFields(const AbsFieldState<P,Q>& field_state) 
    {
      START_CODE();

      // Heatbath all the fields
      
      // Get at the ferion action for piece i
      const WilsonTypeFermAct<Phi,P,Q>& S_f = getFermAct();
      
      // Create a Connect State, apply fermionic boundaries
      Handle< FermState<Phi,P,Q> > f_state(S_f.createState(field_state.getQ()));
      
      // Create a linear operator
      Handle< DiffLinearOperator<Phi,P,Q> > M(S_f.linOp(f_state));
      
      Phi eta=zero;
      
      // Fill the eta field with gaussian noise
      gaussian(eta, M->subset());
      
      // Account for fermion BC by modifying the proposed field
      S_f.getFermBC().modifyF(eta);

      // Temporary: Move to correct normalisation
      eta *= sqrt(0.5);
      
      // Now HIT IT with the ROCK!!!! (Or in this case M^{dagger})
      (*M)(getPhi(), eta, MINUS);
      getMDSolutionPredictor().reset();

      END_CODE();
    }				    
  
    //! Copy pseudofermions if any
    virtual void setInternalFields(const Monomial<P,Q>& m) 
    {
      START_CODE();

      try {
	const TwoFlavorExactWilsonTypeFermMonomial<P,Q,Phi>& fm = dynamic_cast<  const TwoFlavorExactWilsonTypeFermMonomial<P,Q,Phi>& >(m);

	getPhi() = fm.getPhi();
      }
      catch(bad_cast) { 
	QDPIO::cerr << "Failed to cast input Monomial to TwoFlavorExactWilsonTypeFermMonomial " << endl;
	QDP_abort(1);
      }

      // Resetting pseudofermion fields implies resetting the chrono predictor
      getMDSolutionPredictor().reset();

      END_CODE();
    }

  protected:
    //! Accessor for pseudofermion with Pf index i (read only)
    virtual const Phi& getPhi(void) const = 0;

    //! mutator for pseudofermion with Pf index i 
    virtual Phi& getPhi(void) = 0;    

    //! Get at fermion action
    virtual const WilsonTypeFermAct<Phi,P,Q>& getFermAct(void) const = 0;

    //! Get inverter params
    virtual const GroupXML_t& getInvParams(void) const = 0;

    //! Get the initial guess predictor
    virtual AbsChronologicalPredictor4D<Phi>& getMDSolutionPredictor(void) = 0;

    //! Get (M^dagM)^{-1} phi
    virtual int getX(Phi& X, const AbsFieldState<P,Q>& s)
    {
      START_CODE();

      // Grab the fermact
      const WilsonTypeFermAct<Phi,P,Q>& FA = getFermAct();

      // Make the state
      Handle< FermState<Phi,P,Q> > state(FA.createState(s.getQ()));

      // Get system solver
      Handle< MdagMSystemSolver<Phi> > invMdagM(FA.invMdagM(state, getInvParams()));

      // CG Chrono predictor needs MdagM
      Handle< DiffLinearOperator<Phi,P,Q> > MdagM(FA.lMdagM(state));
      (getMDSolutionPredictor())(X, *MdagM, getPhi());

      // Solve MdagM X = eta
      SystemSolverResults_t res = (*invMdagM)(X, getPhi());
      QDPIO::cout << "2Flav::invert,  n_count = " << res.n_count << endl;

      // Insert vector
      (getMDSolutionPredictor()).newVector(X);

      END_CODE();

      return res.n_count;
    }

  };


  //-------------------------------------------------------------------------------------------
  //! Exact 2 degen flavor unpreconditioned fermact monomial
  /*! @ingroup monomial
   *
   * Exact 2 degen flavor unpreconditioned fermact monomial.
   * 
   * CAVEAT: I assume there is only 1 pseudofermion field in the following
   * so called TwoFlavorExact monomial.
   */
  template<typename P, typename Q, typename Phi>
  class TwoFlavorExactUnprecWilsonTypeFermMonomial : public TwoFlavorExactWilsonTypeFermMonomial<P,Q,Phi>
  {
  public:
     //! virtual destructor:
    ~TwoFlavorExactUnprecWilsonTypeFermMonomial() {}

    //! Compute the total action
    virtual Double S(const AbsFieldState<P,Q>& s)
    {
      START_CODE();

      // Self identification/encapsulation Rule
      XMLWriter& xml_out = TheXMLOutputWriter::Instance();
      push(xml_out, "TwoFlavorExactUnprecWilsonTypeFermMonomial");

      Phi X;
      
      // Energy calc doesnt use Chrono Predictor
      X = zero;

      (getMDSolutionPredictor()).reset();
      int n_count = getX(X,s);

      // Action on the entire lattice
      Double action = innerProductReal(getPhi(), X);
      
      write(xml_out, "n_count", n_count);
      write(xml_out, "S", action);
      pop(xml_out);

      END_CODE();

      return action;
    }


  protected:
    //! Accessor for pseudofermion with Pf index i (read only)
    virtual const Phi& getPhi(void) const = 0;

    //! mutator for pseudofermion with Pf index i 
    virtual Phi& getPhi(void) = 0;    

    //! Get at fermion action
    virtual const UnprecWilsonTypeFermAct<Phi,P,Q>& getFermAct(void) const = 0;

    //! Get inverter params
    virtual const GroupXML_t& getInvParams(void) const = 0;

    //! Get at the chronological predcitor
    virtual AbsChronologicalPredictor4D<Phi>& getMDSolutionPredictor(void) = 0;
  };


  //-------------------------------------------------------------------------------------------
  //! Exact 2 degen flavor even-odd preconditioned fermact monomial
  /*! @ingroup monomial
   *
   * Exact 2 degen flavor even-odd preconditioned fermact monomial.
   * Can supply a default dsdq algorithm
   */
  template<typename P, typename Q, typename Phi>
  class TwoFlavorExactEvenOddPrecWilsonTypeFermMonomial : public TwoFlavorExactWilsonTypeFermMonomial<P,Q,Phi>
  {
  public:
     //! virtual destructor:
    ~TwoFlavorExactEvenOddPrecWilsonTypeFermMonomial() {}

    //! Even even contribution (eg ln det Clover)
    virtual Double S_even_even(const AbsFieldState<P,Q>& s)  = 0;

    //! Compute the odd odd contribution (eg
    virtual Double S_odd_odd(const AbsFieldState<P,Q>& s)
    {
      START_CODE();

      XMLWriter& xml_out = TheXMLOutputWriter::Instance();
      push(xml_out, "S_odd_odd");

      const EvenOddPrecWilsonTypeFermAct<Phi,P,Q>& FA = getFermAct();

      Handle< FermState<Phi,P,Q> > bc_g_state = FA.createState(s.getQ());

      // Need way to get gauge state from AbsFieldState<P,Q>
      Handle< EvenOddPrecLinearOperator<Phi,P,Q> > lin(FA.linOp(bc_g_state));
      // Get the X fields
      Phi X;

      // Action calc doesnt use chrono predictor use zero guess
      X[ lin->subset() ] = zero;

      // getX noe always uses chrono predictor. Best to Nuke it therefore
      (getMDSolutionPredictor()).reset();
      int n_count = getX(X, s);
      Double action = innerProductReal(getPhi(), X, lin->subset());
      
      write(xml_out, "n_count", n_count);
      write(xml_out, "S_oo", action);
      pop(xml_out);

      END_CODE();

      return action;
    }

    //! Compute the total action
    Double S(const AbsFieldState<P,Q>& s)  
    {
      START_CODE();

      XMLWriter& xml_out=TheXMLOutputWriter::Instance();
      push(xml_out, "TwoFlavorExactEvenOddPrecWilsonTypeFermMonomial");

      Double action = S_even_even(s) + S_odd_odd(s);

      write(xml_out, "S", action);
      pop(xml_out);

      END_CODE();

      return action;
    }

  protected:
    //! Get at fermion action
    virtual const EvenOddPrecWilsonTypeFermAct<Phi,P,Q>& getFermAct() const = 0;

    //! Get inverter params
    virtual const GroupXML_t& getInvParams(void) const = 0;

    //! Accessor for pseudofermion with Pf index i (read only)
    virtual const Phi& getPhi(void) const = 0;

    //! mutator for pseudofermion with Pf index i 
    virtual Phi& getPhi(void) = 0;    

    virtual AbsChronologicalPredictor4D<Phi>& getMDSolutionPredictor(void) = 0;
  };


  //------------------------------------------------------------------------
  //! Exact 2 degen flavor even-odd preconditioned fermact monomial
  /*! @ingroup monomial
   *
   * Exact 2 degen flavor even-odd preconditioned fermact monomial.
   * Constand even even determinant so can supplyt
   */
  template<typename P, typename Q, typename Phi>
  class TwoFlavorExactEvenOddPrecConstDetWilsonTypeFermMonomial : public TwoFlavorExactEvenOddPrecWilsonTypeFermMonomial<P,Q,Phi>
  {
  public:
     //! virtual destructor:
    ~TwoFlavorExactEvenOddPrecConstDetWilsonTypeFermMonomial() {}

    //! Even even contribution (eg For this kind of Monomial it is 0)
    virtual Double S_even_even(const AbsFieldState<P,Q>& s) {
      return Double(0);
    }

    
    // Inherit everything from Base Class
  protected:
    //! Get at fermion action
    //! For now the prototype is the same as before -- wait until we 
    //! refactor these before making them EvenOddPrecConstDetWilsonType...
    virtual const EvenOddPrecWilsonTypeFermAct<Phi,P,Q>& getFermAct() const = 0;
  };


  //------------------------------------------------------------------------
  //! Exact 2 degen flavor even-odd preconditioned fermact monomial
  /*! @ingroup monomial
   *
   * Exact 2 degen flavor even-odd preconditioned fermact monomial.
   * Constand even even determinant so can supplyt
   */
  template<typename P, typename Q, typename Phi>
  class TwoFlavorExactEvenOddPrecLogDetWilsonTypeFermMonomial : public TwoFlavorExactEvenOddPrecWilsonTypeFermMonomial<P,Q,Phi>
  {
  public:
     //! virtual destructor:
    ~TwoFlavorExactEvenOddPrecLogDetWilsonTypeFermMonomial() {}

    //! Even even contribution 
    virtual Double S_even_even(const AbsFieldState<P,Q>& s) 
    {
      START_CODE();

      const EvenOddPrecLogDetWilsonTypeFermAct<Phi,P,Q>& FA = getFermAct();
      Handle< FermState<Phi,P,Q> > bc_g_state = FA.createState(s.getQ());

      // Need way to get gauge state from AbsFieldState<P,Q>
      Handle< EvenOddPrecLogDetLinearOperator<Phi,P,Q> > lin(FA.linOp(bc_g_state));
      
      Double S_ee =(Double(-2)*lin->LogDetEvenEven());
      XMLWriter& xml_out = TheXMLOutputWriter::Instance();
      push(xml_out, "S_even_even");
      write(xml_out, "S_ee", S_ee);
      pop(xml_out);
      
      END_CODE();

      return S_ee;
    }

    //! Compute the total action
    Double S(const AbsFieldState<P,Q>& s)  
    {
      START_CODE();

      XMLWriter& xml_out=TheXMLOutputWriter::Instance();
      push(xml_out, "TwoFlavorExactEvenOddPrecLogDetWilsonTypeFermMonomial");

      Double S_ee = S_even_even(s);

      Double S_oo = S_odd_odd(s);

      Double action = S_ee + S_oo;

      write(xml_out, "S", action);
      pop(xml_out);

      END_CODE();

      return action;
    }

    //! Compute dsdq for the system... 
    /*! Monomial of the form  chi^dag*(M^dag*M)*chi + 2 TrLn A_ee */
    virtual void dsdq(P& F, const AbsFieldState<P,Q>& s)
    {
      START_CODE();

      // Self Description/Encapsulation Rule
      XMLWriter& xml_out = TheXMLOutputWriter::Instance();
      push(xml_out, "TwoFlavorExactEvenOddPrecLogDetWilsonTypeFermMonomial");

      /**** Identical code for unprec and even-odd prec case *****/
      
      // S_f  chi^dag_{oo}*(M^dag*M)_{oo}^(-1)*chi_{oo} + 2Tr Ln M_{ee}
      // Here, M is some operator.
      //
      // Need
      // dS_f/dU = - psi^dag * [d(M^dag)*M + M^dag*dM] * psi
      //           + 2Tr [ M_{ee}^{-1} dM_{ee} ] ,  psi = (M^dag*M)^(-1)*chi
      //

      // Create FermAct
      const EvenOddPrecLogDetWilsonTypeFermAct<Phi,P,Q>& FA = getFermAct();
      
      // Create a state for linop
      Handle< FermState<Phi,P,Q> > state(FA.createState(s.getQ()));
	
      //Create LinOp
      Handle< EvenOddPrecLogDetLinearOperator<Phi,P,Q> > lin(FA.linOp(state));

      P F_tmp;

      // Do the force computation. deriv() in these linops refers only
      // to the bit coming from the odd-odd bilinear -- this works in 
      // the normal way.
      Phi X, Y;

      // Get X out here
      // (getMDSolutionPredictor())(X);
      int n_count = getX(X,s);
      // (getMDSolutionPredictor()).newVector(X);
      
      (*lin)(Y, X, PLUS);

      lin->deriv(F, X, Y, MINUS);
      
      // fold M^dag into X^dag ->  Y  !!

      lin->deriv(F_tmp, Y, X, PLUS);
      F += F_tmp;
 
      for(int mu=0; mu < F.size(); ++mu)
	F[mu] *= Real(-1);

   
      // F now holds derivative with respect to possibly fat links
      // now derive it with respect to the thin links if needs be
      state->deriv(F);
      Double F_sq = norm2(F);
      
      lin->derivEvenEvenLogDet(F_tmp, PLUS);
      for(int mu =0; mu < Nd; mu++) { 
	F_tmp[mu] *= Real(-2); 
      }
      state->deriv(F_tmp);
      Double F_logdet_sq = norm2(F_tmp);
      write(xml_out, "n_count", n_count);
      write(xml_out, "F_oo_sq", F_sq);
      write(xml_out, "F_logdet_sq", F_logdet_sq);

      F += F_tmp;
      F_sq = norm2(F);
      write(xml_out, "F_sq", F_sq);
      pop(xml_out);

      END_CODE();
    }

    // Inherit everything from Base Class
  protected:
    //! Get at fermion action
    //! For now the prototype is the same as before -- wait until we 
    //! refactor these before making them EvenOddPrecConstDetWilsonType...
    virtual const EvenOddPrecLogDetWilsonTypeFermAct<Phi,P,Q>& getFermAct() const = 0;
  };

}


#endif
