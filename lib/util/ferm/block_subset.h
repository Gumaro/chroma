// -*- C++ -*-
// $Id: block_subset.h,v 1.1 2009-01-16 04:06:34 kostas Exp $
/*! \file
 * \brief Key for propagator colorvector sources
 */

#ifndef __FUNC_block_h__
#define __FUNC_block_h__

#include "chromabase.h"

namespace Chroma
{

  //! Function object used for constructing a block  set
  class BlockFunc : public SetFunc
  {
  public:
    BlockFunc(const multi1d<int>& blk ): time_dir(-1){
      init(blk);
    }
    BlockFunc(int dir, const multi1d<int>& blk ): time_dir(dir){
      init(blk);
    }

    int operator() (const multi1d<int>& coordinate) const{
      int b = coordinate[0]/block[0] ;
      
      for(int d(1);d<Nd ;d++){
	int x = coordinate[d]/block[d] ; 
	b += blockNum[d-1]*x ;
      }
      return b ;
      
    }

    int numSubsets() const{
      return Nblocks ;
    }

  private:
    BlockFunc() {}  // hide default constructor
    
    void BlockFunc::init(const multi1d<int>& blk)  {
      blockNum.resize(Nd) ;
      block.resize(Nd) ;
      Nblocks = 1 ;
      int k(0);
      for(int d(0); d < Nd; d++){
	if (d == time_dir)
	  block[d] = Layout::lattSize()[time_dir] ;
	else{
	  if(k<blk.size()){
	    block[d] = blk[k] ;
	    k++ ;
	  }
	  else
	    block[d] = Layout::lattSize()[d] ;
	}
	blockNum[d] = Layout::lattSize()[d] / block[d];
	Nblocks *= blockNum[d] ;
      }// d
    }


    int time_dir;
    multi1d<int> block ;
    multi1d<int> blockNum;
    int Nblocks ;
  };

} // namespace Chroma

#endif
