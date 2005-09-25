// -*- C++ -*-
// $Id: one_flavor_rat_monomial_w.h,v 2.0 2005-09-25 21:04:41 edwards Exp $

/*! @file
 * @brief One flavor monomials using RHMC
 */

#ifndef __one_flavor_monomial_w_h__
#define __one_flavor_monomial_w_h__

#include "update/molecdyn/monomial/abs_monomial.h"
#include "actions/ferm/fermacts/remez_coeff.h"

namespace Chroma
{
  //-------------------------------------------------------------------------------------------
  //! Exact 1 flavor fermact monomial using rational polynomials
  /*! @ingroup monomial
   *
   * Exact 1 flavor fermact monomial using Rational Polynomial. 
   * Preconditioning is not specified yet.
   * Can supply a default dsdq and pseudoferm refresh algorithm
   */
  template<typename P, typename Q, typename Phi>
  class OneFlavorRatExactWilsonTypeFermMonomial : public ExactWilsonTypeFermMonomial<P,Q,Phi>
  {
  public:
     //! virtual destructor:
    ~OneFlavorRatExactWilsonTypeFermMonomial() {}

    //! Compute the total action
    virtual Double S(const AbsFieldState<P,Q>& s)  = 0;

    //! Compute dsdq for the system... 
    /*! Actions of the form  chi^dag*(M^dag*M)*chi */
    virtual void dsdq(P& F, const AbsFieldState<P,Q>& s)
    {
      // Self Description/Encapsulation Rule
      XMLWriter& xml_out = TheXMLOutputWriter::Instance();
      push(xml_out, "OneFlavorRatExactWilsonTypeFermMonomial");

      /**** Identical code for unprec and even-odd prec case *****/
      
      /* The pseudofermionic action is of the form  S_pf = chi^dag N*D^(-1) chi
       * where N(Q) and D(Q) are polynomials in the rescaled kernel:
       *
       *     Q   = M^dag*M
       *     M   = is some operator
       *
       * The function x^(-alpha) is approximated by c*N(x)/D(x)
       * with c=FRatNorm. This has to be taken into account when comparing
       * the force with the standard HMC routine dsduf for alpha=1.  To solve
       * ( N(Q)/D(Q) )psi the rational function is reformed as a partial fraction
       * expansion and a multishift solver used to find the solution.
       *
       * Need
       * dS_f/dU = - \sum_i psi_i^dag * p_i * [d(M^dag)*M + M^dag*dM] * psi
       *    where    psi_i = (M^dag*M + q_i)^(-1)*chi
       *
       * In Balint's notation, the result is  
       * \dot{S} = -\sum_i p_i [ X_i^dag*\dot{M}^\dag*Y_i - Y_i^dag\dot{M}*X_i]
       * where
       *    X_i = (M^dag*M + q_i)^(-1)*chi   Y_i = M*X_i
       * In Robert's notation,  X_i -> psi_i .
       */
      const WilsonTypeFermAct<Phi,P>& FA = getFermAct();
      
      // Create a state for linop
      Handle< const ConnectState> state(FA.createState(s.getQ()));
	
      // Need way to get gauge state from AbsFieldState<P,Q>
      Handle< const DiffLinearOperator<Phi,P> > lin(FA.linOp(state));

      // Partial Fraction Expansion coeffs for force
      const RemezCoeff_t& fpfe = getFPFE();

      multi1d<Phi> X;
      Phi Y;

      P  F_1, F_2, F_tmp(Nd);
      F.resize(Nd);
      F = zero;

      // Loop over nth-roots, so the pseudoferms
      multi1d<int> n_count(getNthRoot());
      multi1d<Real> F_sq(getNthRoot());

      QDPIO::cout << "nthRoot = " << getNthRoot() << endl;

      for(int n=0; n < getNthRoot(); ++n)
      {
	// Get X out here via multisolver
	n_count[n] = getX(X,fpfe.pole,getPhi()[n],s);

	// Loop over solns and accumulate force contributions
	F_tmp = zero;
	for(int i=0; i < X.size(); ++i)
	{
	  (*lin)(Y, X[i], PLUS);

	  // The  d(M^dag)*M  term
	  lin->deriv(F_1, X[i], Y, MINUS);
      
	  // The  M^dag*d(M)  term
	  lin->deriv(F_2, Y, X[i], PLUS);
	  F_1 += F_2;

	  // Reweight each contribution in partial fraction
	  for(int mu=0; mu < F.size(); mu++)
	    F_tmp[mu] -= fpfe.res[i] * F_1[mu];
	}

	F += F_tmp;
	F_sq[n] = norm2(F_tmp);   // monitor force from each nth-root
      }

      Real F_sq_tot = norm2(F);   // monitor total force

      write(xml_out, "n_count", n_count);
      write(xml_out, "F_sq", F_sq);
      write(xml_out, "F_sq_tot", F_sq);
      pop(xml_out);
    }

  
    //! Refresh pseudofermions
    /*!
     * This routine calculates the pseudofermion field (chi) for the case
     * of rational evolution
     *
     *           chi =  n(Q)*[d(Q)]^(-1) * eta
     * Where:    Q   = M^dag*M
     *           d(Q) = (Q+q_1)*(Q+q_2)*...*(Q+q_m)
     *           n(Q) = (Q+p_1)*(Q+p_2)*...  *(Q+p_m)
     *           m  = HBRatDeg
     *	     							
     * The rational function n(x)/d(x) is the optimal rational
     * approximation to the inverse square root of N(x)/D(x) which in
     * turn is the optimal rational approximation to x^(-alpha).
     * Here, alpha = 1/2
     *
     * To solve {n(Q)*[d(Q)]^(-1) * eta} the partial fraction expansion is
     * used in combination with a multishift solver.
     */
    virtual void refreshInternalFields(const AbsFieldState<P,Q>& s) 
    {
      // Heatbath all the fields
      
      // Self Description/Encapsulation Rule
      XMLWriter& xml_out = TheXMLOutputWriter::Instance();
      push(xml_out, "OneFlavorRatExactWilsonTypeFermMonomial");

      // Get at the ferion action for piece i
      const WilsonTypeFermAct<Phi,P>& S_f = getFermAct();
      
      // Create a Connect State, apply fermionic boundaries
      Handle< const ConnectState > f_state(S_f.createState(s.getQ()));
      
      // Create a linear operator
      Handle< const LinearOperator<Phi> > M(S_f.linOp(f_state));
      
      // Partial Fraction Expansion coeffs for heat-bath
      const RemezCoeff_t& sipfe = getSIPFE();

      // Loop over nth-roots, so the pseudoferms
      getPhi().resize(getNthRoot());
      multi1d<int> n_count(getNthRoot());
      Phi eta;

      for(int n=0; n < getNthRoot(); ++n)
      {
	// Fill the eta field with gaussian noise
	eta = zero;
	gaussian(eta, M->subset());
      
	// Temporary: Move to correct normalisation
	eta *= sqrt(0.5);
      
	// Get X out here via multisolver
	multi1d<Phi> X;
	n_count[n] = getX(X,sipfe.pole,eta,s);

	// Weight solns to make final PF field
	getPhi()[n][M->subset()] = sipfe.norm * eta;
	for(int i=0; i < X.size(); ++i)
	  getPhi()[n][M->subset()] += sipfe.res[i] * X[i];
      }

      write(xml_out, "n_count", n_count);
      pop(xml_out);
    }				    
  
    //! Copy pseudofermions if any
    virtual void setInternalFields(const Monomial<P,Q>& m) {
      try {
	const OneFlavorRatExactWilsonTypeFermMonomial<P,Q,Phi>& fm = dynamic_cast<  const OneFlavorRatExactWilsonTypeFermMonomial<P,Q,Phi>& >(m);

	getPhi() = fm.getPhi();
      }
      catch(bad_cast) { 
	QDPIO::cerr << "Failed to cast input Monomial to OneFlavorRatExactWilsonTypeFermMonomial " << endl;
	QDP_abort(1);
      }
    }


    //! Compute the action on the appropriate subset
    /*!
     * This measures the pseudofermion contribution to the Hamiltonian
     * for the case of rational evolution (with polynomials n(x) and d(x),
     * of degree SRatDeg
     *
     * S_f = chi_dag * (n(A)*d(A)^(-1))^2* chi
     *
     * where A is M^dag*M
     *
     * The rational function n(x)/d(x) is the optimal rational
     * approximation to the square root of N(x)/D(x) which in
     * turn is the optimal rational approximation to x^(-alpha).
     * Here, alpha = 1/2
     */
    virtual Double S_subset(const AbsFieldState<P,Q>& s) const
    {
      XMLWriter& xml_out = TheXMLOutputWriter::Instance();
      push(xml_out, "S_subset");

      const WilsonTypeFermAct<Phi,P>& FA = getFermAct();

      Handle<const ConnectState> bc_g_state = FA.createState(s.getQ());

      // Need way to get gauge state from AbsFieldState<P,Q>
      Handle< const LinearOperator<Phi> > lin(FA.linOp(bc_g_state));

      // Partial Fraction Expansion coeffs for action
      const RemezCoeff_t& spfe = getSPFE();

      // Compute energy
      // Get X out here via multisolver
      multi1d<Phi> X;

      // Loop over nth-roots, so the pseudoferms
      multi1d<int> n_count(getNthRoot());
      Double action = zero;
      Phi psi;

      for(int n=0; n < getNthRoot(); ++n)
      {
	n_count[n] = getX(X,spfe.pole,getPhi()[n],s);

	// Weight solns to make final PF field
	psi[lin->subset()] = spfe.norm * getPhi()[n];
	for(int i=0; i < X.size(); ++i)
	  psi[lin->subset()] += spfe.res[i] * X[i];

	action += norm2(psi, lin->subset());
      }

      write(xml_out, "n_count", n_count);
      write(xml_out, "S", action);
      pop(xml_out);

      return action;
    }


  protected:
    //! Get at fermion action
    virtual const WilsonTypeFermAct<Phi,P>& getFermAct(void) const = 0;

    //! Return number of roots in used
    virtual int getNthRoot() const = 0;

    //! Return the partial fraction expansion for the force calc
    virtual const RemezCoeff_t& getFPFE() const = 0;

    //! Return the partial fraction expansion for the action calc
    virtual const RemezCoeff_t& getSPFE() const = 0;

    //! Return the partial fraction expansion for the heat-bath
    virtual const RemezCoeff_t& getSIPFE() const = 0;

    //! Accessor for pseudofermion (read only)
    virtual const multi1d<Phi>& getPhi(void) const = 0;

    //! mutator for pseudofermion
    virtual multi1d<Phi>& getPhi(void) = 0;    

    //! Multi-mass solver  (M^dagM + q_i)^{-1} chi  using partfrac
    virtual int getX(multi1d<Phi>& X, 
		     const multi1d<Real>& shifts, 
		     const Phi& chi, 
		     const AbsFieldState<P,Q>& s) const = 0;
  };


  //-------------------------------------------------------------------------------------------
  //! Exact 1 flavor unpreconditioned fermact monomial
  /*! @ingroup monomial
   *
   * Exact 1 flavor unpreconditioned fermact monomial.
   */
  template<typename P, typename Q, typename Phi>
  class OneFlavorRatExactUnprecWilsonTypeFermMonomial : public OneFlavorRatExactWilsonTypeFermMonomial<P,Q,Phi>
  {
  public:
     //! virtual destructor:
    ~OneFlavorRatExactUnprecWilsonTypeFermMonomial() {}

    //! Compute the total action
    virtual Double S(const AbsFieldState<P,Q>& s)
    {
      // Self identification/encapsulation Rule
      XMLWriter& xml_out = TheXMLOutputWriter::Instance();
      push(xml_out, "OneFlavorRatExactUnprecWilsonTypeFermMonomial");

      Double action = S_subset(s);

      write(xml_out, "S", action);
      pop(xml_out);

      return action;
    }

  protected:
    //! Get at fermion action
    virtual const UnprecWilsonTypeFermAct<Phi,P>& getFermAct(void) const = 0;

    //! Return number of roots in used
    virtual int getNthRoot() const = 0;

    //! Return the partial fraction expansion for the force calc
    virtual const RemezCoeff_t& getFPFE() const = 0;

    //! Return the partial fraction expansion for the action calc
    virtual const RemezCoeff_t& getSPFE() const = 0;

    //! Return the partial fraction expansion for the heat-bath
    virtual const RemezCoeff_t& getSIPFE() const = 0;

    //! Accessor for pseudofermion (read only)
    virtual const multi1d<Phi>& getPhi(void) const = 0;

    //! mutator for pseudofermion
    virtual multi1d<Phi>& getPhi(void) = 0;    

    //! Multi-mass solver  (M^dagM + q_i)^{-1} chi  using partfrac
    virtual int getX(multi1d<Phi>& X, 
		     const multi1d<Real>& shifts, 
		     const Phi& chi, 
		     const AbsFieldState<P,Q>& s) const = 0;
  };


  //-------------------------------------------------------------------------------------------
  //! Exact 1 flavor even-odd preconditioned fermact monomial
  /*! @ingroup monomial
   *
   * Exact 1 flavor even-odd preconditioned fermact monomial.
   * Can supply a default dsdq algorithm
   */
  template<typename P, typename Q, typename Phi>
  class OneFlavorRatExactEvenOddPrecWilsonTypeFermMonomial : public OneFlavorRatExactWilsonTypeFermMonomial<P,Q,Phi>
  {
  public:
     //! virtual destructor:
    ~OneFlavorRatExactEvenOddPrecWilsonTypeFermMonomial() {}

    //! Even even contribution (eg ln det Clover)
    virtual Double S_even_even(const AbsFieldState<P,Q>& s) = 0;

    //! Compute the odd odd contribution (eg
    virtual Double S_odd_odd(const AbsFieldState<P,Q>& s)
    {
      return S_subset(s);
    }

    //! Compute the total action
    Double S(const AbsFieldState<P,Q>& s)
    {
      XMLWriter& xml_out=TheXMLOutputWriter::Instance();
      push(xml_out, "OneFlavorRatExactEvenOddPrecWilsonTypeFermMonomial");

      Double action_e = S_even_even(s);
      Double action_o = S_odd_odd(s);
      Double action   = action_e + action_o;

      write(xml_out, "S_even_even", action_e);
      write(xml_out, "S_odd_odd", action_o);
      write(xml_out, "S", action);
      pop(xml_out);

      return action;
    }

  protected:
    //! Get at fermion action
    virtual const EvenOddPrecWilsonTypeFermAct<Phi,P>& getFermAct() const = 0;

    //! Return number of roots in used
    virtual int getNthRoot() const = 0;

    //! Return the partial fraction expansion for the force calc
    virtual const RemezCoeff_t& getFPFE() const = 0;

    //! Return the partial fraction expansion for the action calc
    virtual const RemezCoeff_t& getSPFE() const = 0;

    //! Return the partial fraction expansion for the heat-bath
    virtual const RemezCoeff_t& getSIPFE() const = 0;

    //! Accessor for pseudofermion (read only)
    virtual const multi1d<Phi>& getPhi(void) const = 0;

    //! mutator for pseudofermion
    virtual multi1d<Phi>& getPhi(void) = 0;    

    //! Multi-mass solver  (M^dagM + q_i)^{-1} chi  using partfrac
    virtual int getX(multi1d<Phi>& X, 
		     const multi1d<Real>& shifts, 
		     const Phi& chi, 
		     const AbsFieldState<P,Q>& s) const = 0;
  };

}


#endif
