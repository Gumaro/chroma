// -*- C++ -*-
// $Id: sfpcac_w.cc,v 3.10 2007-08-23 19:02:58 edwards Exp $
/*! \file
 *  \brief Schroedinger functional application of PCAC
 */

#include "meas/schrfun/sfpcac_w.h"
#include "meas/schrfun/sfcurcor_w.h"
#include "meas/sources/walfil_w.h"
#include "util/ferm/transf.h"

#include "actions/ferm/fermbcs/schroedinger_fermbc_w.h"

namespace Chroma 
{
  
  //! Schroedinger functional stuff
  /*!
   * Compute correlation functions between axial current or pseudescalar
   * density and boundary fields using Schroedinger BC.
   *
   * Also computed, on demand, are correlation functions between both
   * boundaries with zero, one (vector current) and two (axial current or
   * pseudoscalar density) insertions in the bulk. These currents are
   * controlled by the ZVfactP and ZAfactP boolean flags.
   *
   * Compute quark propagators by using the qprop SystemSolver.
   * The initial guess for the inverter is zero.
   *
   * The results are written to the xml file.
   *
   * For further details see the comments in the dependent subroutines.
   *
   * \param state         gauge field state ( Read )
   * \param qprop         propagator solver ( Read )
   * \param phases        object holds list of momenta and Fourier phases ( Read )
   * \param ZVfactP       flag for doing Z_V measurements ( Read )
   * \param ZAfactP       flag for doing Z_A measurements ( Read )
   * \param x0            time slices with axial current insertions ( Read ) 
   * \param y0            time slices with axial current insertions ( Read ) 
   * \param xml           xml file object ( Write )
   * \param xml_group     string used for writing xml data ( Read )
   */
  void SFpcac(Handle< SystemSolver<LatticeFermion> > qprop,
	      Handle< FermState<LatticeFermion, multi1d<LatticeColorMatrix>,
	      multi1d<LatticeColorMatrix> > > state,
	      const SftMom& phases,
	      bool ZVfactP, bool ZAfactP, 
	      int x0, int y0,
	      XMLWriter& xml_out,
	      const string& xml_group)
  {
    START_CODE();
  
    if ( Ns != 4 )
    {
      QDPIO::cerr << __func__ << ": only supports 4 spin components" << endl;
      QDP_abort(1);
    }
  
    // Need to downcast to the appropriate BC
    const SchrFermBC& fermbc = dynamic_cast<const SchrFermBC&>(state->getBC());

    // Outside group
    push(xml_out, xml_group);

    // Length of lattice in decay direction
    int length = phases.numSubsets();
    int j_decay = phases.getDir();

    // Other useful stuff
    int G5 = Ns*Ns-1;
    int jd = 1 << j_decay;

    multi1d<Real> pseudo_prop(length);
    multi1d<Real> axial_prop(length);
    multi1d<Real> pseudo_prop_b(length);
    multi1d<Real> axial_prop_b(length);

       
    // Sanity checks
    if ( ZAfactP && x0 < y0 )
    {
      QDPIO::cerr << "sfpcac: Z_A computation requires x0 > y0: x0,y0=" 
		  << x0 << " " << y0 << endl;
      QDP_abort(1);
    }

    // 3-space volume normalization
    Real norm = 1.0 / Real(QDP::Layout::vol());
    norm *= Real(QDP::Layout::lattSize()[j_decay]);

    // Spin projectors
    SpinMatrix P_plus, P_minus;
    {
      SpinMatrix g_one = 1.0;

      P_plus  = 0.5*(g_one + (Gamma(jd) * g_one));
      P_minus = 0.5*(g_one - (Gamma(jd) * g_one));
    }

    /* Location of upper wall source */
    int tmin = fermbc.getDecayMin();
    int tmax = fermbc.getDecayMax();

    // Grab the links from the state
    const multi1d<LatticeColorMatrix>& u = state->getLinks();

    // Total number of inversions
    int ncg_had = 0;

    LatticePropagator quark_prop_f = zero;
    LatticePropagator quark_prop_b = zero;

    push(xml_out, "PCAC_measurements");
    for(int direction = -1; direction <= 1; direction+=2)
    {
      int t0 = (direction == -1) ? tmax : tmin;

      for(int color_source = 0; color_source < Nc; ++color_source)
      {
	for(int spin_source = 0; spin_source < Ns; ++spin_source)
	{
	  /* Compute quark propagator "psi" using source "chi" with type specified */
	  /* by WALL_SOURCE, and color and spin equal to color_source and spin_source. */
	  LatticeFermion psi, chi;
	  {
	    LatticeFermion tmp1;
	    walfil(tmp1, t0, j_decay, color_source, spin_source);

	    if (direction == -1)
	    {
	      chi = P_minus * (u[j_decay] * tmp1);
	    }
	    else
	    {
	      chi = shift(P_plus * (adj(u[j_decay]) * tmp1), BACKWARD, j_decay);
	    }
  	  
	    // Solve for the propagator
	    psi = zero;
	    SystemSolverResults_t res = (*qprop)(psi, chi);
	    ncg_had += res.n_count;
	  }


	  /* Store in quark_prop_f/b */
	  if (direction == -1)
	  {
	    FermToProp(psi, quark_prop_b, color_source, spin_source);
	  }
	  else
	  {
	    FermToProp(psi, quark_prop_f, color_source, spin_source);
	  }

	}
      }

      /* Time reverse backwards source */
      if (direction == -1)
      {
	// Construct the pion axial current divergence and the pion correlator
	SFcurcor(pseudo_prop_b, axial_prop_b, quark_prop_b, phases);

	// Normalize to compare to everybody else
	pseudo_prop_b *= norm;
	axial_prop_b  *= norm;

	// Time reverse
	for(int t = 0; t < (length-1)/2 + 1; ++t)
	{
	  int t_eff = length - t - 1;

	  pseudo_prop[t]     = pseudo_prop_b[t_eff];
	  pseudo_prop[t_eff] = pseudo_prop_b[t];

	  axial_prop[t]      = -axial_prop_b[t_eff];
	  axial_prop[t_eff]  = -axial_prop_b[t];
	}
      }
      else
      {
	// Construct the pion axial current divergence and the pion correlator
	SFcurcor(pseudo_prop, axial_prop, quark_prop_f, phases);

	// Normalize to compare to everybody else
	pseudo_prop *= norm;
	axial_prop  *= norm;
      }

      // Write out results
      push(xml_out, "elem");
      write(xml_out, "direction", direction);
      write(xml_out, "pseudo_prop", pseudo_prop);
      write(xml_out, "axial_prop", axial_prop);
      pop(xml_out);

    }  // end for direction
    pop(xml_out);  // PCAC_measurements


    Real f_1 = zero;

    if (ZVfactP || ZAfactP)
    {
      // Sum the forward propagator over time slices to get Kprop
      Propagator kprop = sum(P_plus * (adj(u[j_decay]) * quark_prop_f), phases.getSet()[tmax]);
      kprop *= Real(2)*norm;
      
      /* Construct f_1 */
      f_1 = 0.5 * norm2(kprop);
      
      // Construct H'' = H' gamma_5 K, where H' is the propagator
      // from the upper boundary. 
      // H' is no longer needed: quark_prop_b can be overwritten.
      LatticePropagator tmp_prop = quark_prop_b * Gamma(G5) * kprop;
      quark_prop_b = tmp_prop;
    }


    if (ZVfactP)
    {
      // Construct f_V
      int n = G5 ^ jd;
      LatticeReal r_tmp1 = real(trace(adj(quark_prop_b) * (Gamma(n) * quark_prop_f)));
      multi1d<Double> hrsum = sumMulti(r_tmp1, phases.getSet());

      multi1d<Real> vector_corr(length);
      for(int t = 0; t < length; t++)
      {
	vector_corr[t] = norm * real(hrsum[t]);
      }

      push(xml_out, "ZV_measurements");
      write(xml_out, "f_1", f_1);
      write(xml_out, "vector_corr", vector_corr);
      pop(xml_out);
    }

    if (ZAfactP)
    {
      /* Construct the ingredients for f^I_AA */
      LatticeReal faa_tmp = zero;
      LatticeReal fap_tmp = zero;
      LatticeReal fpp_tmp = zero;
      LatticeInteger t_coord = QDP::Layout::latticeCoordinate(j_decay);

      /* "right" A_0 insertion at x */
      LatticeBoolean tmask = (t_coord == x0);
      LatticePropagator quark_prop_s = zero;    // sequential propagator

      for(int color_source = 0; color_source < Nc; ++color_source)
      {
	for(int spin_source = 0; spin_source < Ns; ++spin_source)
	{
	  LatticeFermion chi;
	  PropToFerm(quark_prop_f, chi, color_source, spin_source);

	  int n = jd ^ G5;
	  LatticeFermion psi = Gamma(n) * chi;
	  /* This gives multiplication with gamma_5 * gamma_0. */
	  /* Include the minus sign for multiplication with */
	  /* gamma_0 * gamma_5 below. */

	  chi = where(tmask, LatticeFermion(-psi), LatticeFermion(zero));
	  psi = zero;
	  SystemSolverResults_t res = (*qprop)(psi, chi);
	  ncg_had += res.n_count;

	  FermToProp(psi, quark_prop_s, color_source, spin_source);
	}
      }

      // "left" P insertion at y
      LatticeReal r_tmp1 = -real(trace(adj(quark_prop_b) * quark_prop_s));

      fap_tmp += where(t_coord == (y0+1), r_tmp1, LatticeReal(zero));
      fap_tmp -= where(t_coord == (y0-1), r_tmp1, LatticeReal(zero));

      /* "left" A_0 insertion at y */
      r_tmp1 = real(trace(adj(quark_prop_b) * (Gamma(jd) * quark_prop_s)));
      faa_tmp += where(t_coord == y0, r_tmp1, LatticeReal(zero));

      /* "right" A_0 insertion at y */
      tmask = (t_coord == y0);
      int n = jd ^ G5;
      quark_prop_s = zero;
      for(int color_source = 0; color_source < Nc; ++color_source)
      {
	for(int spin_source = 0; spin_source < Ns; ++spin_source)
	{
	  LatticeFermion psi, chi;
	  PropToFerm(quark_prop_f, chi, color_source, spin_source);
	  psi = Gamma(n) * chi;
	  /* This gives multiplication with gamma_5 * gamma_0. */
	  /* Include the minus sign for multiplication with */
	  /* gamma_0 * gamma_5 below. */

	  chi = where(tmask, LatticeFermion(-psi), LatticeFermion(zero));
	  psi = zero;
	  SystemSolverResults_t res = (*qprop)(psi, chi);
	  ncg_had += res.n_count;

	  FermToProp(psi, quark_prop_s, color_source, spin_source);
	}
      }

      /* "left" P insertion at x */
      r_tmp1 = real(trace(adj(quark_prop_b) * quark_prop_s));
      fap_tmp += where(t_coord == (x0+1), r_tmp1, LatticeReal(zero));
      fap_tmp -= where(t_coord == (x0-1), r_tmp1, LatticeReal(zero));

      /* "left" A_0 insertion at x */
      r_tmp1 = -real(trace(adj(quark_prop_b) * (Gamma(jd) * quark_prop_s)));
      faa_tmp += where(t_coord == x0, r_tmp1, LatticeReal(zero));

      /* "right" P insertion at x */
      quark_prop_s = zero;
      for(int color_source = 0; color_source < Nc; ++color_source)
      {
	for(int spin_source = 0; spin_source < Ns; ++spin_source)
	{
	  LatticeFermion psi, chi;
	  PropToFerm(quark_prop_f, chi, color_source, spin_source);
	  psi = Gamma(G5) * chi;

	  chi  = where(t_coord == (x0+1), psi, LatticeFermion(zero));
	  chi -= where(t_coord == (x0-1), psi, LatticeFermion(zero));

	  psi = zero;
	  SystemSolverResults_t res = (*qprop)(psi, chi);
	  ncg_had += res.n_count;

	  FermToProp(psi, quark_prop_s, color_source, spin_source);
	}
      }

      /* "left" P insertion at y */
      r_tmp1 = -real(trace(adj(quark_prop_b) * quark_prop_s));
      fpp_tmp += where(t_coord == (y0+1), r_tmp1, LatticeReal(zero));
      fpp_tmp -= where(t_coord == (y0-1), r_tmp1, LatticeReal(zero));

      /* "left" A_0 insertion at y */
      r_tmp1 = real(trace(adj(quark_prop_b) * (Gamma(jd) * quark_prop_s)));
      fap_tmp += where(t_coord == y0, r_tmp1, LatticeReal(zero));

      /* "right" P insertion at y */
      quark_prop_s = zero;
      for(int color_source = 0; color_source < Nc; ++color_source)
      {
	for(int spin_source = 0; spin_source < Ns; ++spin_source)
	{
	  LatticeFermion psi, chi;
	  PropToFerm(quark_prop_f, chi, color_source, spin_source);
	  psi = Gamma(G5) * chi;

	  chi  = where(t_coord == (y0+1), psi, LatticeFermion(zero));
	  chi -= where(t_coord == (y0-1), psi, LatticeFermion(zero));
	  psi = zero;
	  SystemSolverResults_t res = (*qprop)(psi, chi);
	  ncg_had += res.n_count;

	  FermToProp(psi, quark_prop_s, color_source, spin_source);
	}
      }

      /* "left" P insertion at x */
      r_tmp1 = real(trace(adj(quark_prop_b) * quark_prop_s));
      fpp_tmp += where(t_coord == (x0+1), r_tmp1, LatticeReal(zero));
      fpp_tmp -= where(t_coord == (x0-1), r_tmp1, LatticeReal(zero));

      /* "left" A_0 insertion at x */
      r_tmp1 = -real(trace(adj(quark_prop_b) * (Gamma(jd) * quark_prop_s)));
      fap_tmp += where(t_coord == x0, r_tmp1, LatticeReal(zero));

      multi1d<Double> hrsum = sumMulti(faa_tmp, phases.getSet());
      Real f_AA = (real(hrsum[x0]) + real(hrsum[y0])) * norm;

      hrsum = sumMulti(fap_tmp, phases.getSet());
      Real f_AP_PA  = real(hrsum[x0]);
      f_AP_PA += real(hrsum[y0]);
      f_AP_PA += real(hrsum[x0+1]);
      f_AP_PA += real(hrsum[x0-1]);
      f_AP_PA += real(hrsum[y0+1]);
      f_AP_PA += real(hrsum[y0-1]);
      f_AP_PA *= 0.5 * norm;

      hrsum = sumMulti(fpp_tmp, phases.getSet());
      Real f_PP  = real(hrsum[x0+1]);
      f_PP += real(hrsum[x0-1]);
      f_PP += real(hrsum[y0+1]);
      f_PP += real(hrsum[y0-1]);
      f_PP *= 0.25 * norm;

      Real fd_AA;
      fd_AA  = axial_prop_b[y0] * axial_prop[x0];
      fd_AA -= axial_prop_b[x0] * axial_prop[y0];

      Real fd_AP_PA;
      fd_AP_PA  = pseudo_prop_b[y0+1] * axial_prop[x0];
      fd_AP_PA -= pseudo_prop_b[y0-1] * axial_prop[x0];
      fd_AP_PA += axial_prop_b[y0] * pseudo_prop[x0+1];
      fd_AP_PA -= axial_prop_b[y0] * pseudo_prop[x0-1];
      fd_AP_PA -= axial_prop_b[x0] * pseudo_prop[y0+1];
      fd_AP_PA += axial_prop_b[x0] * pseudo_prop[y0-1];
      fd_AP_PA -= pseudo_prop_b[x0+1] * axial_prop[y0];
      fd_AP_PA += pseudo_prop_b[x0-1] * axial_prop[y0];
      fd_AP_PA *= 0.5;

      Real fd_PP;
      fd_PP  = pseudo_prop_b[y0+1] * pseudo_prop[x0+1];
      fd_PP -= pseudo_prop_b[y0+1] * pseudo_prop[x0-1];
      fd_PP -= pseudo_prop_b[y0-1] * pseudo_prop[x0+1];
      fd_PP += pseudo_prop_b[y0-1] * pseudo_prop[x0-1];
      fd_PP -= pseudo_prop_b[x0+1] * pseudo_prop[y0+1];
      fd_PP += pseudo_prop_b[x0+1] * pseudo_prop[y0-1];
      fd_PP += pseudo_prop_b[x0-1] * pseudo_prop[y0+1];
      fd_PP -= pseudo_prop_b[x0-1] * pseudo_prop[y0-1];
      fd_PP *= 0.25;

      push(xml_out, "ZA_measurements");
      write(xml_out, "f_1", f_1);
      write(xml_out, "f_AA", f_AA);
      write(xml_out, "f_AP_PA", f_AP_PA);
      write(xml_out, "f_PP", f_PP);
      write(xml_out, "fd_AA", fd_AA);
      write(xml_out, "fd_AP_PA", fd_AP_PA);
      write(xml_out, "fd_PP", fd_PP);
      pop(xml_out);
    }
  
    push(xml_out,"Relaxation_Iterations");
    write(xml_out, "ncg_had", ncg_had);
    pop(xml_out);

    pop(xml_out);   // xml_group

    END_CODE();
  }

}
