// -*- C++ -*-
// $Id: unprec_dwf_fermact_array_w.h,v 1.19 2005-02-23 19:29:20 edwards Exp $
/*! \file
 *  \brief Unpreconditioned domain-wall fermion action
 */

#ifndef __unprec_dwf_fermact_array_w_h__
#define __unprec_dwf_fermact_array_w_h__

#include "fermact.h"
#include "actions/ferm/fermacts/unprec_dwf_fermact_base_array_w.h"
#include "io/aniso_io.h"

namespace Chroma
{
  //! Name and registration
  namespace UnprecDWFermActArrayEnv
  {
    extern const std::string name;
    extern const bool registered;
  }
  

  //! Params for DWF
  struct UnprecDWFermActArrayParams
  {
    UnprecDWFermActArrayParams() {}
    UnprecDWFermActArrayParams(XMLReader& in, const std::string& path);
    
    Real OverMass;
    Real Mass;
    Real a5;
    int  N5;
    AnisoParam_t anisoParam;
  };


  // Reader/writers
  void read(XMLReader& xml, const string& path, UnprecDWFermActArrayParams& param);
  void write(XMLWriter& xml, const string& path, const UnprecDWFermActArrayParams& param);



  //! Unpreconditioned domain-wall fermion action
  /*! \ingroup fermact
   *
   * Unprecondition domain-wall fermion action. The conventions used here
   * are specified in Phys.Rev.D63:094505,2001 (hep-lat/0005002).
   */

  class UnprecDWFermActArray : public UnprecDWFermActBaseArray< LatticeFermion, multi1d<LatticeColorMatrix> >
  {
  public:
    //! General FermBC
    UnprecDWFermActArray(Handle< FermBC< multi1d<LatticeFermion> > > fbc_, 
			 const Real& OverMass_, const Real& Mass_, int N5_) : 
      fbc(fbc_)
      {
	param.a5=1;
	param.OverMass = OverMass_;
	param.Mass = Mass_;
	param.N5 = N5_;
      }


    //! General FermBC
    UnprecDWFermActArray(Handle< FermBC< multi1d<LatticeFermion> > > fbc_, 
			 const UnprecDWFermActArrayParams& p) :
      fbc(fbc_), param(p) {}

    //! Copy constructor
    UnprecDWFermActArray(const UnprecDWFermActArray& a) : 
      fbc(a.fbc), param(a.param) {}

    //! Assignment
    UnprecDWFermActArray& operator=(const UnprecDWFermActArray& a)
      {fbc=a.fbc; param=a.param; return *this;}

    //! Return the fermion BC object for this action
    const FermBC< multi1d<LatticeFermion> >& getFermBC() const {return *fbc;}

    //! Length of DW flavor index/space
    int size() const {return param.N5;}

    //! Return the quark mass
    Real getQuarkMass() const {return param.Mass;}

    //! Produce an unpreconditioned linear operator for this action with arbitrary quark mass
    const UnprecDWLinOpBaseArray< LatticeFermion, multi1d<LatticeColorMatrix> >* unprecLinOp(Handle<const ConnectState> state, 
											     const Real& m_q) const;

    //! Destructor is automatic
    ~UnprecDWFermActArray() {}

    //! Given a complete propagator as a source, this does all the inversions needed
    /*! \ingroup qprop
     *
     * This routine is actually generic to Domain Wall fermions (Array) fermions
     *
     * \param q_sol    quark propagator ( Write )
     * \param q_src    source ( Read )
     * \param xml_out  diagnostic output ( Modify )
     * \param state    gauge connection state ( Read )
     * \param t_src    time slice of source ( Read )
     * \param j_decay  direction of decay ( Read )
     * \param invParam inverter parameters ( Read )
     * \param ncg_had  number of CG iterations ( Write )
     */
    void quarkProp(LatticePropagator& q_sol,   // Oops, need to make propagator type more general
		   XMLWriter& xml_out,
		   const LatticePropagator& q_src,
		   int t_src, int j_decay,
		   Handle<const ConnectState> state,
		   const InvertParam_t& invParam,
		   bool nonRelProp,
		   bool obsvP,
		   int& ncg_had);


  private:
    Handle< FermBC< multi1d<LatticeFermion> > >  fbc;
    UnprecDWFermActArrayParams param;
  };

}

#endif
