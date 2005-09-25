// $Id: unprec_dwftransf_linop_w.cc,v 2.0 2005-09-25 21:04:30 edwards Exp $
/*! \file
 *  \brief Unpreconditioned Wilson linear operator
 */

#include "chromabase.h"
#include "actions/ferm/linop/unprec_wilson_linop_w.h"
#include "actions/ferm/linop/unprec_dwftransf_linop_w.h"
#include "actions/ferm/invert/invcg2.h"

using namespace QDP::Hints;

namespace Chroma 
{ 
  void UnprecDWFTransfLinOp::create(const multi1d<LatticeColorMatrix>& u_, 
				    const Real& Mass_,
				    const Real& b5_,
				    const Real& c5_,
				    const InvertParam_t& invParam_)
  {
    Mass = Mass_;
    b5 = b5_;
    c5 = c5_;
    invParam = invParam_;

    // Need to create a handle for a wilson linop
    QDPIO::cout << "Creating UnprecDWFTransfLinOp with ";
    QDPIO::cout << " b5=" << b5 << " c5=" << c5 << " Mass=" << Mass;
    QDPIO::cout << " RsdCG=" << invParam.RsdCG << endl;


    Real b5_minus_c5 = b5 - c5;
    D_w = new UnprecWilsonLinOp(u_, Mass_);
    D_denum = new UnprecDWFTransfDenLinOp(u_, b5_minus_c5, D_w);
  }



  void UnprecDWFTransfLinOp::operator() (LatticeFermion& chi, const LatticeFermion& psi, 
					 enum PlusMinus isign) const
  {
    START_CODE();

    int n_count;

    // OK Copy from SZIN Code. DOn't DO hermitian op... just do the D's
    switch(isign) 
    {
    case PLUS:
    {
      // Apply chi = (b5+c5)* gamma_5 * D_w * [ D_denum ]^{-1} * psi
      (*D_denum)(chi, psi, MINUS);
      LatticeFermion tmp;  moveToFastMemoryHint(tmp);

      tmp = psi; 
      
      InvCG2<LatticeFermion>(*D_denum, 
			     chi, 
			     tmp,
			     invParam.RsdCG,
			     invParam.MaxCG,
			     n_count);
      (*D_w)(chi, tmp, PLUS);
   
      break;
    }
    case MINUS:
    {
      // Apply  chi = (b5+c5) * gamma_5 * D_w * [D_denum]^(-1) * gamma_5 * psi
      LatticeFermion tmp;      moveToFastMemoryHint(tmp);
      chi = GammaConst<Ns,Ns*Ns-1>()*psi;      
      (*D_denum)(tmp, chi, MINUS);

      InvCG2<LatticeFermion>(*D_denum, 
			     tmp, 
			     chi,
			     invParam.RsdCG,
			     invParam.MaxCG,
			     n_count);
      
      (*D_w)(tmp, chi, PLUS);
      chi  = GammaConst<Ns,Ns*Ns-1>()*tmp;
      break;
    }
    default:
    {
      QDPIO::cerr << "Bad option " << isign << endl;
      QDP_abort(1);
      break;
    }
    }
    
    chi *= (b5 + c5);
    QDPIO::cout << "UnprecDWFTransfLinOp: ncount= " << n_count << endl;
    END_CODE();
  }



  //------------------------------------------------------------------------

  void UnprecDWFTransfMdagMLinOp::create(const multi1d<LatticeColorMatrix>& u_, 
					 const Real& Mass_,
					 const Real& b5_,
					 const Real& c5_,
					 const InvertParam_t& invParam_)
  {
    Mass = Mass_;
    b5 = b5_;
    c5 = c5_;
    invParam = invParam_;

    // Need to create a handle for a wilson linop
    QDPIO::cout << "Creating UnprecDWFTransfMdagMLinOp with ";
    QDPIO::cout << " b5=" << b5 << " c5=" << c5 << " Mass=" << Mass;
    QDPIO::cout << " RsdCG=" << invParam.RsdCG << endl;

    Real b5_minus_c5 = b5 - c5;
    D_w = new UnprecWilsonLinOp(u_, Mass_);
    D_denum = new UnprecDWFTransfDenLinOp(u_, b5_minus_c5, D_w);
  }



  void UnprecDWFTransfMdagMLinOp::operator() (LatticeFermion& chi, const LatticeFermion& psi, 
					      enum PlusMinus isign) const
  {
    START_CODE();

    int n_count;

    // Apply chi = (b5+c5)^2 * gamma_5 * D_w * [ D_denum^dag * D_denum ]^{-1} * gamma_5 * D_w * psi
    LatticeFermion tmp;       moveToFastMemoryHint(tmp);
    (*D_w)(chi, psi, PLUS);
    tmp = GammaConst<Ns,Ns*Ns-1>()*chi;

    chi = tmp;
    InvCG2<LatticeFermion>(*D_denum, 
			   tmp,
			   chi, 
			   invParam.RsdCG,
			   invParam.MaxCG,
			   n_count);

    (*D_w)(tmp, chi, PLUS);
    chi = GammaConst<Ns,Ns*Ns-1>()*tmp;
    chi *= Real((b5 + c5)*(b5 + c5));

    QDPIO::cout << "UnprecDWFTransfMdagMLinOp: ncount= " << n_count << endl;
    END_CODE();
  }

}; // End Namespace Chroma

