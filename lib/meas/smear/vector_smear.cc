// $Id: vector_smear.cc,v 3.2 2008-11-04 17:26:16 edwards Exp $
/*! \file
 *  \brief vector smearing of color vector
 */

#include "chromabase.h"
#include "meas/smear/vector_smear.h"
#include "util/ft/sftmom.h"

namespace Chroma 
{

  //! Do a vector smearing of a lattice colorvector field
  /*!
   * Arguments:
   *
   *  \param chi      color vector field ( Modify )
   *  \param vecs     vectors for the smearing ( Read )
   *  \param sigma    exponential smearing parameter ( Read )
   *  \param j_decay  direction of decay ( Read )
   */

  void vectorSmear(LatticeColorVector& chi, 
		   const SubsetVectors<LatticeColorVector> & vecs, 
		   const Real& sigma, const int& j_decay)
  {
    LatticeColorVector psi = zero;

    int num_vecs = vecs.size();
    int nt = vecs.getDecayExtent(); 

    //    multi1d<LatticeColorVector> cvec = vecs.getEvectors();

    SftMom phases(0, false, j_decay);

    for (int n = 0 ; n < num_vecs ; ++n)
    {
      EVPair<LatticeColorVector> ev;
      vecs.lookup(n, ev);

      const multi1d<Real>& evals = ev.eigenValue.weights;
      LatticeColorVector cvec = ev.eigenVector;
      LatticeComplex tmp = localInnerProduct( cvec, chi );
		
      multi2d<DComplex> t_sum = phases.sft(tmp);

      for (int t = 0 ; t < nt ; ++t)
      {
	psi[phases.getSet()[t]] += cvec * t_sum[0][t] * 
	  exp( Real(-1.0) * sigma * sigma / Real(4.0) * evals[t]);
      }
    }
	
    chi = psi;
  }


  //! Do a vector smearing of a lattice fermion field
  /*!
   * Arguments:
   *
   *  \param chi      fermion field ( Modify )
   *  \param vecs     vectors for the smearing ( Read )
   *  \param sigma    exponential smearing parameter ( Read )
   *  \param j_decay  direction of decay ( Read )
   */

  void vectorSmear(LatticeFermion& chi, 
		   const SubsetVectors<LatticeColorVector> & vecs, 
		   const Real& sigma, const int& j_decay)
  {
    LatticeFermion psi = zero;

    for (int s = 0 ; s < Ns ; ++s)
    {
      LatticeColorVector chi_s = peekSpin(chi, s);
      vectorSmear(chi_s, vecs, sigma, j_decay);
		
      pokeSpin(psi, chi_s, s);
    }
	
    chi = psi;
  }

}  // end namespace Chroma
