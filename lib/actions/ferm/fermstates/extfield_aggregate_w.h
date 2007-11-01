// -*- C++ -*-
// $Id: extfield_aggregate_w.h,v 1.5 2007-11-01 20:56:46 kostas Exp $
/*! \file
 *  \brief External field functions
 */

#ifndef __extfield_functions_w_h__
#define __extfield_functions_w_h__

#include "handle.h"
#include "actions/ferm/fermstates/extfield.h"


namespace Chroma 
{

  //! Name and registration
  /*! @ingroup fermstates */
  namespace ExternalFieldEnv
  {
    bool registerAll();

    //! Construct zero field
    /*!
     * \ingroup fermstates
     */
    class ZeroExternalField : public ExternalField
    {
    public:
      //! Full constructor
      ZeroExternalField() {}

      //! Return the field
      LatticeComplex operator()(int dir) const;
    };

    struct ConstantMagneticParams{
      int time_dir ;
      Real Bfield ;
      int dir ;
    } ;
    
    //! Construct constant Magnetic field
    /*!
     * \ingroup fermstates
     */
    class ConstantMagneticExternalField : public ExternalField
    {
      int time_dir ;
      Real Bfield ;
      int dir ;
    public:
      //! Full constructor
      ConstantMagneticExternalField(ConstantMagneticParams p):time_dir(p.time_dir),Bfield(p.Bfield),dir(p.dir) {}
      //! set time default to be Nd-1 and zero external field
      ConstantMagneticExternalField():time_dir(Nd-1),Bfield(0.0),dir(0) {} 

      //! Return the field
      LatticeComplex operator()(int dummy) const;
    };

     Handle< ExternalField > reader(XMLReader& xml,  const std::string& path) ;

  }  // end namespace

  
  //! Reader
  /*! @ingroup sources */
  void read(XMLReader& xml, const string& path, ExternalFieldEnv::ConstantMagneticParams& param);
  
  //! Writer
  /*! @ingroup sources */
  void write(XMLWriter& xml, const string& path, const ExternalFieldEnv::ConstantMagneticParams& param);




#if 0
  //! Reader
  /*! @ingroup sources */
  void read(XMLReader& xml, const string& path, DerivQuarkDisplacementEnv::Params& param);

  //! Writer
  /*! @ingroup sources */
  void write(XMLWriter& xml, const string& path, const DerivQuarkDisplacementEnv::Params& param);
#endif

}  // end namespace Chroma

#endif
