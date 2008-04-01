// $Id: syssolver_linop_OPTeigcg.cc,v 1.3 2008-04-01 21:16:18 kostas Exp $
/*! \file
 *  \brief Solve a M*psi=chi linear system by CG2
 */


#include "qdp-lapack_Complex.h"
#include "qdp-lapack_eigpcg.h"  
#include "qdp-lapack_IncrEigpcg.h"


#include "actions/ferm/invert/syssolver_linop_factory.h"
#include "actions/ferm/invert/syssolver_linop_aggregate.h"

#include "actions/ferm/invert/syssolver_linop_OPTeigcg.h"
#include "containers.h"


namespace Chroma
{

  //! CG1 system solver namespace
  namespace LinOpSysSolverOptEigCGEnv
  {
    //! Callback function
    LinOpSystemSolver<LatticeFermion>* createFerm(XMLReader& xml_in,
						  const std::string& path,
						  Handle< LinearOperator<LatticeFermion> > A)
    {
      return new LinOpSysSolverOptEigCG<LatticeFermion>(A, SysSolverOptEigCGParams(xml_in, path));
    }

#if 0
    //! Callback function
    LinOpSystemSolver<LatticeStaggeredFermion>* createStagFerm(
      XMLReader& xml_in,
      const std::string& path,
      Handle< LinearOperator<LatticeStaggeredFermion> > A)
    {
      return new LinOpSysSolverOptEigCG<LatticeStaggeredFermion>(A, SysSolverOptEigCGParams(xml_in, path));
    }
#endif

    //! Name to be used
    const std::string name("EIG_CG_INVERTER");

    //! Local registration flag
    static bool registered = false;

    //! Register all the factories
    bool registerAll() 
    {
      bool success = true; 
      if (! registered)
      {
	success &= Chroma::TheLinOpFermSystemSolverFactory::Instance().registerObject(name, createFerm);
//	success &= Chroma::TheLinOpStagFermSystemSolverFactory::Instance().registerObject(name, createStagFerm);
	registered = true;
      }
      return success;
    }
  }


  //! Anonymous namespace holding method
  namespace
  {
     template<typename T>
     struct MatVecArg{
       T XX ;
       T YY ;
       Handle< LinearOperator<T> > MdagM;
    };

    template<typename T>
    void MatrixMatvec(void *x, void *y, void *params) {
      
      const MatVecArg<T> &arg = *((MatVecArg<T> *) params) ;
      
      //Works only in single precision CHROMA
      RComplex<float> *px = (RComplex<float> *) x;
      RComplex<float> *py = (RComplex<float> *) y;

      //XX.getF() = (Complex *) x ; //AN DOULEPSEI AUTO NA ME FTUSEIS
      //YY.getF() = (Complex *) y ; //AN DOULEPSEI AUTO NA ME FTUSEIS

      //Alliws kanoume copy
      //copy x into XX
      Subset s = arg.MdagM->subset() ;
      if(s.hasOrderedRep()){
	int count=0 ;
	//can be done with ccopy for speed...
	for(int i=s.start(); i <= s.end(); i++)
	  for(int ss(0);ss<Ns;ss++)
	    for(int c(0);c<Nc;c++){
	      arg.XX.elem(i).elem(ss).elem(c) = *(px+count);
	      count++;
	    }
      }
      else{
	int i ;
	const int *tab = s.siteTable().slice();
	int count=0;
	for(int x=0; x < s.numSiteTable(); ++x){
	  i = tab[x] ;
	  for(int ss(0);ss<Ns;ss++)
	    for(int c(0);c<Nc;c++){
	      arg.XX.elem(i).elem(ss).elem(c) = *(px+count);
	      count++;
	    }
	}
      }

      arg.MdagM(arg.YY,arg.XX,PLUS) ;

      //copy back..
      if(s.hasOrderedRep()){
	int count=0 ;
	//can be done with ccopy for speed...
	for(int i=s.start(); i <= s.end(); i++)
	  for(int ss(0);ss<Ns;ss++)
	    for(int c(0);c<Nc;c++){
	      *(py+count) = arg.YY.elem(i).elem(ss).elem(c) ;
	      count++;
	    }
      }
      else{
	int i ;
	int count=0;
	const int *tab = s.siteTable().slice();
	for(int x=0; x < s.numSiteTable(); ++x){
	  i = tab[x] ;
	  for(int ss(0);ss<Ns;ss++)
	    for(int c(0);c<Nc;c++){
	      *(py+count) = arg.YY.elem(i).elem(ss).elem(c) ;
	      count++;
	    }
	}
      }
    }
    

    //! Solver the linear system
    /*!
     * \param psi      solution ( Modify )
     * \param chi      source ( Read )
     * \return syssolver results
     */
    template<typename T>
    SystemSolverResults_t sysSolver(T& psi, const T& chi, 
				    const LinearOperator<T>& A, 
				    Handle< LinearOperator<T> > MdagM, 
				    const SysSolverOptEigCGParams& invParam)
    {
      START_CODE();

      SystemSolverResults_t res;  // initialized by a constructor

      LinAlg::OptEigInfo& EigInfo = TheNamedObjMap::Instance().getData< LinAlg::OptEigInfo >(invParam.eigen_id);

      Subset s = A.subset() ;

      Complex_C *work=NULL  ;
      Complex_C *V=NULL     ;
      Complex_C *ework=NULL ;
      Complex_C *X ;
      Complex_C *B ;
      
      if(s.hasOrderedRep()){
	X = (Complex_C *) &psi.elem(s.start()).elem(0).elem(0).real();
	B = (Complex_C *) &chi.elem(s.start()).elem(0).elem(0).real();
      }
      else{//need to copy
	//X = allocate space for them
	//B =  allocate space for them...
	QDPIO::cout<<"OPPS! I have no implemented OPT_EigCG for Linops with non contigius subset\n";
      }
      Complex_C *evecs = (Complex_C *) &EigInfo.evecs[0] ;
      Complex_C *evals = (Complex_C *) &EigInfo.evals[0] ;
      Complex_C *H  = (Complex_C *) &EigInfo.H[0] ;
      Complex_C *HU = (Complex_C *) &EigInfo.HU[0] ;

      MatVecArg<T> arg ;
      arg.MdagM = MdagM ;
      int esize = invParam.esize*Layout::sitesOnNode()*Nc*Ns ;
      /**
      IncrEigpcg(EigInfo.N, EigInfo.lde, 1, X, B, &EigInfo.ncurEvals, EigInfo.evals.size(), 
		 evecs, evals, H, HU, 
		 MatrixMatvec, NULL, (void *)&arg, work, V, esize, 
		 invParam.esize, invParam.RsdCG, &invParam.restartTol,
		 invParam.NormAest, 
		 invParam.updateRestartTol, invParam.MaxCG, invParam.PrintLevel, 
		 invParam.Neig, invParam.Nmax, stdout);
      **/
      if(!s.hasOrderedRep()){
	QDPIO::cout<<"OPPS! I have no implemented OPT_EigCG for Linops with non contigius subset\n";
      }
      END_CODE();

      return res;
    }

  } // anonymous namespace


  //
  // Wrappers
  //

  // LatticeFermionF
  template<>
  SystemSolverResults_t
  LinOpSysSolverOptEigCG<LatticeFermionF>::operator()(LatticeFermionF& psi, const LatticeFermionF& chi) const
  {
    return sysSolver(psi, chi, *A, MdagM, invParam);
  }

#if 0
  //OPT EigCG does not  work with double prec Lattice Fermions
  // LatticeFermionD
  template<>
  SystemSolverResults_t
  LinOpSysSolverOptEigCG<LatticeFermionD>::operator()(LatticeFermionD& psi, const LatticeFermionD& chi) const
  {
    return sysSolver(psi, chi, *A, MdagM, invParam);
  }


  // Not quite ready yet for these - almost there
  // LatticeStaggeredFermionF
  template<>
  SystemSolverResults_t
  LinOpSysSolverOptEigCG<LatticeStaggeredFermionF>::operator()(LatticeStaggeredFermionF& psi, const LatticeStaggeredFermionF& chi) const
  {
    return sysSolver(psi, chi, *A, MdagM, invParam);
  }

  // LatticeStaggeredFermionD
  template<>
  SystemSolverResults_t
  LinOpSysSolverOptEigCG<LatticeStaggeredFermionD>::operator()(LatticeStaggeredFermionD& psi, const LatticeStaggeredFermionD& chi) const
  {
    return sysSolver(psi, chi, *A, MdagM, invParam);
  }
#endif

}
