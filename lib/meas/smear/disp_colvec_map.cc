// -*- C++ -*-
// $Id: disp_colvec_map.cc,v 1.1 2008-08-06 15:19:41 edwards Exp $
/*! \file
 * \brief Holds displaced color vectors
 */

#include "meas/smear/disp_colvec_map.h"
#include "meas/smear/displacement.h"

namespace Chroma 
{ 
  // Support for the keys of displaced color vectors
  bool operator<(const KeyDispColorVector_t& a, const KeyDispColorVector_t& b)
  {
    multi1d<int> lgaa(1);
    lgaa[0] = a.colvec;
    multi1d<int> lga = concat(lgaa, a.displacement);

    multi1d<int> lgbb(1);
    lgbb[0] = b.colvec;
    multi1d<int> lgb = concat(lgbb, b.displacement);

    return (lga < lgb);
  }


  // Constructor from smeared map 
  DispColorVectorMap::DispColorVectorMap(int disp_length,
					 const multi1d<LatticeColorMatrix>& u_smr,
					 const multi1d<LatticeColorVector>& eigen_vec)
    : displacement_length(disp_length), u(u_smr), eigen_source(eigen_vec)
  {
  }


  //! Accessor
  const LatticeColorVector&
  DispColorVectorMap::getDispVector(const KeyDispColorVector_t& key)
  {
    //Check if any displacement is needed
    if (displacement_length == 0) 
    {
      return eigen_source[key.colvec];
    }
    else
    {
      return displaceObject(key);
    }
  }


  //! Accessor
  const LatticeColorVector&
  DispColorVectorMap::displaceObject(const KeyDispColorVector_t& key)
  {
    // If no entry, then create a displaced version of the quark
    if (disp_src_map.find(key) == disp_src_map.end())
    {
      // Insert an empty entry and then modify it. This saves on
      // copying the data around
      {
	ValDispColorVector_t disp_empty;

	disp_src_map.insert(std::make_pair(key, disp_empty));

	// Sanity check - the entry better be there
	if (disp_src_map.find(key) == disp_src_map.end())
	{
	  QDPIO::cerr << __func__ 
		      << ": internal error - could not insert empty key in map"
		      << endl;
	  QDP_abort(1);
	}		      
      }

      // Modify the previous empty entry
      ValDispColorVector_t& disp_q = disp_src_map.find(key)->second;
      disp_q.vec = eigen_source[key.colvec];

      for(int i=0; i < key.displacement.size(); ++i)
      {
	if (key.displacement[i] > 0)
	{
	  int disp_dir = key.displacement[i] - 1;
	  int disp_len = displacement_length;
	  displacement(u, disp_q.vec, disp_len, disp_dir);
	}
	else if (key.displacement[i] < 0)
	{
	  int disp_dir = -key.displacement[i] - 1;
	  int disp_len = -displacement_length;
	  displacement(u, disp_q.vec, disp_len, disp_dir);
	}
      }

    } // if find in map

    // The key now must exist in the map, so return the vector
    ValDispColorVector_t& disp_q = disp_src_map.find(key)->second;

    return disp_q.vec;
  }

  /*! @} */  // end of group smear

} // namespace Chroma
