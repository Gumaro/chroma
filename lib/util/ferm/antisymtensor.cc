// $Id: antisymtensor.cc,v 1.3 2006-02-11 17:23:06 edwards Exp $
/*! \file
 *  \brief Compute anti-symmetric tensors
 */

#include "chromabase.h"
#include "util/ferm/antisymtensor.h"

namespace Chroma 
{

  //! Anonymous namespace
  /*! \ingroup ferm */
  namespace
  {
    multi3d<int> antisym_tensor3d;
    bool initP = false;


    //! Init gamma matrix multiplication table factors
    /*!
     * \ingroup ferm
     */
    void antiSymTensor3dInit()
    {
      START_CODE();

      antisym_tensor3d.resize(3,3,3);
      antisym_tensor3d = 0;

      // d = \epsilon^{i,j,k}
      // Permutations: +(0,1,2)+(1,2,0)+(2,0,1)-(1,0,2)-(0,2,1)-(2,1,0)

      antisym_tensor3d(0,1,2) = 1;
      antisym_tensor3d(1,2,0) = 1;
      antisym_tensor3d(2,0,1) = 1;

      antisym_tensor3d(1,0,2) = -1;
      antisym_tensor3d(0,2,1) = -1;
      antisym_tensor3d(2,1,0) = -1;
  
      initP = true;

      END_CODE();
    }
  } // end namespace


  //! Return 3d symmetric tensor
  /*!
   * \ingroup ferm
   *
   * \return  \f$\epsilon_{ijk}\f$
   */
  int antiSymTensor3d(int i, int j, int k)
  {
    if (! initP)
      antiSymTensor3dInit();

    return antisym_tensor3d(i,j,k);
  }

}  // end namespace Chroma
