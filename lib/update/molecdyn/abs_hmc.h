#ifndef abs_hmc_h
#define abs_hmc_h

#include "chromabase.h"
#include "update/molecdyn/field_state.h"
#include "update/molecdyn/abs_hamiltonian.h"
#include "update/molecdyn/abs_integrator.h"
#include "update/molecdyn/global_metropolis_accrej.h"


using namespace QDP;
using namespace std;
using namespace Chroma;

namespace Chroma { 

  template<typename P, typename Q>
  class AbsHMC {
  public: 
    
    // Virtual destructor
    virtual ~AbsHMC() {};
    

    // Traj accessor
    virtual const int& getTrajNum(void) const = 0;

    // Do the HMC trajectory
    virtual void operator()(AbsFieldState<P,Q>& s,
			    const bool doAccep)
      
    {
      const AbsMDIntegrator<P,Q>& MD = getMDIntegrator();
      const ExactAbsHamiltonian<P,Q>& H_MC = getMCHamiltonian();
      
      // HMC Algorithm.
      // 1) Refresh momenta
      //
      refreshP(s);
      
      // Refresh Pseudofermions
      H_MC.refresh(s);
      
      // 2) SaveState -- Perhaps this could be done better?
      Handle< AbsFieldState<P,Q> >  s_old(s.clone());
      
      // 3) Integrate MD trajectory
      MD(s);
      
      // 4) Measure energy of the old state
      Double KE_old, PE_old;
      H_MC.mesE(*s_old, KE_old, PE_old);
      
      // 5) Measure the energy of the new state
      Double KE, PE;
      H_MC.mesE(s, KE, PE);
      
      // Work out energy differences
      Double DeltaKE = KE - KE_old;
      Double DeltaPE = PE - PE_old;
      Double DeltaH  = DeltaKE + DeltaPE;
      
      // If we intend to do an accept reject step
      // (ie we are not warming up)
      if( doAccept ) { 
	
	// Measure Acceptance
	bool acceptTestResult = acceptReject(DeltaH);
	
	// If rejected restore fields
	// If accepted no need to do anything
	if ( ! acceptTestResult ) { 
	  s.getQ() = old_state->getQ();
	  
	  // I am going to refresh these so maybe these copies 
	  //  for the momenta and the pseudofermions
	  // are not really needed...
	  //
	  // restore momenta
	  s.getP() = old_state->getP();
	}
      }
      
      // Increase trajectory count
      getTrajNum()++;
      
    }
    
  protected:
    // Get at the Exact Hamiltonian -- It would be great if I could
    // enforce this to be exact. Perhaps I should request that it be
    // an AbsExactHamiltonian<P,Q> 
    virtual ExactAbsHamiltonian<P,Q>& getMCHamiltonian(void) const = 0;
    
    // Get at the HybridIntegrator -- Again I could ask for this
    // to come from a base class but this one may be a complex beastie
    // with monitoring and such so it is bes to template this
    virtual AbsMDIntegrator<P,Q>& getMDIntegrator(void) const = 0;
    
    // Traj mutator
    virtual int& getTrajNum(void) = 0;

    virtual void refreshP(AbsFieldState<P,Q>& state) const = 0;
  
    
    virtual bool acceptReject(const Double& DeltaH) const = 0;
    
  };


}; // end namespace chroma 



#endif
