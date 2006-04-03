// $Id: lcm_pqp_leapfrog_mts.h,v 3.0 2006-04-03 04:59:07 edwards Exp $
/*! @file
 * @brief Second order minimal norm (2MN) integrator with multiple time scales
 *
 * 2MN intregator with multiple time scales for HMC
 */

#ifndef LCM_PQP_LEAPFROG_MTS_H
#define LCM_PQP_LEAPFROG_MTS_H


#include "chromabase.h"
#include "update/molecdyn/hamiltonian/abs_hamiltonian.h"
#include "update/molecdyn/integrator/abs_integrator.h"
#include "update/molecdyn/integrator/lcm_integrator_leaps.h"
#include <string>
#include <iostream>

namespace Chroma 
{

  /*! @ingroup integrator */
  namespace LatColMatPQPLeapfrogIntegratorMtsEnv {
    extern const std::string name;
    extern const bool registered;
  };

  /*! @ingroup integrator */
  struct  LatColMatPQPLeapfrogIntegratorMtsParams {
    LatColMatPQPLeapfrogIntegratorMtsParams();
    LatColMatPQPLeapfrogIntegratorMtsParams(XMLReader& xml, const std::string& path);

    int number_of_timescales; // Number of timescales
    Real tau; // trajectory length

    multi1d<int> n_steps_list;  // number of integration steps sorted by timescales
    multi1d< multi1d<int> > monomial_list; // Indices of monomials sorted by timescales

  };

  /*! @ingroup integrator */
  void read(XMLReader& xml_in, 
	    const std::string& path,
	    LatColMatPQPLeapfrogIntegratorMtsParams& p);

  /*! @ingroup integrator */
  void write(XMLWriter& xml_out,
	     const std::string& path, 
	     const LatColMatPQPLeapfrogIntegratorMtsParams& p);

  //! MD integrator interface for PQP leapfrog
  /*! @ingroup integrator
   *  Specialised to multi1d<LatticeColorMatrix>
   */
  class LatColMatPQPLeapfrogIntegratorMts 
    : public AbsMDIntegrator<multi1d<LatticeColorMatrix>,
    multi1d<LatticeColorMatrix> > {
    
    public:
    
    // Construct from params struct and Hamiltonian
    LatColMatPQPLeapfrogIntegratorMts(const LatColMatPQPLeapfrogIntegratorMtsParams& p,
				  Handle< AbsHamiltonian< multi1d<LatticeColorMatrix>, multi1d<LatticeColorMatrix> > >& H_) : n_steps_list(p.n_steps_list), tau(p.tau), H_MD(H_), monomial_list(p.monomial_list), number_of_timescales(p.number_of_timescales) {
      
      // Check the number of timescales
      if(number_of_timescales != monomial_list.size()) {
	QDPIO::cerr << "Error: Wrong number of time scales specified for PQP Leapfrog MTS integrator." << endl;
	QDPIO::cerr << "Integrator has    " << number_of_timescales << endl;
	QDPIO::cerr << "Monomial list has " << monomial_list.size() << endl;
	QDP_abort(1);
      }
      
      // Check for sanity
      int num_ham_monomials = getHamiltonian().numMonomials();

      // Sum up the number of monomials available
      int num_monomials = 0;
      for(int i = 0; i < p.monomial_list.size1(); i++) {
	num_monomials += p.monomial_list[i].size1();
      }

      // Check that the number of short timescale monomials + 
      //   the number of long_timescale monomials equals the total number of 
      //   monomials in the hamiltonian
      if( num_ham_monomials != num_monomials ) {
	QDPIO::cerr << "Error: Wrong number of monomials specified for PQP Leapfrog MTS integrator." << endl;
	QDPIO::cerr << "Hamiltonian has " << num_ham_monomials << endl;
	QDPIO::cerr << "Integrator has  " << num_monomials << endl;
	QDP_abort(1);
      }

      // Check that a monomial has not been used twice and that 
      // no bogus indices are specified
      multi1d<bool> exclusivity_check(num_ham_monomials);
     
      // Initialise filter -- set all elements to false
      for(int i=0; i < num_ham_monomials; i++) { 
	exclusivity_check[i] = false;
      }

      // Go through list of monomials
      for(int i=0; i < monomial_list.size1(); i++) {
	for(int j = 0; j < monomial_list[i].size1(); j++) {
	  // Check for bogus index
	  if( monomial_list[i][j] < 0 || 
	      monomial_list[i][j] >= num_ham_monomials ) { 
	    
	    QDPIO::cerr << "Index out of range. monomial_list["<<i<<"]["<<j<<" has index " 
			<< monomial_list[i][j] << " but max index is " << num_ham_monomials-1 << endl;
	    QDP_abort(1);
	  }
	  
	  // If index is not bogus, check it has not been used already
	  // ie only insert it if exclusivity_check[i] is false
	  if( exclusivity_check[monomial_list[i][j]] == true ) { 
	    QDPIO::cerr << "Exclusivity check failed. Monomial " << i << " is listed more than once in list of monomials" << endl;
	    QDP_abort(1);
	  }
	  else { 
	    // Its not there yet, so insert it
	    exclusivity_check[monomial_list[i][j]] = true;
	  }
        }
      }

      // Now go through exclusivity check array. If a member is false,
      // it means that that monomial term has been missed in both
      // the short and long monomial lists
      for(int i=0; i < num_ham_monomials; i++) { 
	if( exclusivity_check[i] == false ) { 
	  QDPIO::cerr << "Monomial " << i << " not used in monomial list " << endl;
	  QDP_abort(1);
	}
      }

      // At this point we have checked that
      //  -- the number of monomials sums to the correct
      //     number of terms
      //  -- that all the monomials have been marked one way or the other
      //  -- that there are no duplicates between the lists

    }

    // Copy constructor
    LatColMatPQPLeapfrogIntegratorMts(const LatColMatPQPLeapfrogIntegratorMts& l) :
      n_steps_list(l.n_steps_list), tau(l.tau), H_MD(l.H_MD), monomial_list(l.monomial_list), number_of_timescales(l.number_of_timescales)  {}

    // ! Destruction is automagic
    ~LatColMatPQPLeapfrogIntegratorMts(void) {};

    //! Get at the MD Hamiltonian
    AbsHamiltonian<multi1d<LatticeColorMatrix>,
      multi1d<LatticeColorMatrix> >& getHamiltonian(void) {
      return *H_MD;
    }

    //! Do a trajectory
    void operator()(AbsFieldState<multi1d<LatticeColorMatrix>,
		    multi1d<LatticeColorMatrix> >& s) {
      int recursive_seed = number_of_timescales-1;

      // The half step at the beginning for all the momenta
      Real dtau  = tau/Real(n_steps_list[recursive_seed]*2);
      for(int i = number_of_timescales-1; i > 0; i--) {
	leapP(monomial_list[i], dtau, s);
	dtau /= Real(n_steps_list[i-1]);
      }
      leapP(monomial_list[0], dtau, s);

      // The rest we do recursively
      recursive_integrator(recursive_seed, tau, true, s);

      // The half step at the end for all the momenta
      dtau = tau/Real(n_steps_list[recursive_seed]*2);
      for(int i = number_of_timescales-1; i > 0; i--) {
	leapP(monomial_list[i], dtau, s);
	dtau /= Real(n_steps_list[i-1]);
      }
      leapP(monomial_list[0], dtau, s);
    }

    protected:

    void recursive_integrator(const int recursion_index, const Real tau0, const bool halfstep,
			      AbsFieldState<multi1d<LatticeColorMatrix>,
			      multi1d<LatticeColorMatrix> >& s) {
      Real dtau = tau0/Real(n_steps_list[recursion_index]);

      if(recursion_index == 0) {
	for(int i = 1; i < n_steps_list[recursion_index]; i++) {
	  leapQ(dtau, s);
	  leapP(monomial_list[recursion_index], 
		dtau, s);
	}
	leapQ(dtau, s);
	if(!halfstep) {
	  leapP(monomial_list[recursion_index], 
		dtau, s);
	}
      }
      else {
	for(int i = 1; i < n_steps_list[recursion_index]; i++){
	  recursive_integrator(recursion_index-1, dtau, false, s);
	  leapP(monomial_list[recursion_index], 
		dtau, s);
	}
	if(recursion_index == (number_of_timescales-1)) {
	  recursive_integrator(recursion_index-1, dtau, true, s);
	}
	else {
	  recursive_integrator(recursion_index-1, dtau, halfstep, s);
	}
	if(!halfstep) {
	  leapP(monomial_list[recursion_index],
		dtau, s);
	}
      }
    }

    //! LeapP for just a selected list of monomials
    void leapP(const multi1d<int>& monomial_list,
	       const Real& dt, 
	       AbsFieldState<multi1d<LatticeColorMatrix>,
	       multi1d<LatticeColorMatrix> >& s) {

      LCMMDIntegratorSteps::leapP(monomial_list, 
				  dt, 
				  getHamiltonian(),
				  s);
    }

    //! Leap with Q (with all monomials)
    void leapQ(const Real& dt, 
	       AbsFieldState<multi1d<LatticeColorMatrix>,
	       multi1d<LatticeColorMatrix> >& s) {
      LCMMDIntegratorSteps::leapQ(dt,s);
    }




    //! Get the trajectory length
    const Real getTrajLength(void) const {
      return tau;
    }


    private:
    multi1d<int> n_steps_list;
    multi1d< multi1d<int> > monomial_list;
    Real tau;
    int number_of_timescales;
    Handle< AbsHamiltonian<multi1d<LatticeColorMatrix>, 
      multi1d<LatticeColorMatrix> > > H_MD;
    
    
  };

};


#endif
