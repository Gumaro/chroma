// -*- C++ -*-
// $Id: linop.h,v 1.1 2003-02-10 15:51:06 edwards Exp $

/*! @file
 * @brief Linear Operators
 */

QDP_BEGIN_NAMESPACE(QDP);

/*! @defgroup linop Linear Operators
 *
 * Supports creation and application for linear operators that
 * hold things like Dirac operators, etc.
 *
 * @{
 */

//! Linear Operator
class LinearOperator
{
public:
  //! Apply the operator onto a source vector
  virtual LatticeFermion operator() (const LatticeFermion& psi, int isign) const = 0;

  //! Return the subset on which the operator acts
  virtual const Subset& subset() const = 0;
};


//! Dslash-line Linear Operator
/*! These are concessions/optimizations for red-black checkboarding */
class DslashLinearOperator
{
public:
  //! Apply operator on both checkerboards
  virtual LatticeFermion operator() (const LatticeFermion& psi, int isign) const
    {
      LatticeFermion d;

      d[rb[0]] = this->(psi, isign, 0);
      d[rb[1]] = this->(psi, isign, 1);
    }

  virtual LatticeFermion operator() (const LatticeFermion& psi, int isign, int cb) const = 0;
};



/** @} */ // end of group linop

QDP_END_NAMESPACE();
