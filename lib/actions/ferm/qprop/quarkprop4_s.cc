// $Id: quarkprop4_s.cc,v 2.1 2005-12-15 04:03:27 edwards Exp $
/*! \file
 *  \brief Full quark propagator solver
 *
 *  Given a complete propagator as a source, this does all the inversions needed
 */

#include "chromabase.h"
#include "fermact.h"
#include "util/ferm/transf.h"
#include "actions/ferm/qprop/quarkprop4_s.h"


namespace Chroma 
{
  //! Given a complete propagator as a source, this does all the inversions needed
  /*! \ingroup qprop
   *
   * This routine is actually generic to all Wilson-like fermions
   *
   * \param q_sol    quark propagator ( Write )
   * \param q_src    source ( Read )
   * \param invParam inverter parameters ( Read )
   * \param RsdCG    CG (or MR) residual used here ( Read )
   * \param MaxCG    maximum number of CG iterations ( Read )
   * \param ncg_had  number of CG iterations ( Write )
   */

  template<typename T>
  void quarkProp_a(LatticeStaggeredPropagator& q_sol, 
		   XMLWriter& xml_out,
		   const LatticeStaggeredPropagator& q_src,
		   const FermionAction<T>& S_f,
		   Handle<const ConnectState> state,
		   const InvertParam_t& invParam,
		   QuarkSpinType quarkSpinType,
		   int& ncg_had)
  {
    START_CODE();

    push(xml_out, "QuarkProp4");

    ncg_had = 0;

    Handle<const SystemSolver<T> > qprop(S_f.qprop(state,invParam));

//  LatticeStaggeredFermion psi = zero;  // note this is ``zero'' and not 0

    // This version loops over all color and spin indices
    for(int color_source = 0; color_source < Nc; ++color_source)
    {
      LatticeStaggeredFermion psi = zero;  // note this is ``zero'' and not 0
      LatticeStaggeredFermion chi;

      // Extract a fermion source
      PropToFerm(q_src, chi, color_source);

      // Use the last initial guess as the current initial guess

      /* 
       * Normalize the source in case it is really huge or small - 
       * a trick to avoid overflows or underflows
       */
      Real fact = Real(1) / sqrt(norm2(chi));
      chi *= fact;

      // Compute the propagator for given source color.
//    S_f.qprop(psi, state, chi, invParam, n_count);
      int n_count = (*qprop)(psi, chi);
      ncg_had += n_count;

      push(xml_out,"Qprop");
      write(xml_out, "RsdCG", invParam.RsdCG);
      write(xml_out, "n_count", n_count);
      pop(xml_out);

      // Unnormalize the source following the inverse of the normalization above
      fact = Real(1) / fact;
      psi *= fact;

      /*
       * Move the solution to the appropriate components
       * of quark propagator.
       */
      FermToProp(psi, q_sol, color_source);
    } /* end loop over color_source */

    pop(xml_out);

    END_CODE();
  }


  //! Given a complete propagator as a source, this does all the inversions needed
  /*! \ingroup qprop
   *
   * This routine is actually generic to all Wilson-like fermions
   *
   * \param q_sol    quark propagator ( Write )
   * \param q_src    source ( Read )
   * \param invParam inverter parameters ( Read )
   * \param ncg_had  number of CG iterations ( Write )
   */
  void quarkProp4(LatticeStaggeredPropagator& q_sol, 
		  XMLWriter& xml_out,
		  const LatticeStaggeredPropagator& q_src,	
		  const StaggeredTypeFermAct< LatticeStaggeredFermion, multi1d<LatticeColorMatrix> >& S_f,
		  Handle<const ConnectState> state,
		  const InvertParam_t& invParam,
		  QuarkSpinType quarkSpinType,
		  int& ncg_had)
  {
    quarkProp_a<LatticeStaggeredFermion>(q_sol, xml_out, q_src, S_f, state, invParam, quarkSpinType, ncg_had);
  }



  //! Given a complete propagator as a source, this does all the inversions needed
  /*! \ingroup qprop
   *
   * This routine is actually generic to all Staggered-like fermions
   *
   * \param q_sol    quark propagator ( Write )
   * \param q_src    source ( Read )
   * \param invParam inverter parameters ( Read )
   * \param ncg_had  number of CG iterations ( Write )
   */
  template<>
  void 
  StaggeredTypeFermAct< LatticeStaggeredFermion, multi1d<LatticeColorMatrix> >::quarkProp(
    LatticeStaggeredPropagator& q_sol, 
    XMLWriter& xml_out,
    const LatticeStaggeredPropagator& q_src,
    Handle<const ConnectState> state,
    const InvertParam_t& invParam,
    QuarkSpinType quarkSpinType,
    int& ncg_had)
  {
    quarkProp4(q_sol, xml_out, q_src, *this, state, invParam, quarkSpinType, ncg_had);
  }


}; // namespace Chroma
