// -*- C++ -*-
// $Id: prec_dwf_fermact_array_sse_w.h,v 1.10 2004-12-09 03:58:03 edwards Exp $
/*! \file
 *  \brief 4D style even-odd preconditioned domain-wall fermion action
 */

#ifndef __prec_dwf_fermact_array_sse_w_h__
#define __prec_dwf_fermact_array_sse_w_h__

#include "fermact.h"
#include "actions/ferm/fermacts/prec_dwf_fermact_base_array_w.h"

using namespace QDP;

namespace Chroma
{
  //! Name and registration
  namespace SSEEvenOddPrecDWFermActArrayEnv
  {
    extern const std::string name;
    extern const bool registered;
  }
  

  //! Params for DWF
  struct SSEEvenOddPrecDWFermActArrayParams
  {
    SSEEvenOddPrecDWFermActArrayParams(XMLReader& in, const std::string& path);
    
    Real OverMass;
    Real Mass;
    Real a5;
    int  N5;
  };


  // Reader/writers
  void read(XMLReader& xml, const string& path, SSEEvenOddPrecDWFermActArrayParams& param);
  void write(XMLReader& xml, const string& path, const SSEEvenOddPrecDWFermActArrayParams& param);


  //! 4D style even-odd preconditioned domain-wall fermion action
  /*! \ingroup fermact
   *
   * 4D style even-odd preconditioned domain-wall fermion action. 
   * Follows notes of Orginos (10/2003)
   *
   * Hopefully, the conventions used here
   * are specified in Phys.Rev.D63:094505,2001 (hep-lat/0005002).
   */
  class SSEEvenOddPrecDWFermActArray : public EvenOddPrecDWFermActBaseArray<LatticeFermion>
  {
  public:
    //! General FermBC
    SSEEvenOddPrecDWFermActArray(Handle< FermBC< multi1d<LatticeFermion> > > fbc_, 
				 const Real& OverMass_, const Real& Mass_, int N5_) : 
      fbc(fbc_), OverMass(OverMass_), Mass(Mass_), N5(N5_) 
      {
	a5=1;
	QDPIO::cout << "Construct SSEEvenOddPrecDWFermActArray: OverMass = " << OverMass 
		    << "  Mass = " << Mass 
		    << "  N5 = " << N5 
		    << "  a5 = " << a5 
		    << endl;

	init();
      }

    //! General FermBC
    SSEEvenOddPrecDWFermActArray(Handle< FermBC< multi1d<LatticeFermion> > > fbc_, 
				 const SSEEvenOddPrecDWFermActArrayParams& param) :
      fbc(fbc_), OverMass(param.OverMass), Mass(param.Mass), a5(param.a5), N5(param.N5) {init();}

    //! Copy constructor
    SSEEvenOddPrecDWFermActArray(const SSEEvenOddPrecDWFermActArray& a) : 
      fbc(a.fbc), OverMass(a.OverMass), Mass(a.Mass), a5(a.a5), N5(a.N5) {}

    //! Assignment
    SSEEvenOddPrecDWFermActArray& operator=(const SSEEvenOddPrecDWFermActArray& a)
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

    //! Produce an even-odd preconditioned linear operator for this action with arbitrary quark mass
    const EvenOddPrecDWLinOpBaseArray<LatticeFermion>* precLinOp(Handle<const ConnectState> state, 
								 const Real& m_q) const;

    //! Redefine quark propagator routine for 5D fermions
    /*! 
     * Solves  M.psi = chi
     *
     * \param psi      quark propagator ( Modify )
     * \param state    gauge connection state ( Read )
     * \param chi      source ( Modify )
     * \param invParam inverter parameters ( Read (
     * \param ncg_had  number of CG iterations ( Write )
     */
    void qpropT(multi1d<LatticeFermion>& psi, 
		Handle<const ConnectState> state, 
		const multi1d<LatticeFermion>& chi, 
		const InvertParam_t& invParam,
		int& ncg_had) const;
  
    //! Destructor is automatic
    ~SSEEvenOddPrecDWFermActArray() {fini();}

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
    void dwf_quarkProp4(LatticePropagator& q_sol, 
			XMLWriter& xml_out,
			const LatticePropagator& q_src,
			int t_src, int j_decay,
			Handle<const ConnectState> state,
			const InvertParam_t& invParam,
			int& ncg_had);


  protected:
    //! Hide default constructor
    SSEEvenOddPrecDWFermActArray() {}

    //! Private internal initializer
    void init();

    //! Private internal destructor
    void fini();

  private:
    Handle< FermBC< multi1d<LatticeFermion> > >  fbc;
    Real OverMass;
    Real Mass;
    Real a5;
    int  N5;
  };

}

using namespace Chroma;

#endif
