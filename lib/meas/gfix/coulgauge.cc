// $Id: coulgauge.cc,v 1.2 2004-01-02 21:55:37 edwards Exp $
/*! \file
 *  \brief Coulomb (and Landau) gauge fixing 
 */

#include "chromabase.h"
#include "meas/gfix/coulgauge.h"
#include "meas/gfix/grelax.h"
#include "util/gauge/reunit.h"

using namespace QDP;

/********************** HACK ******************************/
// Primitive way for now to indicate the time direction
static int tDir() {return Nd-1;}
static Real xi_0() {return 1.0;}
/******************** END HACK ***************************/


//! Coulomb (and Landau) gauge fixing
/*!
 * \ingroup gfix
 *
 * Driver for gauge fixing to Coulomb gauge in slices perpendicular
 * to the direction "j_decay".
 * If j_decay >= Nd: fix to Landau gauge.
 * Note: as written this works only for SU(2) and SU(3)!

 * \param ug       (gauge fixed) gauge field ( Modify )
 * \param n_gf     number of gauge fixing iterations ( Write )
 * \param j_decay  direction perpendicular to slices to be gauge fixed ( Read )
 * \param GFAccu   desired accuracy for gauge fixing ( Read )
 * \param GFMax    maximal number of gauge fixing iterations ( Read )
 * \param OrDo     use overrelaxation or not ( Read )
 * \param OrPara   overrelaxation parameter ( Read )
 */

void coulGauge(multi1d<LatticeColorMatrix>& ug, int& n_gf, 
	       int j_decay, const Real& GFAccu, int GFMax, 
	       bool OrDo, const Real& OrPara)
{
  Double tgfold;
  Double tgfnew;
  Double tgf_t;
  Double tgf_s;
  Double norm;
  int num_sdir;
  bool tdirp;

  START_CODE("coulGauge");

  Real xi_sq = pow(xi_0(),2);
  if( j_decay >= 0 && j_decay < Nd )
  {
    if( tDir() >= 0 && tDir() != j_decay )
    {
      num_sdir = Nd - 2;
      tdirp = true;
      norm = Double(Layout::vol()*Nc) * (Double(num_sdir)+Double(xi_sq));
    }
    else
    {
      num_sdir = Nd - 1;
      tdirp = false;
      norm = Double(Layout::vol()*Nc*num_sdir);
    }
  }
  else
  {
    if( tDir() >= 0 && tDir() < Nd )
    {
      num_sdir = Nd - 1;
      tdirp = true;
      norm = Double(Layout::vol()*Nc) * (Double(num_sdir)+Double(xi_sq));
    }
    else
    {
      num_sdir = Nd;
      tdirp = false;
      norm = Double(Layout::vol()*Nc*num_sdir);
    }
  }

      
  /* Compute initial gauge fixing term: sum(trace(U_spacelike)); */
  tgf_t = 0;
  tgf_s = 0;
  for(int mu=0; mu<Nd; ++mu)
    if( mu != j_decay )
    {
      if( mu != tDir() )
      {
	tgf_s += sum(real(trace(ug[mu])));
      }
      else
	tgf_t += sum(real(trace(ug[mu])));
    }

  if( tdirp )
  {
    tgfold = (xi_sq*tgf_t+tgf_s)/norm;
    tgf_s = tgf_s/(Double(Layout::vol()*Nc*num_sdir));
    tgf_t = tgf_t/(Double(Layout::vol()*Nc));
  }
  else
  {
    tgf_s = tgf_s/(Double(Layout::vol()*Nc*num_sdir));
    tgfold = tgf_s;
  }
  
  n_gf = 0;
  bool wrswitch = true;    /* switch for writing of gauge fixing term */
  Double conver = 1;        /* convergence criterion */
  multi1d<LatticeColorMatrix> u_tmp(Nd);

  /* Gauge fix until converged or too many iterations */
  while( toBool(conver > GFAccu)  &&  n_gf < GFMax )
  {
    n_gf = n_gf + 1;
    if( GFMax - n_gf < 11 ) 
      wrswitch = true;
    
    if (Nc > 1)
    {
      /* Loop over SU(2) subgroup index */
      for(int su2_index=0; su2_index < Nc*(Nc-1)/2; ++su2_index)
      {
	/* Gather the Nd negative links attached to a site: */
	/* u_tmp(x,mu) = U(x-mu,mu) */
	for(int mu=0; mu<Nd; ++mu)
	  u_tmp[mu] = shift(ug[mu], BACKWARD, mu);

	/* Now do a gauge fixing relaxation step */
	grelax(ug, u_tmp, j_decay, su2_index, OrDo, OrPara);
	
      }   /* end su2_index loop */
    }
    else
    {
      /* Gather the Nd negative links attached to a site: */
      /* u_tmp(x,mu) = U(x-mu,mu) */
      for(int mu=0; mu<Nd; ++mu)
	u_tmp[mu] = shift(ug[mu], BACKWARD, mu);

      int su2_index = -1;
      /* Now do a gauge fixing relaxation step */
      grelax(ug, u_tmp, j_decay, su2_index, OrDo, OrPara);
    }

    /* Reunitarize */
    for(int mu=0; mu<Nd; ++mu)
      reunit(ug[mu]);

    /* Compute new gauge fixing term: sum(trace(U_spacelike)): */
    tgf_t = 0;
    tgf_s = 0;
    for(int mu=0; mu<Nd; ++mu)
      if( mu != j_decay )
      {
	if( mu != tDir() )
	{
	  tgf_s += sum(real(trace(ug[mu])));
	}
	else
	  tgf_t += sum(real(trace(ug[mu])));
      }

    if( tdirp )
    {
      tgfnew = (xi_sq*tgf_t+tgf_s)/norm;
      tgf_s = tgf_s/(Double(Layout::vol()*Nc*num_sdir));
      tgf_t = tgf_t/(Double(Layout::vol()*Nc));
    }
    else
    {
      tgf_s = tgf_s/(Double(Layout::vol()*Nc*num_sdir));
      tgfnew = tgf_s;
    }

    if( wrswitch ) 
      QDPIO::cout << "COULGAUGE: iter= " << n_gf 
		  << "  tgfold= " << tgfold 
		  << "  tgfnew= " << tgfnew
		  << "  tgf_s= " << tgf_s 
		  << "  tgf_t= " << tgf_t << endl;

    /* Normalized convergence criterion: */
    conver = fabs((tgfnew - tgfold) / tgfnew);
    tgfold = tgfnew;
  }       /* end while loop */

      
  if( wrswitch )
    QDPIO::cout << "COULGAUGE: end: iter= " << n_gf 
		<< "  tgfold= " << tgfold 
		<< "  tgf_s= " << tgf_s 
		<< "  tgf_t= " << tgf_t << endl;

#if 0
  /*+ debugging */
  XMLBufferWriter xml_out;
  push(xml_out,"Final_trace_max_in_CoulGauge");
  Write(xml_out, j_decay);
  write(xml_out, "t_dir", tDir());
  Write(xml_out, n_gf);
  Write(xml_out, tgfold);
  Write(xml_out, tgf_s);
  Write(xml_out, tgf_t);
  pop(xml_out);
#endif

  END_CODE("coulGauge");
}
