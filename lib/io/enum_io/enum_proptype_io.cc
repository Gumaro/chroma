#include "enum_proptype_io.h"

#include <string>
using namespace std;
using namespace Chroma;

namespace Chroma { 

  namespace PropTypeEnv { 

    const bool registerAll(void) 
    {
      bool success; 
      success = thePropTypeMap::Instance().registerPair(string("SCIDAC"), PROP_TYPE_SCIDAC );
      success &= thePropTypeMap::Instance().registerPair(string("SZIN"), PROP_TYPE_SZIN);
      success &= thePropTypeMap::Instance().registerPair( string("KYU"), PROP_TYPE_KYU );
      
      return success;
    }
    const string typeIDString = "PropType";
    const bool registered = registerAll();
  };
  using namespace PropTypeEnv;

  //! Read a propagator type enum
  void read(XMLReader& xml_in,  const string& path, PropType& t) {
    thePropTypeMap::Instance().read(typeIDString,xml_in, path,t);
  }
  
  //! Write a propagator type enum
  void write(XMLWriter& xml_out, const string& path, const PropType& t) {
    thePropTypeMap::Instance().write(typeIDString,xml_out, path, t);
  }
};
