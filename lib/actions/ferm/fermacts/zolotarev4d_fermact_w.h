// -*- C++ -*-
// $Id: zolotarev4d_fermact_w.h,v 1.27 2004-09-09 15:51:31 edwards Exp $

/*! \file
 *  \brief 4D Zolotarev variant of Overlap-Dirac operator
 */

#ifndef __zolotarev4d_fermact_w_h__
#define __zolotarev4d_fermact_w_h__

#include "fermact.h"
#include "actions/ferm/fermacts/overlap_fermact_base_w.h"
#include "actions/ferm/fermacts/overlap_state.h"
#include "meas/eig/eig_w.h"
#include "io/overlap_state_info.h"

using namespace QDP;

namespace Chroma
{
  //! Name and registration
  namespace Zolotarev4DFermActEnv
  {
    extern const std::string name;
    extern const bool registered;
  }


  //! Params for overlap ferm acts
  struct Zolotarev4DFermActParams
  {
    Zolotarev4DFermActParams(XMLReader& in, const std::string& path);
    
    Real Mass;
    int RatPolyDeg;
    int RatPolyDegPrecond;
    int ReorthFreqInner;

    InvertParam_t invParamInner;
    OverlapInnerSolverType InnerSolverType;
    
    std::string AuxFermAct;
    std::string AuxFermActGrp;
  };


  // Reader/writers
  void read(XMLReader& xml, const string& path, Zolotarev4DFermActParams& param);
  void write(XMLReader& xml, const string& path, const Zolotarev4DFermActParams& param);


  //! 4D Zolotarev variant of Overlap-Dirac operator
  /*!
   * \ingroup fermact
   *
   * This routine is specific to Wilson-like fermions!
   *
   * NOTE: for now we assume the kernel is a fund. rep. fermion type,
   * but that is not necessary
   */
  class Zolotarev4DFermAct : public OverlapFermActBase
  {
  public:
    //! Full constructor
    Zolotarev4DFermAct(Handle<FermBC<LatticeFermion> > fbc_,
		       Handle<UnprecWilsonTypeFermAct<LatticeFermion> > Mact_, 
		       const Real& Mass_,
		       const int RatPolyDeg_,
		       const Real& RsdCGinner_,
		       int MaxCGinner_,
		       XMLWriter& writer_,
		       const int ReorthFreqInner_=10,
		       const OverlapInnerSolverType inner_solver_type_=OVERLAP_INNER_CG_SINGLE_PASS
      ) :
      fbc(fbc_), Mact(Mact_), params.Mass(Mass_), 
      params.RatPolyDeg(RatPolyDeg_), 
      params.invParamInner.RsdCG(RsdCGinner_), 
      params.invParamInner.MaxCGi(MaxCGinner_),
      params.ReorthFreqInner(ReorthFreqInner_), 
      params.inner_solver_type(inner_solver_type_)
      {
	// Default Preconditioner degree is RatPolyDeg
	RatPolyDegPrecond = RatPolyDeg_;
      }

    Zolotarev4DFermAct(Handle<FermBC<LatticeFermion> > fbc_,
		       Handle<UnprecWilsonTypeFermAct<LatticeFermion> > Mact_, 
		       const Real& Mass_,
		       const int RatPolyDeg_,
		       const int RatPolyDegPrecond_,
		       const Real& RsdCGinner_,
		       int MaxCGinner_,
		       XMLWriter& writer_,
		       const int ReorthFreqInner_=10,
		       const OverlapInnerSolverType inner_solver_type_=OVERLAP_INNER_CG_SINGLE_PASS
      ) :
      fbc(fbc_), Mact(Mact_), Mass(Mass_), RatPolyDeg(RatPolyDeg_), RatPolyDegPrecond(RatPolyDegPrecond_),
      RsdCGinner(RsdCGinner_), MaxCGinner(MaxCGinner_), writer(writer_),
      ReorthFreqInner(ReorthFreqInner_), inner_solver_type(inner_solver_type_)
      {}

    //! Construct from param struct
    Zolotarev4DFermAct(Handle<FermBC<LatticeFermion> > fbc_,
		       const Zolotarev4DFermActParams& params,
		       XMLWriter& xml_out);

    //! Copy Constructor
    Zolotarev4DFermAct(const Zolotarev4DFermAct& a) :
      fbc(a.fbc), Mact(a.Mact), Mass(a.Mass), RatPolyDeg(a.RatPolyDeg), 
      RatPolyDegPrecond(a.RatPolyDegPrecond),
      RsdCGinner(a.RsdCGinner), MaxCGinner(a.MaxCGinner), writer(a.writer),
      ReorthFreqInner(a.ReorthFreqInner), inner_solver_type(a.inner_solver_type)
      {};
  

    // Assignment
    /* The writer makes this duff not supporting copying or assignment
       Zolotarev4DFermAct& operator=(const Zolotarev4DFermAct& a) 
       {
       fbc = a.fbc;
       Mact = a.Mact;
       Mass = a.Mass;
       RatPolyDeg=a.RatPolyDeg;
       RsdCGinner=a.RsdCGinner;
       MaxCGinner=a.MaxCGinner;
       writer = a.writer;
       return *this;
       }
    */

    //! Return the fermion BC object for this action
    const FermBC<LatticeFermion>& getFermBC() const {return *fbc;}

    //! Return the quark mass
    Real quark_mass() const {return Mass;}


    //! Is the operator Chiral 
    /*! The operator is chiral if it satisfies the GW 
     *  relation (or a massive version of it). It is certainly 
     *  the intention that this operator be chiral in this sense.
     *  However, setting it up wrongly may make it non chiral.
     *  that would need a run-time check. So this is a hack below,
     *  signifying intent */
    bool isChiral() const { return true; }
    
    XMLWriter& getWriter() const { 
      return writer;
    }

    // Create state functions

    //! Given links, create the state needed for the linear operators
    /*! Override the parent */
    const OverlapConnectState<LatticeFermion>*
    createState(const multi1d<LatticeColorMatrix>& u_) const ;

    // Just gauge field and epsilon -- Approx Max is 2*Nd 
    const OverlapConnectState<LatticeFermion>*
    createState(const multi1d<LatticeColorMatrix>& u_, 
		const Real& approxMin_) const ;
 
    // Gauge field, epsilon, approx min, approx max
    const OverlapConnectState<LatticeFermion>*
    createState(const multi1d<LatticeColorMatrix>& u_, 
		const Real& approxMin_, 
		const Real& approxMax_) const;


    // Gauge field, e-values, e-vectors
    const OverlapConnectState<LatticeFermion>*
    createState(const multi1d<LatticeColorMatrix>& u_,
		const multi1d<Real>& lambda_lo_,
		const multi1d<LatticeFermion>& evecs_lo_, 
		const Real& lambda_hi_) const;


    // Gauge field and whatever (min/max, e-vectors)
    const OverlapConnectState<LatticeFermion>*
    createState(const multi1d<LatticeColorMatrix>& u_,
		const OverlapStateInfo& state_info,
		XMLWriter& xml_out,
		Real wilsonMass = 16) const;

    //! Produce a linear operator for this action
    /*! 
     * NOTE: the arg MUST be the original base because C++ requires it for a virtual func!
     * The function will have to downcast to get the correct state
     */
    const LinearOperator<LatticeFermion>* linOp(Handle<const ConnectState> state) const;

    const LinearOperator<LatticeFermion>* linOpPrecondition(Handle<const ConnectState> state) const;

    //! Produce a linear operator M^dag.M for this action
    /*! 
     * NOTE: the arg MUST be the original base because C++ requires it for a virtual func!
     * The function will have to downcast to get the correct state
     */
    const LinearOperator<LatticeFermion>* lMdagM(Handle<const ConnectState> state) const;

    //! Produce a linear operator M^dag.M for this action to be applied
    //  to a vector of known chirality. Chirality is passed in
    const LinearOperator<LatticeFermion>* lMdagM(Handle<const ConnectState> state, const Chirality& chirality) const;

    //! Produce a linear operator M^dag.M for this action
    /*! 
     * NOTE: the arg MUST be the original base because C++ requires it for a virtual func!
     * The function will have to downcast to get the correct state
     */
    const LinearOperator<LatticeFermion>* lMdagMPrecondition(Handle<const ConnectState> state) const;

    //! Produce a linear operator M^dag.M for this action to be applied
    //  to a vector of known chirality. Chirality is passed in
    const LinearOperator<LatticeFermion>* lMdagMPrecondition(Handle<const ConnectState> state, const Chirality& chirality) const;


    //! Produce a linear operator that gives back gamma_5 eps(H)
    const LinearOperator<LatticeFermion>* lgamma5epsH(Handle<const ConnectState> state) const;

    //! Produce a linear operator that gives back gamma_5 eps(H)
    const LinearOperator<LatticeFermion>* lgamma5epsHPrecondition(Handle<const ConnectState> state) const;
    // Special qprop for now
    /*
      void qprop(LatticeFermion& psi, 
      Handle<const ConnectState> state, 
      const LatticeFermion& chi, 
      const InvertParam_t& invParam,
      int& ncg_had) const;
    */
    //! Destructor is automatic
    ~Zolotarev4DFermAct() {}

  protected:
    //! Helper in construction
    void init(int& numroot, 
	      Real& coeffP, 
	      multi1d<Real>& resP, 
	      multi1d<Real>& rootQ, 
	      int& NEig, 
	      multi1d<Real>& EigValFunc,
	      const OverlapConnectState<LatticeFermion>& state) const;

    //! Construct stuff but use RatPolyDegPrec in the polynomial
    void initPrec(int& numroot, 
		  Real& coeffP, 
		  multi1d<Real>& resP, 
		  multi1d<Real>& rootQ, 
		  int& NEig, 
		  multi1d<Real>& EigValFunc,
		  const OverlapConnectState<LatticeFermion>& state) const;


  private:
    //!  Partial constructor not allowed
    Zolotarev4DFermAct();

  private:
    Handle<FermBC<LatticeFermion> >  fbc;   // fermion BC
    // Auxilliary action used for kernel of operator
    Handle<UnprecWilsonTypeFermAct<LatticeFermion> > Mact;
    Zolotarev4DFermActParams params;
  };

}
#endif
