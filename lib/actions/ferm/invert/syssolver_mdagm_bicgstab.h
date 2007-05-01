// -*- C++ -*-
// $Id: syssolver_mdagm_bicgstab.h,v 3.1 2007-05-01 14:39:13 bjoo Exp $
/*! \file
 *  \brief Solve a MdagM*psi=chi linear system by BiCGStab
 */

#ifndef __syssolver_mdagm_bicgstab_h__
#define __syssolver_mdagm_bicgstab_h__
#include "chroma_config.h"

#include "handle.h"
#include "syssolver.h"
#include "linearop.h"
#include "actions/ferm/invert/syssolver_mdagm.h"
#include "actions/ferm/invert/syssolver_bicgstab_params.h"
#include "actions/ferm/invert/invbicgstab.h"
#ifdef CHROMA_DO_ONE_CG_RESTART
#include "actions/ferm/invert/invcg2.h"
#endif 

namespace Chroma
{

  //! BiCGStab system solver namespace
  namespace MdagMSysSolverBiCGStabEnv
  {
    //! Name to be used
    extern const std::string name;

    //! Register the syssolver
    bool registerAll();
  }


  //! Solve a BiCGStab system. Here, the operator is NOT assumed to be hermitian
  /*! \ingroup invert
   */
  template<typename T>
  class MdagMSysSolverBiCGStab : public MdagMSystemSolver<T>
  {
  public:
    //! Constructor
    /*!
     * \param M_        Linear operator ( Read )
     * \param invParam  inverter parameters ( Read )
     */
    MdagMSysSolverBiCGStab(Handle< LinearOperator<T> > A_,
			   const SysSolverBiCGStabParams& invParam_) : 
      A(A_), invParam(invParam_) 
    {}

    //! Destructor is automatic
    ~MdagMSysSolverBiCGStab() {}

    //! Return the subset on which the operator acts
    const Subset& subset() const {return A->subset();}

    //! Solver the linear system
    /*!
     * \param psi      solution ( Modify )
     * \param chi      source ( Read )
     * \return syssolver results
     */
    SystemSolverResults_t operator() (T& psi, const T& chi) const
    {
	START_CODE();

	SystemSolverResults_t res1,res2,res3;  // initialized by a constructor
	
	// ( M^\dag M )^{-1} => M^{-1} M^{-dag}
	// so ( M^\dag M )^{-1} X = \phi
	// => X = M^{-1} M^{-dag} \phi
	//      = M%{-1} Y        Y = M^{-\dag} \phi
	T Y = psi;
	res1 = InvBiCGStab(*A, chi, Y, invParam.RsdBiCGStab, invParam.MaxBiCGStab, MINUS );
	res2 = InvBiCGStab(*A, Y, psi, invParam.RsdBiCGStab, invParam.MaxBiCGStab, PLUS );

	res3.n_count = res2.n_count + res1.n_count;
	res3.resid = res2.resid;


	{ // Find true residuum
	  Y=zero;
	  T re=zero;
	  (*A)(Y, psi, PLUS);
	  (*A)(re,Y, MINUS);
	  re[A->subset()] -= chi;
	  QDPIO::cout << "MDAGM_BICGSTAB: n=" 
		      << res3.n_count 
		      << " resid=" << sqrt(norm2(re,A->subset())) << endl;
	}
#ifdef CHROMA_DO_ONE_CG_RESTART
	// CG Polish - should be very quick
	res3 = InvCG2(*A, chi, psi, invParam.RsdBiCGStab, invParam.MaxBiCGStab);	res3.n_count += res2.n_count + res1.n_count;
#endif
	

	END_CODE();

	return res3;
    }


  private:
    // Hide default constructor
    MdagMSysSolverBiCGStab() {}

    Handle< LinearOperator<T> > A;
    SysSolverBiCGStabParams invParam;
  };


} // End namespace

#endif 

