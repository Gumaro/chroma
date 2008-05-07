// -*- C++ -*-
// $Id: clover_fermact_params_w.h,v 3.1 2008-05-07 01:12:18 bjoo Exp $
/*! \file
 *  \brief Parameters for Clover fermion action
 */

#ifndef __clover_fermact_params_w_h__
#define __clover_fermact_params_w_h__

#include "io/aniso_io.h"

namespace Chroma
{
  //! Params for clover ferm acts
  /*! \ingroup fermacts */
  struct CloverFermActParams
  {
    CloverFermActParams();
    CloverFermActParams(XMLReader& in, const std::string& path);
    
    Real Mass;
    Real clovCoeffR;
    Real clovCoeffT;
    Real u0;

    // Optional Anisotropy
    AnisoParam_t anisoParam;
    Real max_norm;
    bool max_norm_usedP;
  };


  // Reader/writers
  /*! \ingroup fermacts */
  void read(XMLReader& xml, const string& path, CloverFermActParams& param);

  /*! \ingroup fermacts */
  void write(XMLWriter& xml, const string& path, const CloverFermActParams& param);
}

#endif
