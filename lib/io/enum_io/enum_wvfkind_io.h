// -*- C++ -*-
// $Id: enum_wvfkind_io.h,v 3.1 2006-12-10 02:06:45 edwards Exp $
/*! \file
 * \brief Wavekind enum
 */
#ifndef enum_wvfkind_io_h
#define enum_wvfkinf_io_h

#include "chromabase.h"
#include <string>
#include "singleton.h"
#include "io/enum_io/enum_type_map.h"


namespace Chroma {

  /*!
   * Types and structures
   *
   * \ingroup io
   *
   * @{
   */

  //! Wave-function types for smearing
  enum WvfKind {
    WVF_KIND_GAUSSIAN,
    WVF_KIND_EXPONENTIAL,
    WVF_KIND_GAUGE_INV_GAUSSIAN,
    WVF_KIND_WUPPERTAL,
    WVF_KIND_JACOBI
  };

  namespace WvfKindEnv { 
    extern const string typeIDString;
    extern bool registered; 
    bool registerAll(void);   // Forward declaration
  }

  // A singleton to hold the typemap
  typedef SingletonHolder<EnumTypeMap<WvfKind> > theWvfKindMap;

  // Reader and writer

  //! Read an WvfKind enum
  void read(XMLReader& r, const string& path, WvfKind& t);

  //! Write an WvfKind Type enum
  void write(XMLWriter& w, const string& path, const WvfKind& t);

  /*! @} */   // end of group io

};
#endif
