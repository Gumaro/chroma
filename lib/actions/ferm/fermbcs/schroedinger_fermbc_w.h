// -*- C++ -*-
// $Id: schroedinger_fermbc_w.h,v 3.0 2006-04-03 04:58:48 edwards Exp $
/*! @file
 * @brief Fermion action boundary conditions
 */

#ifndef __schroedinger_fermbc_w_h__
#define __schroedinger_fermbc_w_h__

#include "fermbc.h"

namespace Chroma
{

  //! Abstract class for all gauge action boundary conditions with Schroedinger BC
  /*! @ingroup fermbcs
   *
   *  Schroedinger BC implies periodic in dirs orthog to decay dir, and some
   *  kind of fixed BC in the decay dir.
   */
  class SchrFermBC : public FermBC<LatticeFermion,
		                   multi1d<LatticeColorMatrix>, 
		                   multi1d<LatticeColorMatrix> >
  {
  public:
    //! Virtual destructor to help with cleanup;
    virtual ~SchrFermBC() {}

    //! Modify U fields according to the fermion BC in place
    virtual void modify(multi1d<LatticeColorMatrix>& u) const
    {
      for(int mu=0; mu < u.size(); ++mu)
	copymask(u[mu], lSFmask()[mu], SFBndFld()[mu]);
    }

    //! Modify fermion fields in place
    virtual void modifyF(LatticeFermion& psi) const
    {
      copymask(psi, lSFmaskF(), LatticeFermion(QDP::zero));
    }

    //! Modify fermion fields in place under a subset
    virtual void modifyF(LatticeFermion& psi, 
			 const OrderedSubset& s) const
    {
      // Ooops, this is ignoring the subset!! Need to fix
      // but I don't think this is an error though
      copymask(psi, lSFmaskF(), LatticeFermion(QDP::zero));
    }

    //! Modify fermion fields in place
    virtual void modifyF(multi1d<LatticeFermion>& psi) const
    {
      QDP_error_exit("not implemented");
    }
    
    //! Modify fermion fields in place under a subset
    virtual void modifyF(multi1d<LatticeFermion>& psi, 
			 const OrderedSubset& s) const
    {
      QDP_error_exit("not implemented");
    }

    //! Zero some gauge-like field in place on the masked links
    virtual void zero(multi1d<LatticeColorMatrix>& ds_u) const
    {
      LatticeColorMatrix z = QDP::zero;

      for(int mu=0; mu < ds_u.size(); ++mu)
	copymask(ds_u[mu], lSFmask()[mu], z);
    }

    //! Says if there are fermion non-trivial 
    bool nontrivialP() const {return true;}

  protected:
    //! Mask which lattice sites have fixed fermion fields
    virtual const LatticeBoolean& lSFmaskF() const = 0;

    //! Mask which lattice sites have fixed gauge links
    virtual const multi1d<LatticeBoolean>& lSFmask() const = 0;

    //! Fixed gauge links on only the lSFmask() sites
    virtual const multi1d<LatticeColorMatrix>& SFBndFld() const = 0;
  };

}


#endif
