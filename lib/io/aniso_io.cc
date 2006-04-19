// $Id: aniso_io.cc,v 3.1 2006-04-19 02:26:34 edwards Exp $
/*! \file
 *  \brief Anisotropy parameters
 */

#include "io/aniso_io.h"

namespace Chroma 
{ 

  //! Initialize a anisotropy param struct
  AnisoParam_t::AnisoParam_t()
  {
    anisoP = false;
    t_dir  = Nd-1;   // doesn't matter - should not be used when anisoP is false
    xi_0   = 1;
    nu     = 1;
  }
  
  //! Read a anisotropy param struct
  void read(XMLReader& xml, const string& path, AnisoParam_t& param)
  {
    QDPIO::cout << "Reading an AnisoParam_t" << endl;

    XMLReader paramtop(xml, path);

    read(paramtop, "anisoP", param.anisoP);

    // To avoid confusion later, if the anisoP is false, then set the
    // struct to its default state.
    if (param.anisoP)
    {
      read(paramtop, "t_dir", param.t_dir);
      read(paramtop, "xi_0", param.xi_0);

      if (paramtop.count("nu") != 0) 
	read(paramtop, "nu", param.nu);
      else
	param.nu = 1.0;
    }
    else
    {
      AnisoParam_t foo;
      param = foo;
    }
  }


  //! Write a anisotropy param struct
  void write(XMLWriter& xml, const string& path, const AnisoParam_t& param)
  {
    push(xml, path);

    write(xml, "anisoP", param.anisoP);
    write(xml, "t_dir", param.t_dir);
    write(xml, "xi_0", param.xi_0);
    write(xml, "nu", param.nu);

    pop(xml);
  }

}  // end namespace Chroma
