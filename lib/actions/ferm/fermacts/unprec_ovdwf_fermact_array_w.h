// -*- C++ -*-
// $Id: unprec_ovdwf_fermact_array_w.h,v 1.10 2004-12-09 03:58:03 edwards Exp $
/*! \file
 *  \brief Unpreconditioned Overlap-DWF (Borici) action
 */

#ifndef __unprec_ovdwf_fermact_array_w_h__
#define __unprec_ovdwf_fermact_array_w_h__

#include "fermact.h"
#include "actions/ferm/fermacts/unprec_dwf_fermact_base_array_w.h"

using namespace QDP;

namespace Chroma
{
  //! Name and registration
  namespace UnprecOvDWFermActArrayEnv
  {
    extern const std::string name;
    extern const bool registered;
    //! Name to be used
  }
  

  //! Params for OvDWF
  struct UnprecOvDWFermActArrayParams
  {
    UnprecOvDWFermActArrayParams() {}
    UnprecOvDWFermActArrayParams(XMLReader& in, const std::string& path);
    
    Real OverMass;
    Real Mass;
    Real a5;
    int  N5;
  };


  // Reader/writers
  void read(XMLReader& xml, const string& path, UnprecOvDWFermActArrayParams& param);
  void write(XMLReader& xml, const string& path, const UnprecOvDWFermActArrayParams& param);



  //! Unpreconditioned Overlap-style (Borici) OvDWF fermion action
  /*! \ingroup fermact
   *
   * Unprecondition domain-wall fermion action. The conventions used here
   * are specified in Phys.Rev.D63:094505,2001 (hep-lat/0005002).
   */
  class UnprecOvDWFermActArray : public UnprecDWFermActBaseArray<LatticeFermion>
  {
  public:
    //! General FermBC
    UnprecOvDWFermActArray(Handle< FermBC< multi1d<LatticeFermion> > > fbc_, 
			   const Real& OverMass_, const Real& Mass_, int N5_) : 
      fbc(fbc_), OverMass(OverMass_), Mass(Mass_), N5(N5_) {a5=1;}

    //! General FermBC
    UnprecOvDWFermActArray(Handle< FermBC< multi1d<LatticeFermion> > > fbc_, 
			   const UnprecOvDWFermActArrayParams& param) :
      fbc(fbc_), OverMass(param.OverMass), Mass(param.Mass), a5(param.a5), N5(param.N5) {}

    //! Copy constructor
    UnprecOvDWFermActArray(const UnprecOvDWFermActArray& a) : 
      fbc(a.fbc), OverMass(a.OverMass), Mass(a.Mass), a5(a.a5), N5(a.N5) {}

    //! Assignment
    UnprecOvDWFermActArray& operator=(const UnprecOvDWFermActArray& a)
      {fbc=a.fbc; OverMass=a.OverMass; Mass=a.Mass; a5=a.a5; N5=a.N5; return *this;}

    //! Return the fermion BC object for this action
    const FermBC< multi1d<LatticeFermion> >& getFermBC() const {return *fbc;}

    //! Length of DW flavor index/space
    int size() const {return N5;}

    //! Return the quark mass
    Real quark_mass() const {return Mass;}

    //! Produce an unpreconditioned linear operator for this action with arbitrary quark mass
    const UnprecDWLinOpBaseArray<LatticeFermion>* unprecLinOp(Handle<const ConnectState> state, 
							      const Real& m_q) const;

    //! Destructor is automatic
    ~UnprecOvDWFermActArray() {}

  private:
    Handle< FermBC< multi1d<LatticeFermion> > >  fbc;
    Real OverMass;
    Real Mass;
    Real a5;
    int  N5;
  };

}

#endif
