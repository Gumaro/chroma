// -*- C++ -*-
// $Id: srcfil.h,v 1.5 2004-11-20 15:11:50 mcneile Exp $
/*! \file
 *  \brief Point source construction
 */

#ifndef __srcfil_h__
#define __srcfil_h__

//! Fill a specific color and spin index with 1.0
/*!
 * \ingroup hadron
 *
 * This routine is specific to Wilson fermions! 
 *
 * \param a      Source fermion
 * \param coord  Lattice coordinate
 * \param color_index  Color index
 * \param spin_index   Spin index
 */

void srcfil(LatticeFermion& a, multi1d<int>& coord, int color_index, int spin_index);

//! Fill a specific color index with 1.0
/*!
 * This routine is specific to Wilson fermions! 
 *
 * \param a      Source lattice Color Vector
 * \param coord  Lattice coordinate
 * \param color_index  Color index
 */

void srcfil(LatticeColorVector& a, multi1d<int>& coord, int color_index);

//! Fill a specific color index with 1.0
/*!
 * This routine is specific to Staggered fermions! 
 *
 * \param a      Source lattice Color Vector
 * \param coord  Lattice coordinate
 * \param color_index  Color index
 */

void srcfil(LatticeStaggeredFermion& a, multi1d<int>& coord, int color_index) ;

#endif
