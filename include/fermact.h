// -*- C++ -*-
// $Id: fermact.h,v 1.14 2003-12-10 16:20:59 bjoo Exp $

/*! @file
 * @brief Class structure for fermion actions
 */

#ifndef __fermact_h__
#define __fermact_h__

using namespace QDP;

#include "invtype.h"
#include "linearop.h"

//! Base class for quadratic matter actions (e.g., fermions)
/*! @ingroup actions
 *
 * Supports creation and application for quadratic actions.
 * This is basically a foundry class with additional operations.
 *
 * The class holds info on the particulars of a bi-local action,
 * but it DOES NOT hold gauge fields. A specific dirac-operator
 * is a functional of the gauge fields, hence when a dirac-operator
 * is needed, it is created.
 *
 * The ConnectState holds gauge fields and whatever auxilliary info
 * is needed to create a specific dirac operator (linear operator)
 * on some background gauge field.
 *
 * The linop and lmdagm functions create a linear operator on a 
 * fixed ConnectState
 *
 * The qprop solves for the full linear system undoing all preconditioning.
 * No direct functions are provided (or needed) to call a linear system
 * solver - that is a stand-alone function in the generic programming sense.
 *
 * dsdu provides the derivative of the action on a specific background
 * gauge field.
 *
 */
template<typename T>
class FermionAction
{
public:
#if 0
  //! Produce a foundry class for linear operators
  /*! NOTE: maybe this should be abstracted to a foundry class */
//  virtual LinOpFoundry* linop(const multi1d<LatticeColorMatrix>& _u) const = 0;
#endif

  //! Given links, create the state needed for the linear operators
  /*! Default version uses a SimpleConnectState */
  virtual const ConnectState* createState(const multi1d<LatticeColorMatrix>& u) const
    {return new SimpleConnectState(u);}

  //! Produce a linear operator for this action
  virtual const LinearOperator<T>* linOp(const ConnectState& state) const = 0;

  //! Produce a linear operator M^dag.M for this action
  virtual const LinearOperator<T>* lMdagM(const ConnectState& state) const = 0;

  //! Compute quark propagator over base type
  /*! 
   * Solves  M.psi = chi
   *
   * \param psi      quark propagator ( Modify )
   * \param u        gauge field ( Read )
   * \param chi      source ( Modify )
   * \param invType  inverter type ( Read (
   * \param RsdCG    CG (or MR) residual used here ( Read )
   * \param MaxCG    maximum number of CG iterations ( Read )
   * \param ncg_had  number of CG iterations ( Write )
   *
   */
  virtual void qpropT(T& psi, 
		      const ConnectState& state, 
		      const T& chi, 
		      enum InvType invType,
		      const Real& RsdCG, 
		      int MaxCG, int& ncg_had) const;

  //! Compute quark propagator over simpler type
  /*! 
   * Solves  M.psi = chi
   *
   * \param psi      quark propagator ( Modify )
   * \param u        gauge field ( Read )
   * \param chi      source ( Modify )
   * \param invType  inverter type ( Read (
   * \param RsdCG    CG (or MR) residual used here ( Read )
   * \param MaxCG    maximum number of CG iterations ( Read )
   * \param ncg_had  number of CG iterations ( Write )
   *
   */
  virtual void qprop(typename BaseType<T>::Type_t& psi, 
		     const ConnectState& state, 
		     const typename BaseType<T>::Type_t& chi, 
		     enum InvType invType,
		     const Real& RsdCG, 
		     int MaxCG, int& ncg_had) const;
  
  //! Compute dS_f/dU
  virtual void dsdu(multi1d<LatticeColorMatrix>& result,
		    const ConnectState& state,
		    const T& psi) const
    {
      QDPIO::cerr << "FermionAction::dsdu not implemented" << endl;
      QDP_abort(1);
    }

  //! Virtual destructor to help with cleanup;
  virtual ~FermionAction() {}
};


//! Partial specialization for base class of quadratic matter actions (e.g., fermions)
/*! @ingroup actions
 *
 * Supports creation and application for quadratic actions, specialized
 * to support arrays of matter fields
 */
template<typename T>
class FermionAction< multi1d<T> >
{
public:
  //! Expected length of array index
  virtual int size() const = 0;

  //! Given links, create the state needed for the linear operators
  /*! Default version uses a SimpleConnectState */
  virtual ConnectState* createState(const multi1d<LatticeColorMatrix>& u) const
    {
      return new SimpleConnectState(u);
    }

  //! Produce a linear operator for this action
  virtual const LinearOperator< multi1d<T> >* linOp(const ConnectState& state) const = 0;

  //! Produce a linear operator M^dag.M for this action
  virtual const LinearOperator< multi1d<T> >* lMdagM(const ConnectState& state) const = 0;

  //! Compute quark propagator over base type
  /*! 
   * Solves  M.psi = chi
   *
   * \param psi      quark propagator ( Modify )
   * \param u        gauge field ( Read )
   * \param chi      source ( Modify )
   * \param invType  inverter type ( Read (
   * \param RsdCG    CG (or MR) residual used here ( Read )
   * \param MaxCG    maximum number of CG iterations ( Read )
   * \param ncg_had  number of CG iterations ( Write )
   *
   */
  virtual void qpropT(multi1d<T>& psi, 
		      const ConnectState& state, 
		      const multi1d<T>& chi, 
		      enum InvType invType,
		      const Real& RsdCG, 
		      int MaxCG, int& ncg_had) const;

  //! Compute quark propagator over base type
  /*! 
   * Solves  M.psi = chi
   *
   * \param psi      quark propagator ( Modify )
   * \param u        gauge field ( Read )
   * \param chi      source ( Modify )
   * \param invType  inverter type ( Read (
   * \param RsdCG    CG (or MR) residual used here ( Read )
   * \param MaxCG    maximum number of CG iterations ( Read )
   * \param ncg_had  number of CG iterations ( Write )
   *
   */
  virtual void qprop(T& psi, 
		     const ConnectState& state, 
		     const T& chi, 
		     enum InvType invType,
		     const Real& RsdCG, 
		     int MaxCG, int& ncg_had) const = 0;

  //! Compute dS_f/dU
  virtual void dsdu(multi1d<LatticeColorMatrix>& result,
		    const ConnectState& state,
		    const multi1d<T>& psi) const
    {
      QDPIO::cerr << "FermionAction::dsdu not implemented" << endl;
      QDP_abort(1);
    }

  //! Virtual destructor to help with cleanup;
  virtual ~FermionAction() {}
};


//! Wilson-like fermion actions
/*! @ingroup actions
 *
 * Wilson-like fermion actions
 */
template<typename T>
class WilsonTypeFermAct : public FermionAction<T>
{
public:
};


//! Unpreconditioned Wilson-like fermion actions
/*! @ingroup actions
 *
 * Unpreconditioned like Wilson-like fermion actions
 */
template<typename T>
class UnprecWilsonTypeFermAct : public WilsonTypeFermAct<T>
{
public:
#if 0
  //! Compute quark propagator
  void qprop(T& psi, 
	     const ConnectState& state, 
	     const T& chi, 
	     enum InvType invType,
	     const Real& RsdCG, 
	     int MaxCG, int& ncg_had) const;
#endif
};


//! Even-odd preconditioned Wilson-like fermion actions
/*! @ingroup actions
 *
 * Even-odd preconditioned like Wilson-like fermion actions
 */
template<typename T>
class EvenOddPrecWilsonTypeFermAct : public WilsonTypeFermAct<T>
{
public:
  //! Override to produce an even-odd prec. linear operator for this action
  /*! Covariant return rule - override base class function */
  virtual const EvenOddPrecLinearOperator<T>* linOp(const ConnectState& state) const = 0;

  //! Compute quark propagator over base type
  /*! 
   * Solves  M.psi = chi
   *
   * Provides a default version
   *
   * \param psi      quark propagator ( Modify )
   * \param u        gauge field ( Read )
   * \param chi      source ( Modify )
   * \param invType  inverter type ( Read (
   * \param RsdCG    CG (or MR) residual used here ( Read )
   * \param MaxCG    maximum number of CG iterations ( Read )
   * \param ncg_had  number of CG iterations ( Write )
   */
  virtual void qpropT(T& psi, 
		      const ConnectState& state, 
		      const T& chi, 
		      enum InvType invType,
		      const Real& RsdCG, 
		      int MaxCG, int& ncg_had) const;

  //! Compute quark propagator over simpler type
  /*! 
   * Solves  M.psi = chi
   *
   * Provides a default version
   *
   * \param psi      quark propagator ( Modify )
   * \param u        gauge field ( Read )
   * \param chi      source ( Modify )
   * \param invType  inverter type ( Read (
   * \param RsdCG    CG (or MR) residual used here ( Read )
   * \param MaxCG    maximum number of CG iterations ( Read )
   * \param ncg_had  number of CG iterations ( Write )
   */
  virtual void qprop(typename BaseType<T>::Type_t& psi, 
		     const ConnectState& state, 
		     const typename BaseType<T>::Type_t& chi, 
		     enum InvType invType,
		     const Real& RsdCG, 
		     int MaxCG, int& ncg_had) const;
  
};


//! Partial specialization of even-odd preconditioned Wilson-like fermion actions over arrays
/*! @ingroup actions
 *
 * Even-odd preconditioned like Wilson-like fermion actions.
 * Here, use arrays of matter fields.
 */
template<typename T>
class EvenOddPrecWilsonTypeFermAct< multi1d<T> > : public WilsonTypeFermAct< multi1d<T> >
{
public:
  //! Override to produce an even-odd prec. linear operator for this action
  /*! Covariant return rule - override base class function */
  virtual const EvenOddPrecLinearOperator< multi1d<T> >* linOp(const ConnectState& state) const = 0;

  //! Compute quark propagator over base type
  /*! 
   * Solves  M.psi = chi
   *
   * Provides a default version
   *
   * \param psi      quark propagator ( Modify )
   * \param u        gauge field ( Read )
   * \param chi      source ( Modify )
   * \param invType  inverter type ( Read (
   * \param RsdCG    CG (or MR) residual used here ( Read )
   * \param MaxCG    maximum number of CG iterations ( Read )
   * \param ncg_had  number of CG iterations ( Write )
   */
  virtual void qpropT(multi1d<T>& psi, 
		      const ConnectState& state, 
		      const multi1d<T>& chi, 
		      enum InvType invType,
		      const Real& RsdCG, 
		      int MaxCG, int& ncg_had) const;

  //! Compute quark propagator over simpler type
  /*! 
   * Solves  M.psi = chi
   *
   * Provides a default version
   *
   * \param psi      quark propagator ( Modify )
   * \param u        gauge field ( Read )
   * \param chi      source ( Modify )
   * \param invType  inverter type ( Read (
   * \param RsdCG    CG (or MR) residual used here ( Read )
   * \param MaxCG    maximum number of CG iterations ( Read )
   * \param ncg_had  number of CG iterations ( Write )
   */
  virtual void qprop(typename BaseType<T>::Type_t& psi, 
		     const ConnectState& state, 
		     const typename BaseType<T>::Type_t& chi, 
		     enum InvType invType,
		     const Real& RsdCG, 
		     int MaxCG, int& ncg_had) const;
  
};


//! Staggered-like fermion actions
/*! @ingroup actions
 *
 * Staggered-like fermion actions
 */
template<typename T>
class StaggeredTypeFermAct : public FermionAction<T>
{
public:
};


//! Unpreconditioned Staggered-like fermion actions
/*! @ingroup actions
 *
 * Unpreconditioned like Staggered-like fermion actions
 */
template<typename T>
class UnprecStaggeredTypeFermAct : public StaggeredTypeFermAct<T>
{
public:
#if 0
  //! Compute quark propagator
  void qprop(T& psi, 
	     const ConnectState& state, 
	     const T& chi, 
	     enum InvType invType,
	     const Real& RsdCG, 
	     int MaxCG, int& ncg_had) const;
#endif
};


//! Even-odd preconditioned Staggered-like fermion actions
/*! @ingroup actions
 *
 * Even-odd preconditioned like Staggered-like fermion actions
 */
template<typename T>
class EvenOddPrecStaggeredTypeFermAct : public StaggeredTypeFermAct<T>
{
public:
};


// Asqtad Fermion Action.
// Have a seperate class for now due to the linop's dependance on
// u_fat and u_triple rather than just u!
template<typename T>
class EvenOddPrecAsqtadFermTypeAction : 
  public EvenOddPrecStaggeredTypeFermAct<T>
{
public:
                                                                                                                                                 
  //! Produce a linear operator for this action
  /*! NOTE: maybe this should be abstracted to a foundry class object */
  virtual const LinearOperator<T>* linOp(const ConnectState& state) const = 0;
                                                                                                                                                 
  //! Produce a linear operator M^dag.M for this action
  /*! NOTE: maybe this should be abstracted to a foundry class object */
  virtual const LinearOperator<T>* lMdagM(const ConnectState& state) const = 0;
                                                                                                                                                 
  //! Compute quark propagator
  /*! NOTE: maybe this should produce a quark prop foundry */

#if 0
  void qprop(T& psi,
             const multi1d<LatticeColorMatrix>& u_fat,
             const multi1d<LatticeColorMatrix>& u_triple,
             const T& chi,
             int invType,
             const Real& RsdCG,
             int MaxCG, const Real& mass, int& ncg_had) const;
#endif                                                                                                                                            
  //! Virtual destructor to help with cleanup;
  virtual ~EvenOddPrecAsqtadFermTypeAction() {}
                                                                                                                                                 
                                                                                                                                                 
};

#endif
