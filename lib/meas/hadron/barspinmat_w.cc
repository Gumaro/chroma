// $Id: barspinmat_w.cc,v 2.2 2006-01-05 16:34:55 kostas Exp $
/*! \file
 *  \brief Baryon spin and projector matrices
 */

#include "chromabase.h"
#include "meas/hadron/barspinmat_w.h"

namespace Chroma 
{

  /*! \ingroup hadron */
  namespace BaryonSpinMats
  {
    //! NR = (1/2)* ( 1 + g_4 )
    SpinMatrix NR()
    {
      SpinMatrix g_one = 1.0;
      return SpinMatrix(0.5 * (g_one + (g_one * Gamma(8))));
    }

    //! NRnegPar = (1/2)* ( 1 - g_4 )
    SpinMatrix NRnegPar()
    {
      SpinMatrix g_one = 1.0;
      return SpinMatrix(0.5 * (g_one - (g_one * Gamma(8))));
    }

    //! C = Gamma(10)
    SpinMatrix C()
    {
      return SpinMatrix(Gamma(10) * SpinMatrix(1.0));
    }

    //! C NR = (1/2)*C * ( 1 + g_4 )
    SpinMatrix CNR()
    {
      SpinMatrix g_one = 1.0;
      return SpinMatrix(Gamma(10) * NR());
    }

    //! C gamma_5 gamma_4 = - Gamma(13)
    SpinMatrix Cg5g4()
    {
      SpinMatrix g_mone = -1.0;
      return SpinMatrix(Gamma(13) * g_mone);
    }

    //! C g_k = C gamma_k
    SpinMatrix Cgk(int k)
    {
      SpinMatrix g_one = 1.0;
      SpinMatrix gt;
 
      // NOTE: not being too careful about minus signs. Should check.
      // However, all current uses have these matrices coming as a square
      switch (k)
      {
      case 1:
	gt = Gamma(11) * g_one;
	break;

      case 2:
	gt = Gamma(8) * g_one;
	break;

      case 3:
	gt = Gamma(14) * g_one;
	break;

      default:
	gt = g_one;  // Make compiler happy
	QDPIO::cerr << __func__ << ": invalid k=" << k << endl;
	QDP_abort(1);
      }
      return gt;
    }

    //! C g4 g_k = C gamma_4 gamma_k
    SpinMatrix Cg4gk(int k)
    {
      SpinMatrix g_one = 1.0;
      SpinMatrix gt;
 
      // NOTE: not being too careful about minus signs. Should check.
      // However, all current uses have these matrices coming as a square
      switch (k)
      {
      case 1:
	gt = Gamma(3) * g_one;
	break;

      case 2:
	gt = Gamma(0) * g_one;
	break;

      case 3:
	gt = Gamma(6) * g_one;
	break;

      default:
	gt = g_one;  // Make compiler happy
	QDPIO::cerr << __func__ << ": invalid k=" << k << endl;
	QDP_abort(1);
      }
      return gt;
    }

    //! C g_k NR = C gamma_k (1/2)(1 + gamma_4)
    SpinMatrix CgkNR(int k)
    {
      return SpinMatrix(Cgk(k) * NR());
    }


    //! C g_5 = C gamma_5 = Gamma(5)
    SpinMatrix Cg5()
    {
      return SpinMatrix(Gamma(5) * SpinMatrix(1.0));
    }


    //! C g_5 NR = (1/2)*C gamma_5 * ( 1 + g_4 )
    SpinMatrix Cg5NR()
    {
      return SpinMatrix(Gamma(5) * NR());
    }

    //! C g_5 NR = (1/2)*C gamma_5 * ( 1 - g_4 )
    SpinMatrix Cg5NRnegPar()
    {
      SpinMatrix g_one = 1.0;
 
      return SpinMatrix(Gamma(5) * NRnegPar());
    }

    //! C gamma_- = Cgm = (C gamma_-)^T
    SpinMatrix Cgm()
    {
      SpinMatrix g_one = 1.0;
      SpinMatrix g_tmp1 = 0.5 * (Gamma(2) * g_one  +  timesI(Gamma(1) * g_one));
      return SpinMatrix(Gamma(10) * g_tmp1);
    }

    //! C gamma_4 gamma_- = Cg4m
    SpinMatrix Cg4m()
    {
      SpinMatrix g_one = 1.0;
      SpinMatrix g_tmp1 = 0.5 * (Gamma(2) * g_one  +  timesI(Gamma(1) * g_one));
      return SpinMatrix(Gamma(10) * (Gamma(8) * g_tmp1));
    }

    //! C gamma_- NR = CgmNR = C gamma_- (1/2)(1 + gamma_4)
    SpinMatrix CgmNR()
    {
      return SpinMatrix(Cgm() * NR());
    }


    //! T = (1 + gamma_4) / 2 = (1 + Gamma(8)) / 2
    SpinMatrix Tunpol()
    {
      return NR();
    }


    //! T = (1 + gamma_4) / 2 = (1 - Gamma(8)) / 2
    SpinMatrix TunpolNegPar()
    {
      return NRnegPar();
    }


    //! T = \Sigma_3 (1 + gamma_4) / 2 = -i (Gamma(3) + Gamma(11)) / 2
    SpinMatrix Tpol()
    {
      SpinMatrix g_one = 1.0;
      return SpinMatrix(0.5 * timesMinusI(Gamma(3)*g_one + Gamma(11)*g_one));
    }

    //! T = \Sigma_3 (1 - gamma_4) / 2 = -i (-Gamma(3) + Gamma(11)) / 2
    SpinMatrix TpolNegPar()
    {
      SpinMatrix g_one = 1.0;
      return SpinMatrix(0.5 * timesMinusI(Gamma(11)*g_one-Gamma(3)*g_one));
    }


    //! T = (1 + \Sigma_3)*(1 + gamma_4) / 2   = (1 + Gamma(8) - i G(3) - i G(11)) / 2
    SpinMatrix Tmixed()
    {
      SpinMatrix g_one = 1.0;
      return SpinMatrix(0.5*(g_one + Gamma(8)*g_one + timesMinusI(Gamma(3)*g_one + Gamma(11)*g_one)));
    }

    //! T = (1 - \Sigma_3)*(1 - gamma_4) / 2   = (1 - Gamma(8) + i G(3) - i G(11)) / 2
    // Need to flip the spin for time reversal
    SpinMatrix TmixedNegPar()
    {
      SpinMatrix g_one = 1.0;
      return SpinMatrix(0.5*(g_one - Gamma(8)*g_one + timesMinusI(Gamma(3)*g_one - Gamma(11)*g_one)));
    }

  }

} // end namespace
