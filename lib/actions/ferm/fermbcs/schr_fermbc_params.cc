// $Id: schr_fermbc_params.cc,v 2.1 2006-02-26 03:47:52 edwards Exp $
/*! \file
 *  \brief Schroedinger fermion bc
 */

#include "actions/ferm/fermbcs/schr_fermbc_params.h"

namespace Chroma
{

  // Readers and writerss for the params 
  //! Read parameters
  SchrFermBCParams::SchrFermBCParams(XMLReader& xml, const string& path)
  {
    XMLReader paramtop(xml, path);

    read(paramtop, "SchrFunType", SchrFun);
    read(paramtop, "SchrPhiMult", SchrPhiMult);
    read(paramtop, "theta", theta);
  }

  //! Read parameters
  void read(XMLReader& xml, const string& path, SchrFermBCParams& param)
  {
    SchrFermBCParams tmp(xml, path);
    param = tmp;
  }

  //! Write parameters
  void write(XMLWriter& xml_out, const string& path, const SchrFermBCParams& param)
  {
    push(xml_out, path);
    write(xml_out, "theta", param.theta);
    pop(xml_out);
  }

}
