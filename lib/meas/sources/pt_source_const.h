// -*- C++ -*-
// $Id: pt_source_const.h,v 2.4 2005-11-16 02:34:58 edwards Exp $
/*! \file
 *  \brief Point source construction
 */

#ifndef __pt_source_const_h__
#define __pt_source_const_h__

#include "meas/sources/source_construction.h"

namespace Chroma
{

  //! Name and registration
  /*! @ingroup sources */
  namespace PointQuarkSourceConstEnv
  {
    extern const std::string name;
    extern const bool registered;

  
    //! Point source parameters
    /*! @ingroup sources */
    struct Params
    {
      Params();
      Params(XMLReader& in, const std::string& path);
      void writeXML(XMLWriter& in, const std::string& path) const;
    
      std::string      link_smearing;        /*!< link smearing xml */
      std::string      link_smearing_type;   /*!< link smearing type name */

      int              disp_length;          /*!< displacement length */
      int              disp_dir;             /*!< x(0), y(1), z(2) */   

      int              j_decay;              /*!< decay direction */
      multi1d<int>     t_srce;               /*!< source location */
    };


    //! Point source construction
    /*! @ingroup sources
     *
     * Create a point propagator source
     */
    template<typename T>
    class SourceConst : public QuarkSourceConstruction<T>
    {
    public:
      //! Full constructor
      SourceConst(const Params& p) : params(p) {}

      //! Construct the source
      T operator()(const multi1d<LatticeColorMatrix>& u) const;

    private:
      //! Hide partial constructor
      SourceConst() {}

    private:
      Params  params;   /*!< source params */
    };

  }  // end namespace


  //! Reader
  /*! @ingroup sources */
  void read(XMLReader& xml, const string& path, PointQuarkSourceConstEnv::Params& param);

  //! Writer
  /*! @ingroup sources */
  void write(XMLWriter& xml, const string& path, const PointQuarkSourceConstEnv::Params& param);

}  // end namespace Chroma


#endif
