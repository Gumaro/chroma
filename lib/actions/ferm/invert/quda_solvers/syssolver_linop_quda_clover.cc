// $Id: syssolver_linop_quda_clover.cc,v 1.1 2009-09-29 23:10:30 bjoo Exp $
/*! \file
 *  \brief Solve a MdagM*psi=chi linear system by CG2
 */

#include "actions/ferm/invert/syssolver_linop_factory.h"
#include "actions/ferm/invert/syssolver_linop_aggregate.h"
#include "actions/ferm/invert/quda_solvers/syssolver_quda_clover_params.h"
#include "actions/ferm/invert/quda_solvers/syssolver_linop_quda_clover.h"
#include "io/aniso_io.h"


#include "handle.h"
#include "actions/ferm/fermstates/periodic_fermstate.h"
#include "actions/ferm/linop/lwldslash_w.h"
#include "meas/glue/mesplq.h"
// QUDA Headers
#include <quda.h>
#include <util_quda.h>

namespace Chroma
{
  namespace LinOpSysSolverQUDACloverEnv
  {

    //! Anonymous namespace
    namespace
    {
      //! Name to be used
      const std::string name("QUDA_CLOVER_INVERTER");

      //! Local registration flag
      bool registered = false;
    }



    LinOpSystemSolver<LatticeFermion>* createFerm(XMLReader& xml_in,	
						  const std::string& path,
						  Handle< FermState< LatticeFermion, multi1d<LatticeColorMatrix>, multi1d<LatticeColorMatrix> > > state, 
						  
						  Handle< LinearOperator<LatticeFermion> > A)
    {
      return new LinOpSysSolverQUDAClover(A, state,SysSolverQUDACloverParams(xml_in, path));
    }

    //! Register all the factories
    bool registerAll() 
    {
      bool success = true; 
      if (! registered)
      {
	success &= Chroma::TheLinOpFermSystemSolverFactory::Instance().registerObject(name, createFerm);
	registered = true;
      }
      return success;
    }
  }

  SystemSolverResults_t 
  LinOpSysSolverQUDAClover::qudaInvert(const QF& links, 
				       const QDPCloverTermT<TF, UF>& clover,
				       const QDPCloverTermT<TF, UF>& invclov,
				       const TF& chi_s,
				       TF& psi_s) const{

    SystemSolverResults_t ret;
    QudaGaugeParam q_gauge_param;
    QudaInvertParam inv_param;
    const multi1d<int>& latdims = Layout::lattSize();

    QF links_trans;

    // Kappa norm chi

    q_gauge_param.X[0] = latdims[0];
    q_gauge_param.X[1] = latdims[1];
    q_gauge_param.X[2] = latdims[2];
    q_gauge_param.X[3] = latdims[3];

 
    const AnisoParam_t& aniso = invParam.CloverParams.anisoParam;

    // Convention: BC has to be applied already
    // This flag just tells QUDA that this is so,
    // so that QUDA can take care in the reconstruct
#if 1
    if( aniso.anisoP ) {                     // Anisotropic case
      Real gamma_f = aniso.xi_0 / aniso.nu; 
      q_gauge_param.anisotropy = toDouble(gamma_f);
    }
    else {
      q_gauge_param.anisotropy = 1.0;
    }
#endif

    // Convention: BC has to be applied already
    // This flag just tells QUDA that this is so,
    // so that QUDA can take care in the reconstruct
    if( invParam.AntiPeriodicT ) { 
      q_gauge_param.t_boundary = QUDA_ANTI_PERIODIC_T;
    }
    else { 
      q_gauge_param.t_boundary = QUDA_PERIODIC_T;
    }


    q_gauge_param.gauge_order = QUDA_QDP_GAUGE_ORDER; // gauge[mu], p, col col
    q_gauge_param.cpu_prec = QUDA_SINGLE_PRECISION;  // Single Prec G-field
    q_gauge_param.cuda_prec = QUDA_SINGLE_PRECISION; 
    q_gauge_param.reconstruct = QUDA_RECONSTRUCT_12;
    q_gauge_param.cuda_prec_sloppy = QUDA_SINGLE_PRECISION; // No Sloppy
    q_gauge_param.reconstruct_sloppy = QUDA_RECONSTRUCT_12; // No Sloppy

    // Do I want to Gauge Fix? -- Not yet
    q_gauge_param.gauge_fix = QUDA_GAUGE_FIXED_NO;  // No Gfix yet

    q_gauge_param.blockDim = 64;         // I copy these from invert test
    q_gauge_param.blockDim_sloppy = 64;
    
    // OK! This is ugly: gauge_param is an 'extern' in dslash_quda.h
    gauge_param = &q_gauge_param;

    // Definitely no clover here...

    inv_param.dslash_type = QUDA_CLOVER_WILSON_DSLASH; // Sets Clover Matrix
    inv_param.inv_type = QUDA_CG_INVERTER;      // CG Inverter

    // Fiendish idea from Ron. Set the kappa=1/2 and use 
    // unmodified clover term, and ask for Kappa normalization
    // This should give us A - (1/2)D as the unpreconditioned operator
    // and probabl 1 - {1/4} A^{-1} D A^{-1} D as the preconditioned
    // op. Apart from the A_oo stuff on the antisymmetric we have
    // nothing to do...
    inv_param.kappa = 0.5;

    inv_param.tol = toDouble(invParam.RsdTarget);
    inv_param.maxiter = invParam.MaxIter;
    inv_param.reliable_delta = toDouble(invParam.Delta);

    //  (1-k^2 Doe Deo)
    inv_param.matpc_type = QUDA_MATPC_ODD_ODD; 

    // Solve the preconditioned matrix (rather than the prop
    inv_param.solution_type = QUDA_MATPC_SOLUTION;
    inv_param.mass_normalization = QUDA_KAPPA_NORMALIZATION;

    inv_param.cpu_prec = QUDA_SINGLE_PRECISION;
    inv_param.cuda_prec = QUDA_SINGLE_PRECISION;
    inv_param.cuda_prec_sloppy = QUDA_SINGLE_PRECISION;
    inv_param.preserve_source = QUDA_PRESERVE_SOURCE_YES;

    // Even-odd colour inside spin
    inv_param.dirac_order = QUDA_DIRAC_ORDER;
    inv_param.verbosity = QUDA_SUMMARIZE;

    // Clover configuration
    inv_param.clover_cpu_prec = QUDA_SINGLE_PRECISION;
    inv_param.clover_cuda_prec = QUDA_SINGLE_PRECISION;
    inv_param.clover_cuda_prec_sloppy = QUDA_SINGLE_PRECISION;
    inv_param.clover_order = QUDA_PACKED_CLOVER_ORDER;

    // Set up the links
    void* gauge[4];
    for(int mu=0; mu < Nd; mu++) { 
      gauge[mu] = (void *)&(links[mu].elem(all.start()).elem().elem(0,0).real());
    }

    // Set up Clover Inverse...
    multi1d<QUDAPackedClovSite<REAL> > packed_invclov(all.siteTable().size());
    invclov.packForQUDA(packed_invclov, 0);
    invclov.packForQUDA(packed_invclov, 1);
    loadCloverQuda(NULL, &(packed_invclov[0]), &inv_param);

    // Solving  A_oo ( 1 - A^{-1}_oo D A^{-1}_ee D ) psi = chi
    // so            ( 1 - A^{-1}_oo D A^{-1}_ee D ) psi = A^{-1}_oo chi
    // So set up A^{-1}_oo chi

    TF mod_chi;
    invclov.apply(mod_chi, chi_s, PLUS, 1);

    StopWatch swatch1; 
    swatch1.reset();
    swatch1.start();
    loadGaugeQuda((void *)gauge, &q_gauge_param);
 
// DEAD Test Code
#if 1
   void* spinorIn =(void *)&(mod_chi.elem(rb[1].start()).elem(0).elem(0).real());
    void* spinorOut =(void *)&(psi_s.elem(rb[0].start()).elem(0).elem(0).real());

    // OK Here I have a chance to test directly 
    // Even Target Checkerboard, No Dagger
    psi_s = zero;
    dslashQuda(spinorOut, spinorIn, &inv_param, 0, 0);

    // Need to create a simple ferm state from the links...
    Handle< FermState<TF, QF, QF> > pstate(new PeriodicFermState<TF,QF,QF>(links));
    QDPWilsonDslashT<TF,QF,QF>  qdp_dslash(pstate, aniso);
    

    TF tmp, psi2;
    psi2 = zero;
    qdp_dslash.apply(tmp, mod_chi, PLUS, 0);
    invclov.apply(psi2, tmp, PLUS, 0);

    TF r=zero;
    r = psi2 - psi_s;
    
    QDPIO::cout << "CB=0" << endl;
    QDPIO::cout << "Dslash Test: || r || = " << sqrt(norm2(r,rb[0])) << endl;
    //    QDPIO::cout << "Dslash Test: || r ||/|| psi || = " << sqrt(norm2(r,rb[0])/norm2(psi_s, rb[0])) << endl;

    QDPIO::cout << "CB=1: Should be zero" << endl;
    QDPIO::cout << "Dslash Test: || r || = " << sqrt(norm2(r,rb[1])) << endl;
    //QDPIO::cout << "Dslash Test: || r ||/|| psi || = " << sqrt(norm2(r,rb[1])/norm2(psi_s, rb[1])) << endl;
    
    const int* tab = rb[0].siteTable().slice();
    for(int i=0; i < rb[0].numSiteTable(); i++) { 
      int j = tab[i];
      bool printSite=false;

      for(int spin=0; spin < 4; spin++) {
	for(int col=0; col < 3; col++) { 
	  if( (fabs(r.elem(j).elem(spin).elem(col).real()) > 1.0e-5 )
	      || (fabs(r.elem(j).elem(spin).elem(col).imag()) > 1.0e-5 )) {
	    printSite=true;
	  }
	}
      }
      if( printSite ) { 
	  
	for(int spin=0; spin < 4; spin++) { 
	  for(int col=0; col < 3; col++) { 
	    QDPIO::cout << "Site= " << j << " Spin= "<< spin << " Col= " << col << " spinor = ( " 
			<< r.elem(j).elem(spin).elem(col).real()  << " , " 
			<< r.elem(j).elem(spin).elem(col).imag()  << " )" << endl;
	  }
	}
	QDPIO::cout << endl;
      }
    }
    QDP_abort(1);
#else

   void* spinorIn =(void *)&(mod_chi.elem(rb[1].start()).elem(0).elem(0).real());
    void* spinorOut =(void *)&(psi_s.elem(rb[1].start()).elem(0).elem(0).real());

    invertQuda(spinorOut, spinorIn, &inv_param);
#endif
    // Take care of mass normalization
    //psi_s *= (invMassParam);
    swatch1.stop();


    QDPIO::cout << "Cuda Space Required" << endl;
    QDPIO::cout << "\t Spinor:" << inv_param.spinorGiB << " GiB" << endl;
    QDPIO::cout << "\t Gauge :" << q_gauge_param.gaugeGiB << " GiB" << endl;
    QDPIO::cout << "\t InvClover :" << inv_param.cloverGiB << " GiB" << endl;
    QDPIO::cout << "QUDA_CG_CLOVER_SOLVER: time="<< inv_param.secs <<" s" ;
    QDPIO::cout << "\tPerformance="<<  inv_param.gflops/inv_param.secs<<" GFLOPS" ; 
    QDPIO::cout << "\tTotal Time (incl. load gauge)=" << swatch1.getTimeInSeconds() <<" s"<<endl;

    ret.n_count =inv_param.iter;

    return ret;

  }
  

}


