#ifndef enum_eigenvectype_io_h
#define enum_eigenvectype_io_h

#include "chromabase.h"
#include <string>
#include "singleton.h"
#include "io/enum_io/enum_type_map.h"


using namespace std;
using namespace Chroma;

namespace Chroma {

  /*!
   * Types and structures
   *
   * \ingroup io
   *
   * @{
   */
  //! Eigenvector type
  enum EigenVecType {
    EVEC_TYPE_SCIDAC = 2,
    EVEC_TYPE_SZIN
  };


  namespace EigenVecTypeEnv { 
    extern const string typeIDString;
    extern bool registered; 
    bool registerAll(void);   // Forward declaration
  }

  // A singleton to hold the typemap
  typedef SingletonHolder<EnumTypeMap<EigenVecType> > theEigenVecTypeMap;

  // Reader and writer

  //! Read an eigenvector enum
  void read(XMLReader& r, const string& path, EigenVecType& t);

  //! Write an eigenvector enum
  void write(XMLWriter& w, const string& path, const EigenVecType& t);

};
#endif
