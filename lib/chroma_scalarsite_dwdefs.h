// -*- C++ -*-
// $Id: chroma_scalarsite_dwdefs.h,v 1.2 2004-03-29 21:31:15 edwards Exp $

/*! \file
 * \brief Additional QDP type definitions
 */

#include "qdp.h"

#define CHROMA_USE_DWF

QDP_BEGIN_NAMESPACE(QDP);

/*! \addtogroup defs Type definitions
 *
 * User constructed types made from QDP type compositional nesting.
 * The layout is suitable for a scalar-like implementation. Namely,
 * sites are the slowest varying index.
 *
 * @{
 */

const int Ls = 8;   // HACK - FOR now fixed DW index here

// Aliases for a scalar architecture

// New guy beyond qdp_defs.h
typedef OLattice< PSpinVector< PColorVector< RComplex< PDWVector<REAL, Ls> >, Nc>, Ns> > LatticeDWFermion;
typedef OLattice< PSpinVector< PColorVector< RComplex< PDWVector<REAL, Ls> >, Nc>, Ns>>1> > LatticeHalfDWFermion;

typedef OScalar< PSpinVector< PColorVector< RComplex< PDWVector<REAL, Ls> >, Nc>, Ns> > DWFermion;


/*! @} */   // end of group defs

QDP_END_NAMESPACE();

