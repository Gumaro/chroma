// $Id: chromabase.h,v 1.13 2004-01-05 05:01:33 edwards Exp $
//
// Absolute basic stuff to use chroma
//
/*! \file
 * \brief Primary include file for CHROMA library code
 *
 * This is the absolute basic stuff to use Chroma in say
 * library codes.
 */

#ifndef CHROMABASE_INCLUDE
#define CHROMABASE_INCLUDE

#include "qdp.h"

#if defined(ARCH_SCALAR) || defined(ARCH_PARSCALAR)
#include "chroma_scalarsite_dwdefs.h"

#elif defined(ARCH_SCALARVEC) || defined(ARCH_PARSCALARVEC)
// #include "chroma_scalarvecsite_defs.h"

#else
#error "Unknown architecture ARCH"
#endif

namespace QDP {

// Trait classes to undo DW index
template<class T>
struct BaseType {};

template<>
struct BaseType<LatticeFermion>
{
  typedef LatticeFermion   Type_t;
};

template<>
struct BaseType<LatticeDWFermion>
{
  typedef LatticeFermion   Type_t;
};

}

using namespace QDP;

// Extremely basic types
enum PlusMinus {PLUS = 1, MINUS = -1};


// Useful constants
const Real fuzz = 1.0e-5;
const Real twopi = 6.283185307179586476925286;

#define TO_REAL(a) float(a)


// Hooks for various things
#define START_CODE(a)
#define END_CODE(a)


#endif
