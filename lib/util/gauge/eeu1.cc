// $Id: eeu1.cc,v 1.1 2003-12-30 19:49:18 edwards Exp $
/*! \file
 *  \brief Exactly exponentiate a U(1) lie algebra element
 */

#include "chromabase.h"
#include "util/gauge/eeu1.h"

using namespace QDP;

//! Exactly exponentiate a U(1) lie algebra element
/*!
 * \ingroup gauge
 *
 *  \param a        LatticeColorMatrix          (Modify)
 */

void eeu1(LatticeColorMatrix& lambda)
{
  START_CODE("eeu1");

  if ( Nc != 1 )
  {
    QDPIO::cerr << "eeu1: can only handle U(1)" << endl;
    QDP_abort(1);
  }

  LatticeReal phi = imag(trace(lambda));	   
  LatticeComplex a = cmplx(cos(phi),sin(phi));
  pokeColor(lambda, a, 0,0);
    
  END_CODE("eeu1");
}
