/*! \file
 *  \brief Wall-sink nucleon form-factors 
 *
 *  Form factors constructed from a quark and a backward quark propagator
 */
#include "qdp_config.h"
#if QDP_NS == 4
#if QDP_ND == 4
#if QDP_NC == 3


#include "chromabase.h"
#include "meas/hadron/wallnuclff_w.h"

namespace Chroma {


//! Compute dbar-d current insertion in nucleon
/*!
 * \ingroup hadron
 *
 * quark contraction within a baryon
 *
 * \param q1        first quark ( Read )
 * \param q2        second quark ( Read )
 * \param q3        third quark ( Read )
 *
 * \return color-contracted spin object
 */
template<class T1, class T2, class T3>
static
LatticeSpinMatrix baryonContract(const T1& q1,
				 const T2& q2, 
				 const T3& q3)
{
  LatticeSpinMatrix  S; 

  S = traceColor(q1 * traceSpin(quarkContract13(q3*Gamma(5), Gamma(5)*q2)))
    + traceColor(q1 * quarkContract13(q3*Gamma(5), Gamma(5)*q2));

  return S;
}


//! Wall-sink nucleon-> gamma+nucleon form-factors
/*!
 * \ingroup hadron
 *
 * This routine is specific to Wilson fermions!
 *
 * \param form               Mega-structure holding form-factors ( Write )
 * \param u                  gauge fields (used for non-local currents) ( Read )
 * \param forw_u_prop        forward U quark propagator ( Read )
 * \param back_u_prop        backward D quark propagator ( Read )
 * \param forw_d_prop        forward U quark propagator ( Read )
 * \param back_d_prop        backward D quark propagator ( Read )
 * \param u_x2               forward U quark propagator evaluated at sink  ( Read )
 * \param d_x2               forward D quark propagator evaluated at sink  ( Read )
 * \param phases             fourier transform phase factors ( Read )
 * \param t0                 time slice of the source ( Read )
 * \param wall_source        true if using a wall source ( Read )
 */

void wallNuclFormFac(WallFormFac_formfacs_t& form,
		     const multi1d<LatticeColorMatrix>& u, 
		     const LatticePropagator& forw_u_prop,
		     const LatticePropagator& back_u_prop, 
		     const LatticePropagator& forw_d_prop,
		     const LatticePropagator& back_d_prop, 
		     const Propagator& u_x2,
		     const Propagator& d_x2,
		     const SftMom& phases,
		     int t0,
		     bool wall_source)
{
  START_CODE();

  if ( Ns != 4 || Nc != 3 || Nd != 4 )	// Code is specific to Ns=4, Nc=3, Nd=4
    return;

  form.subroutine = "wallNuclFormFac";

  // Length of lattice in decay direction and 3pt correlations fcns
  int length = phases.numSubsets();

  int G5 = Ns*Ns-1;
  
  // The list of meaningful insertions
  //   The gamma matrices specified correspond to 
  // V_mu and A_mu = gamma_mu gamma_5, specificially
  // where in the Chroma code gamma_5 = g_3 g_2 g_1 g_0
  //
  //               GAMMA              CURRENT
  //                 1                  V_0
  //                 2                  V_1
  //                 4                  V_2
  //                 8                  V_3
  //                 14                -A_0
  //                 13                 A_1
  //                 11                -A_2
  //                 7                  A_3
  multi1d<int> gamma_list(2*Nd);
  gamma_list[0] = 1;
  gamma_list[1] = 2;
  gamma_list[2] = 4;
  gamma_list[3] = 8;
  gamma_list[4] = 14;
  gamma_list[5] = 13;
  gamma_list[6] = 11;
  gamma_list[7] = 7;

  // Spin projectors
  multi1d<SpinMatrix> S_proj(Nd);
  SpinMatrix  g_one = 1.0;

  // T = (1 + gamma_4) / 2 = (1 + Gamma(8)) / 2
  S_proj[0] = 0.5 * (g_one + (g_one * Gamma(8)));

  // T = \Sigma_k (1 + gamma_4) / 2 = -i gamma_i gamma_j S_proj[0] i,j cyclic perms
  S_proj[1] = timesMinusI(Gamma(1 << 1) * (Gamma(1 << 2) * S_proj[0]));
  S_proj[2] = timesMinusI(Gamma(1 << 2) * (Gamma(1 << 0) * S_proj[0]));
  S_proj[3] = timesMinusI(Gamma(1 << 0) * (Gamma(1 << 1) * S_proj[0]));


  // Quark names
  multi1d<std::string> quark_name(2);
  quark_name[0] = "u";
  quark_name[1] = "d";

  // Formfac names
  multi1d<std::string> formfac_name(1);
  formfac_name[0] = "nucleon->gamma+nucleon";

  // Projector names
  multi1d<std::string> proj_name(Nd);
  proj_name[0] = "I";
  proj_name[1] = "sigma_1";
  proj_name[2] = "sigma_2";
  proj_name[3] = "sigma_3";


  // Antiquarks
  LatticePropagator anti_u_prop = adj(Gamma(G5)*back_u_prop*Gamma(G5));
  LatticePropagator anti_d_prop = adj(Gamma(G5)*back_d_prop*Gamma(G5));

  // Resize some things - this is needed upfront because I traverse the 
  // structure in a non-recursive scheme
  form.quark.resize(quark_name.size());
  for (int ud=0; ud < form.quark.size(); ++ud) 
  {
    form.quark[ud].formfac.resize(formfac_name.size());
    for(int dp = 0; dp < form.quark[ud].formfac.size(); ++dp)
    {
      form.quark[ud].formfac[dp].lorentz.resize(1);
      for(int lorz = 0; lorz < form.quark[ud].formfac[dp].lorentz.size(); ++lorz)
      {
	form.quark[ud].formfac[dp].lorentz[lorz].projector.resize(proj_name.size());
	for (int proj = 0; proj < form.quark[ud].formfac[dp].lorentz[lorz].projector.size(); ++proj) 
	{
	  form.quark[ud].formfac[dp].lorentz[lorz].projector[proj].insertion.resize(gamma_list.size());
	}
      }
    }
  }


  // For calculational purpose, loop over insertions first.
  // This is out-of-order from storage within the data structure
  // Loop over gamma matrices of the insertion current of insertion current
  for(int gamma_ctr = 0; gamma_ctr < gamma_list.size(); ++gamma_ctr)
  {
    int gamma_value = gamma_list[gamma_ctr];
    int mu = gamma_ctr % Nd;
    bool compute_nonlocal = (gamma_ctr < Nd) ? true : false;

    // Loop over "u"=0 or "d"=1 pieces
    for(int ud = 0; ud < form.quark.size(); ++ud)
    {
      WallFormFac_quark_t& quark = form.quark[ud];
      quark.quark_ctr = ud;
      quark.quark_name = quark_name[ud];

      LatticePropagator local_insert_prop, nonlocal_insert_prop;


      switch (ud)
      {
      case 0:
      {
	// "\bar u O u" insertion in rho
	// The local non-conserved current contraction
	local_insert_prop = anti_u_prop*Gamma(gamma_value)*forw_u_prop;

	if (compute_nonlocal)
	{
	  // Construct the non-local (possibly conserved) current contraction
	  nonlocal_insert_prop = nonlocalCurrentProp(u, mu, forw_u_prop, anti_u_prop);
	}
      }
      break;

      case 1:
      {
	// "\bar d O d" insertion in rho
	// The local non-conserved current contraction
	local_insert_prop = anti_d_prop*Gamma(gamma_value)*forw_d_prop;

	if (compute_nonlocal)
	{
	  // Construct the non-local (possibly conserved) current contraction
	  nonlocal_insert_prop = nonlocalCurrentProp(u, mu, forw_d_prop, anti_d_prop);
	}
      }
      break;

      default:
	QDP_error_exit("Unknown ud type", ud);
      }


      // Loop over "rho->rho"=0 types of form-factors
      for(int dp = 0; dp < quark.formfac.size(); ++dp)
      {
	WallFormFac_formfac_t& formfac = quark.formfac[dp];
	formfac.formfac_ctr  = dp;
	formfac.formfac_name = formfac_name[dp];

	LatticeSpinMatrix local_contract, nonlocal_contract;


	// Loop over Lorentz indices of source and sink hadron operators
	for(int lorz = 0; lorz < formfac.lorentz.size(); ++lorz)
	{
	  WallFormFac_lorentz_t& lorentz = formfac.lorentz[lorz];
	  lorentz.lorentz_ctr = lorz;

	  int gamma_value1 = G5;
	  int gamma_value2 = G5;

	  lorentz.snk_gamma = gamma_value1;
	  lorentz.src_gamma = gamma_value2;

	  // Contractions depend on "ud" (u or d quark contribution)
	  // and source/sink operators
	  switch (ud)
	  {
	  case 0:
	  {
	    // "\bar u O u" insertion in rho
	    // The local non-conserved current contraction
	    local_contract = 
	      baryonContract(local_insert_prop, u_x2, d_x2) + 
	      baryonContract(u_x2, local_insert_prop, d_x2);

	    if (compute_nonlocal)
	    {
	      // Construct the non-local (possibly conserved) current contraction
	      nonlocal_contract = 
		baryonContract(nonlocal_insert_prop, u_x2, d_x2) + 
		baryonContract(u_x2, nonlocal_insert_prop, d_x2);
	    }
	  }
	  break;

	  case 1:
	  {
	    // "\bar d O d" insertion in rho
	    // The local non-conserved current contraction
	    local_contract =
	      baryonContract(u_x2, u_x2, local_insert_prop);

	    if (compute_nonlocal)
	    {
	      // Construct the non-local (possibly conserved) current contraction
	      nonlocal_contract =
		baryonContract(u_x2, u_x2, nonlocal_insert_prop);
	    }
	  }
	  break;

	  default:
	    QDP_error_exit("Unknown ud type", ud);
	  }


	  // Loop over the spin projectors
	  for (int proj = 0; proj < lorentz.projector.size(); ++proj) 
	  {
	    WallFormFac_projector_t& projector = lorentz.projector[proj];
	    projector.proj_ctr  = proj;
	    projector.proj_name = proj_name[proj];

	    WallFormFac_insertion_t& insertion = projector.insertion[gamma_ctr];

	    insertion.gamma_ctr   = gamma_ctr;
	    insertion.mu          = mu;
	    insertion.gamma_value = gamma_value;

	    // The local non-conserved std::vector-current matrix element 
	    LatticeComplex corr_local_fn = traceSpin(S_proj[proj] * local_contract);

	    // The nonlocal (possibly conserved) current matrix element 
	    LatticeComplex corr_nonlocal_fn = traceSpin(S_proj[proj] * nonlocal_contract);
	
	    multi1d<WallFormFac_momenta_t>& momenta = insertion.momenta;

	    wallFormFacSft(momenta, corr_local_fn, corr_nonlocal_fn, phases,
			   compute_nonlocal, t0);

	  } // end for(proj)
	}  // end for(dp)
      } // end for(ud)
    } // end for(lorz)
  } // end for(gamma_ctr)

  END_CODE();
}

}  // end namespace Chroma

#endif
#endif
#endif

