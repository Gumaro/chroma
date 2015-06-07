/*! \file
 *  \brief Make contact with the QDP clover multigrid solver, transfer
 *         the gauge field, generate the coarse grids, solve systems
 */
#include "state.h"
#include "meas/inline/io/named_objmap.h"
#include "actions/ferm/invert/syssolver_mdagm_factory.h"
#include "actions/ferm/invert/syssolver_mdagm_aggregate.h"
#include <cstdio>
#include <ostream>

using namespace std;

#if BASE_PRECISION == 32
#define QDP_Precision 'F'
#define QLA_Precision 'F'
#define toReal toFloat
#elif BASE_PRECISION == 64
#define QDP_Precision 'D'
#define QLA_Precision 'D'
#define toReal toDouble
#endif

#include "actions/ferm/invert/qop_mg/syssolver_linop_qop_mg_w.h"
#include "actions/ferm/invert/qop_mg/syssolver_mdagm_qop_mg_w.h"


extern "C" {
  // This should be placed on the include path.
#include "wilsonmg-interface.h"
}

#include "meas/glue/mesplq.h"

namespace Chroma
{


  // This will come from syssolver_linop_qop_mg_w.h
  //  static multi1d<LatticeColorMatrix> u;
  // These functions will allow QDP to look into the Chroma gauge field and set
  // the QDP gauge field at each site equal to the one in Chroma. There doesn't
  // seem to be a good way to treat the extra std::vector index of the gauge field,

  template<typename T> // T is the Lattice Fermion type
  MdagMSysSolverQOPMG<T>::
    MdagMSysSolverQOPMG(Handle< LinearOperator<T> > A_,
			Handle< FermState<T,Q,Q> > state_, 
                        const SysSolverQOPMGParams& invParam_) : 
  A(A_), state(state_), invParam(invParam_)
  {
    if (invParam.Levels>0 && PC(g_param).levels>0) MGP(finalize)();
  // Copy the parameters read from XML into the QDP global structure
    for (int d=0; d<4; d++) PC(g_param).bc[d]  = 1;
    PC(g_param).aniso_xi = toReal(invParam.AnisoXi);
    PC(g_param).aniso_nu = toReal(invParam.AnisoNu);
    PC(g_param).kappa    = toReal(invParam.Kappa);
    PC(g_param).kappac   = toReal(invParam.KappaCrit);
    PC(g_param).mass     = toReal(invParam.Mass);
    PC(g_param).massc    = toReal(invParam.MassCrit);
    PC(g_param).clov_s   = toReal(invParam.Clover);
    PC(g_param).clov_t   = toReal(invParam.CloverT);
    PC(g_param).res      = toReal(invParam.Residual);
    PC(g_param).ngcr     = invParam.NumGCRVecs;
    PC(g_param).maxiter  = invParam.MaxIter;
    PC(g_param).verb     = invParam.Verbose;
    PC(g_param).levels   = invParam.Levels;

    for (int n=0; n<invParam.Levels; n++) {
      for (int d=0; d<4; d++)
        PC(g_param).block[n][d] = invParam.Blocking[n][d];
      PC(g_param).nNullVecs[n]   = invParam.NumNullVecs[n];
      PC(g_param).nullMaxIter[n] = invParam.NullMaxIter[n];
      PC(g_param).nullRes[n]  = toReal(invParam.NullResidual[n]);
      PC(g_param).nullConv[n] = toReal(invParam.NullConvergence[n]);
      PC(g_param).nExtraVecs[n]  = invParam.NumExtraVecs[n];
      PC(g_param).urelax[n]   = toReal(invParam.Underrelax[n]);
      PC(g_param).npre[n]        = invParam.NumPreHits[n];
      PC(g_param).npost[n]       = invParam.NumPostHits[n];
      PC(g_param).cngcr[n]       = invParam.CoarseNumGCRVecs[n];
      PC(g_param).cmaxiter[n]    = invParam.CoarseMaxIter[n];
      PC(g_param).cres[n]     = toReal(invParam.CoarseResidual[n]);
    }

// The std::vector of functions will be used by QDP to assign the gauge links
// at each site of the QDP lattice
    if (invParam.Levels>0) {
      /* We're going to pull the gauge field out of Chroma's aether */
#if 0
      u = TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(invParam.GaugeID);
#else
      u = state_->getLinks();
#endif
      // Compute the plaquette for comparison with MG code
      {
      	Double w_plaq, s_plaq, t_plaq, link;

      	MesPlq(u, w_plaq, s_plaq, t_plaq, link);
      	QDPIO::cout << "Plaquette from State: " << std::endl;
      	QDPIO::cout << "  w_plaq = " << w_plaq << std::endl;
      	QDPIO::cout << "  s_plaq = " << s_plaq << std::endl;
      	QDPIO::cout << "  t_plaq = " << t_plaq << std::endl;
      	QDPIO::cout << "  link trace =  " << link << std::endl;
      }

      int machsize[4], latsize[4];
      for (int d=0;d<4;d++) machsize[d] = Layout::logicalSize()[d];
      for (int d=0;d<4;d++) latsize[d]  = Layout::lattSize()[d];
      void (*peekpoke[4])(QLA(ColorMatrix) *dest, int coords[]) =
        {peekpoke0,peekpoke1,peekpoke2,peekpoke3};
      MGP(initialize)(machsize, latsize, peekpoke);
      //MGP(teststuff)();
    }
  }
      
  template<typename T> // T is the Lattice Fermion type
  MdagMSysSolverQOPMG<T>::
    ~MdagMSysSolverQOPMG()
  {
    if (invParam.Levels<0) MGP(finalize)();
  }
  
  void *fermionsrc, *fermionsol;
  template<typename T>
  void peekpokesrc(QLA(DiracFermion) *dest, int coords[])
  {
    multi1d<int> x(4); for (int i=0; i<4; i++) x[i] = coords[i];
    Fermion src; src.elem() = ((T*)fermionsrc)->elem(Layout::linearSiteIndex(x));
    /*START_CODE();
    double bsq = norm2(*(T*)fermionsrc).elem().elem().elem().elem();
    printf("Chroma:   in norm2 = %g\n",bsq);
    END_CODE();*/
    //printf("Chroma: x = %i %i %i %i:\n",x[0],x[1],x[2],x[3]);
    QLA(Complex) z;
    QLA(Real) real, imag;
    for (int s=0; s<4; s++)
      for (int c=0; c<3; c++) {
        real = src.elem().elem(s).elem(c).real();
        imag = src.elem().elem(s).elem(c).imag();
        //printf("Chroma:   s=%i,c=%i == %g + I %g\n",s,c,real,imag);
        QLA(C_eq_R_plus_i_R)(&z, &real, &imag);
        QLA(elem_D)(*dest,c,s) = z;
      }
  }
  template<typename T>
  void peekpokesol(QLA(DiracFermion) *src, int coords[])
  {
    multi1d<int> x(4); for (int i=0; i<4; i++) x[i] = coords[i];
    ColorVector ctmp;
    Fermion ftmp;
    /*START_CODE();
    double bsq = norm2(*(T*)fermionsrc).elem().elem().elem().elem();
    printf("Chroma:   in norm2 = %g\n",bsq);
    END_CODE();*/
    //printf("Chroma: x = %i %i %i %i:\n",x[0],x[1],x[2],x[3]);
    QLA(Complex) z;
    QLA(Real) real, imag;
    for (int s=0; s<4; s++) {
      for (int c=0; c<3; c++) {
        z = QLA_elem_D(*src,c,s);
        QLA(R_eq_re_C)(&real, &z);
        QLA(R_eq_im_C)(&imag, &z);
        Complex ztmp = cmplx(Real(real), Real(imag));
        pokeColor(ctmp, ztmp, c);
      }
      pokeSpin(ftmp, ctmp, s);
    }
    pokeSite(*((T*)fermionsol), ftmp, x);
  }

  //! Solve the linear system
  /*!
   * \param psi      solution ( Modify )
   * \param chi      source ( Read )
   * \return syssolver results
   */
  template<typename T> // T is the Lattice Fermion type
  SystemSolverResults_t
    MdagMSysSolverQOPMG<T>::operator() (T& psi, const T& chi) const
  {
    START_CODE();
    
    SystemSolverResults_t res;
    
    StopWatch swatch;
    swatch.reset();
    swatch.start();

    // we will solve for g5 D g5 D psi = chi
    // so psi = D^(-1)g5 D^(-1) g5 chi

    T g5chi = Gamma(Nd*Nd-1)*chi ; //
    T tmpsol  ;

    // Set global pointers to our source and solution fermion fields
    fermionsrc = (void*)&g5chi;
    fermionsol = (void*)&tmpsol;
    double bsq = norm2(chi,all).elem().elem().elem().elem();
    QDPIO::cout << "Chroma:   chi all norm2 = " << bsq << std::endl;
    res.n_count = MGP(solve)(peekpokesrc<T>, peekpokesol<T>);

    tmpsol =  Gamma(Nd*Nd-1)*tmpsol ;
    // Set global pointers to our source and solution fermion fields
    fermionsrc = (void*)&tmpsol;
    fermionsol = (void*)&psi;
    res.n_count += MGP(solve)(peekpokesrc<T>, peekpokesol<T>);

    bsq = norm2(psi,all).elem().elem().elem().elem();
    QDPIO::cout << "Chroma:   psi all norm2 = " << bsq << std::endl;
 
    swatch.stop();
    double time = swatch.getTimeInSeconds();
    { 
      T r;
      r[A->subset()] = chi;
      T tmp;
      T tmp2 ;
      (*A)(tmp2, psi , PLUS );
      (*A)(tmp , tmp2, MINUS);

      r[A->subset()] -= tmp;
      res.resid = sqrt(norm2(r, A->subset()));
    }

    QDPIO::cout << "QOPMG_SOLVER: " << res.n_count << " iterations."
                << " Rsd = " << res.resid
                << " Relative Rsd = " << res.resid/sqrt(norm2(chi,A->subset()))
                << std::endl;
    QDPIO::cout << "QOPMG_SOLVER_TIME: " << time << " secs" << std::endl;

    END_CODE();
    return res;
  }


  //! QDP multigrid system solver namespace
  namespace MdagMSysSolverQOPMGEnv
  {
    //! Anonymous namespace
    namespace
    {
      //! Name to be used
      const std::string name("QOP_CLOVER_MULTIGRID_INVERTER");

      //! Local registration flag
      bool registered = false;
    }


    //! Callback function for standard precision
    MdagMSystemSolver<LatticeFermion>*
      createFerm( XMLReader& xml_in,
                  const std::string& path,
                  Handle< FermState< LatticeFermion, 
                                     multi1d<LatticeColorMatrix>,
                                     multi1d<LatticeColorMatrix> > > state, 
                  Handle< LinearOperator<LatticeFermion> >           A)
    {
      return new MdagMSysSolverQOPMG<LatticeFermion>
	(A, state, SysSolverQOPMGParams(xml_in, path));
    }

    /*//! Callback function for single precision
    MdagMSystemSolver<LatticeFermionF>*
      createFermF( XMLReader&                                          xml_in,
                   const std::string&                                  path,
                   Handle< FermState< LatticeFermionF, 
                                      multi1d<LatticeColorMatrixF>,
                                      multi1d<LatticeColorMatrixF> > > state,
                   Handle< LinearOperator<LatticeFermionF> >           A)
    {
      return new MdagMSysSolverQOPMG<LatticeFermionF>
                   (A, SysSolverQOPMGParams(xml_in, path));
    }*/

    //! Register all the factories
    bool registerAll() 
    {
      bool success = true; 
      if (! registered)
      {
        success &= Chroma::TheMdagMFermSystemSolverFactory::Instance().registerObject(name, createFerm);
        //success &= Chroma::TheMdagMFFermSystemSolverFactory::Instance().registerObject(name, createFermF);
        registered = true;
      }
      return success;
    }
  }
}
