// -*- C++ -*-
// $Id: inv_rel_gmresr_cg.h,v 1.1 2004-05-25 21:47:39 bjoo Exp $
/*! \file
 *  \brief Relaxed GMRESR algorithm of the Wuppertal Group
 */

#ifndef __inv_rel_gmresr_cg__
#define __inv_rel_gmresr_cg__

#include "chromabase.h"
#include "linearop.h"


template<typename T>
void InvRelGMRESR_CG(const ApproxLinearOperator<T>& PrecMM,
		     const ApproxLinearOperator<T>& UnprecMM,
		     const T& b,
		     T& x,
		     const Real& epsilon, 
		     const Real& epsilon_prec,
		     int MaxGMRESR, 
		     int MaxGMRESRPrec,
		     int& n_count);

#endif
