// -*- C++ -*-
// $Id: lcm_pqp_leapfrog.h,v 3.2 2006-09-20 20:28:05 edwards Exp $

/*! @file
 * @brief Leapfrog integrator
 *
 * Leapfrog intregator for HMC
 */

#ifndef LCM_PQP_LEAPFROG_H
#define LCM_PQP_LEAPFROG_H


#include "chromabase.h"
#include "update/molecdyn/hamiltonian/abs_hamiltonian.h"
#include "update/molecdyn/integrator/abs_integrator.h"
#include "update/molecdyn/integrator/lcm_integrator_leaps.h"
#include <string>


namespace Chroma 
{

  /*! @ingroup integrator */
  namespace LatColMatPQPLeapfrogIntegratorEnv 
  {
    extern const std::string name;
    bool registerAll();
  }


  /*! @ingroup integrator */
  struct  LatColMatPQPLeapfrogIntegratorParams 
  {
    LatColMatPQPLeapfrogIntegratorParams();
    LatColMatPQPLeapfrogIntegratorParams(XMLReader& xml, const std::string& path);

    int  n_steps;
    Real tau0;
  };

  /*! @ingroup integrator */
  void read(XMLReader& xml_in, 
	    const std::string& path,
	    LatColMatPQPLeapfrogIntegratorParams& p);

  /*! @ingroup integrator */
  void write(XMLWriter& xml_out,
	     const std::string& path, 
	     const LatColMatPQPLeapfrogIntegratorParams& p);

  //! MD integrator interface for PQP leapfrog
  /*! @ingroup integrator
   *  Specialised to multi1d<LatticeColorMatrix>
   */
  class LatColMatPQPLeapfrogIntegrator 
    : public PQPLeapfrogIntegrator<multi1d<LatticeColorMatrix>,
				   multi1d<LatticeColorMatrix> > 
  {
  public:

    // Simplest Constructor
    LatColMatPQPLeapfrogIntegrator(int  n_steps_, 
				   const Real& tau_,
				   Handle< AbsHamiltonian< multi1d<LatticeColorMatrix>, multi1d<LatticeColorMatrix> > >& H_ ) : n_steps(n_steps_), tau0(tau_), H_MD(H_) {};

    // Construct from params struct and Hamiltonian
    LatColMatPQPLeapfrogIntegrator(const LatColMatPQPLeapfrogIntegratorParams& p,
				   Handle< AbsHamiltonian< multi1d<LatticeColorMatrix>, multi1d<LatticeColorMatrix> > >& H_) : n_steps(p.n_steps), tau0(p.tau0), H_MD(H_) {}

    // Copy constructor
    LatColMatPQPLeapfrogIntegrator(const LatColMatPQPLeapfrogIntegrator& l) :
      n_steps(l.n_steps), tau0(l.tau0), H_MD(l.H_MD) {}

    // ! Destruction is automagic
    ~LatColMatPQPLeapfrogIntegrator(void) {};

    //! Get at the MD Hamiltonian
    AbsHamiltonian<multi1d<LatticeColorMatrix>,
		   multi1d<LatticeColorMatrix> >& getHamiltonian(void) {
      return *H_MD;
    }


  protected:

    void leapP(const Real& dt, 
	       AbsFieldState<multi1d<LatticeColorMatrix>,
	       multi1d<LatticeColorMatrix> >& s) 
    {
      START_CODE();

      LCMMDIntegratorSteps::leapP(dt, getHamiltonian(), s);
    
      END_CODE();
    }

    //! Leap with Q
    void leapQ(const Real& dt, 
	       AbsFieldState<multi1d<LatticeColorMatrix>,
			     multi1d<LatticeColorMatrix> >& s) 
    {
      START_CODE();

      LCMMDIntegratorSteps::leapQ(dt, s);
    
      END_CODE();
    }

    //! Get the trajectory length
    const Real getTrajLength(void) const {
      return tau0;
    }

    //! Get the step size 
    const Real getStepSize(void) const {
      return tau0/Real(n_steps);
    }

    //! Get the number of steps
    int getNumSteps(void) const { 
      return n_steps;
    }

  private:
    int  n_steps;
    Real tau0;
    Handle< AbsHamiltonian<multi1d<LatticeColorMatrix>, 
			    multi1d<LatticeColorMatrix> > > H_MD;
    
    
  };

}


#endif
