// $Id: simple_fermbc_s.cc,v 1.1 2004-12-24 04:23:20 edwards Exp $
/*! \file
 *  \brief Simple fermionic BC
 */

#include "actions/ferm/fermbcs/simple_fermbc_s.h"
#include "actions/ferm/fermbcs/fermbc_factory_s.h"

namespace Chroma
{

  //! Name and registration
  namespace StaggeredTypeSimpleFermBCEnv
  {
    //! Callback function
    FermBC<LatticeStaggeredFermion>* createFermBC(XMLReader& xml_in, const std::string& path)
    {
      SimpleFermBCParams bc(xml_in, path);
      return new SimpleFermBC<LatticeStaggeredFermion>(bc.boundary);
    }

    //! Name to be used 
    const std::string name = SimpleFermBCEnv::name;

    //! Register the fermbc
    const bool registered = TheStaggeredTypeFermBCFactory::Instance().registerObject(name, createFermBC);
  }

}
