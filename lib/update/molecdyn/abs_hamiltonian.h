#ifndef abs_hamiltonian_h
#define abs_hamiltonian_h

#include "chromabase.h"
#include "update/molecdyn/field_state.h"
#include "update/molecdyn/abs_monomial.h"


namespace Chroma { 

  /*! A hamiltonian is essentially an aggregator for Monomials 
   *  As such we get two kinds -- Exact Hamiltonians and inexact 
   *  ones */
  template<typename P, typename Q>
    class AbsHamiltonian {
    public:
    
    //! virtual descructor:
    virtual ~AbsHamiltonian() {}
    
    //! Compute dsdq for the system...
    //  The Category default goes through all the monomials
    //  and sums their contribution
    //
    //  s is the state, F is the computed force
    virtual void dsdq(P& F, const AbsFieldState<P,Q>& s) const {
      int num_terms = numMonomials();
      
      if( num_terms > 0 ) {
	getMonomial(0).dsdq(F, s);
	
	for(int i=1; i < num_terms; i++) { 
	  P cur_F;
	  getMonomial(i).dsdq(cur_F, s);
	  F += cur_F;
	}
      }
    }
    
    //! Refresh pseudofermsions (if any)
    virtual void refresh(const AbsFieldState<P,Q>& s) {
      int num_terms = numMonomials();
      for(int i=0; i < num_terms; i++) { 
	getMonomial(i).refresh(s);
      }
    }

    //! Copy Pseudo
    protected:
    
    //! Get hold of monomial with index i
    virtual Monomial<P,Q>& getMonomial(int i) const = 0;
    
    //! Get the number of monomials.
    virtual int numMonomials(void) const =0;
    
    
  };
  
  
  /*! Now define similar classes for exact algorithms.
   * These are basically the same as before but they can compute
   * energies too. Do these need to inerit?
   * Yes. Reason: We can always give it to an inexact algorithm through
   * a downcast. In that case the energy calculations would be hidden.
   */
  
  template<typename P, typename Q>
    class ExactAbsHamiltonian : public AbsHamiltonian<P, Q> {
    public:
    
    //! virtual descructor:
    virtual ~ExactAbsHamiltonian() {}

    //! Compute dsdq for the system...
    //  The Category default goes through all the monomials
    //  and sums their contribution
    //
    //  s is the state, F is the computed force
    virtual void dsdq(P& F, const AbsFieldState<P,Q>& s) const {
      int num_terms = numMonomials();
      
      if( num_terms > 0 ) {
	getMonomial(0).dsdq(F, s);
	
	for(int i=1; i < num_terms; i++) { 
	  P cur_F;
	  getMonomial(i).dsdq(cur_F, s);
	  F += cur_F;
	}
      }
    }

    //! Refresh pseudofermsions (if any)
    virtual void refresh(const AbsFieldState<P,Q>& s) {
      int num_terms = numMonomials();
      for(int i=0; i < num_terms; i++) { 
	getMonomial(i).refresh(s);
      }
    }
    
    //! Compute the energies 
    //! The total energy
    virtual void  mesE(const AbsFieldState<P,Q>& s, Double& KE, Double& PE) const 
    {
      KE = mesKE(s);
      PE = mesPE(s);
    }
    
    //! The Kinetic Energy
    virtual Double mesKE(const AbsFieldState<P,Q>& s) const 
    {
      // Return 1/2 sum pi^2
      // may need to loop over the indices of P?
      Double KE=norm2(s.getP());
      return KE;
    }
    
    //! The Potential Energy 
    virtual Double mesPE(const AbsFieldState<P,Q>& s) const 
    {
	
      // Cycle through all the monomials and compute their contribution
      int num_terms = numMonomials();
      
      
      Double PE=Double(0);
      if( num_terms > 0 ) {
	PE = getMonomial(0).S(s);
	QDPIO::cout << "MesPE: PE[0] = " << PE << endl;
	for(int i=1; i < num_terms; i++) { 
	  Double tmp = getMonomial(i).S(s);
	  PE += tmp;
	  QDPIO::cout <<"MesPE: PE["<<i<<"] = " << tmp << endl;
	}
      }
      QDPIO::cout << "MesPE: Total PE = " << PE << endl;
      return PE;
    }
    
    
    protected:
    
    //! Get hold of monomial with index i
    virtual ExactMonomial<P,Q>& getMonomial(int i) const = 0;
    
    //! Get the number of monomials.
    virtual int numMonomials(void) const = 0;
    
  };


}; // End namespace Chroma
#endif
