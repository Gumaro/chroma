// -*- C++ -*-
// $Id: transf.h,v 1.5 2004-11-12 10:49:38 mcneile Exp $

#ifndef __transf_h__
#define __transf_h__

//! Convert (insert) a LatticeColorVector into a LatticeFermion
/*!
 * \ingroup ferm
 *
 * \param a      Source color vector
 * \param b      Destination fermion
 * \param spin_index   Spin index
 */
void CvToFerm(const LatticeColorVectorF& a, LatticeFermionF& b, 
	      int spin_index);

//! Convert (insert) a LatticeColorVector into a LatticeFermion
/*!
 * \ingroup ferm
 *
 * \param a      Source color vector
 * \param b      Destination fermion
 * \param spin_index   Spin index
 */
void CvToFerm(const LatticeColorVectorD& a, LatticeFermionD& b, 
	      int spin_index);


//! Insert a LatticeFermion into a LatticePropagator
/*!
 * \ingroup ferm
 *
 * \param a      Source fermion
 * \param b      Destination propagator
 * \param color_index  Color index
 * \param spin_index   Spin index
 */
void FermToProp(const LatticeFermionF& a, LatticePropagatorF& b, 
		int color_index, int spin_index);

//! Insert a LatticeFermion into a LatticePropagator
/*!
 * \ingroup ferm
 *
 * \param a      Source fermion
 * \param b      Destination propagator
 * \param color_index  Color index
 * \param spin_index   Spin index
 */
void FermToProp(const LatticeFermionD& a, LatticePropagatorD& b, 
		int color_index, int spin_index);


//! Extract a LatticeFermion from a LatticePropagator
/*!
 * \ingroup ferm
 *
 * \param a      Source propagator
 * \param b      Destination fermion
 * \param color_index  Color index
 * \param spin_index   Spin index
 */
void PropToFerm(const LatticePropagatorF& b, LatticeFermionF& a, 
		int color_index, int spin_index);

//! Extract a LatticeFermion from a LatticePropagator
/*!
 * \ingroup ferm
 *
 * \param a      Source propagator
 * \param b      Destination fermion
 * \param color_index  Color index
 * \param spin_index   Spin index
 */
void PropToFerm(const LatticePropagatorD& b, LatticeFermionD& a, 
		int color_index, int spin_index);


//! Insert a LatticeStaggeredFermion into a LatticeStaggeredPropagator
/*!
 * \ingroup ferm
 *
 * \param a      Source fermion
 * \param b      Destination propagator
 * \param color_index  Color index
 *
 */
void FermToProp(const LatticeStaggeredFermion& a, LatticeStaggeredPropagator& b, 
		int color_index) ; 


#endif


