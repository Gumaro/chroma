#include "enum_srcsnktype_io.h"

#include <string>
using namespace std;
using namespace Chroma;

namespace Chroma { 


  /*****************  SOURCES *****************************/
  namespace SourceTypeEnv { 

    const bool registerAll(void) 
    {
      bool success; 
      success = theSourceTypeMap::Instance().registerPair(string("POINT_SOURCE"),
							  SRC_TYPE_POINT_SOURCE);

      success &=theSourceTypeMap::Instance().registerPair(string("WALL_SOURCE" ), 
							  SRC_TYPE_WALL_SOURCE);
      
      success &=theSourceTypeMap::Instance().registerPair(string("SHELL_SOURCE" ), 
							  SRC_TYPE_SHELL_SOURCE );

      success &=theSourceTypeMap::Instance().registerPair(string("BNDST_SOURCE" ), 
							  SRC_TYPE_BNDST_SOURCE);

      return success;
    }

    const string typeIDString = "SourceType";
    const bool registered = registerAll();
  };
  
  //! Read an SourceType enum
  void read(XMLReader& xml_in,  const string& path, SourceType& t) {
    theSourceTypeMap::Instance().read(SourceTypeEnv::typeIDString,
				      xml_in, 
				      path,
				      t);
  }
  
  //! Write an SourceType enum
  void write(XMLWriter& xml_out, const string& path, const SourceType& t) {
    theSourceTypeMap::Instance().write(SourceTypeEnv::typeIDString,
				       xml_out, path, t);
  }

  /*****************  SINKS *****************************/
  namespace SinkTypeEnv { 

    const bool registerAll(void) 
    {
      bool success; 
      success = theSinkTypeMap::Instance().registerPair(string("POINT_SINK" ),
							SNK_TYPE_POINT_SINK);

      success &=theSinkTypeMap::Instance().registerPair(string("WALL_SINK" ), 
							 SNK_TYPE_WALL_SINK);

      success &=theSinkTypeMap::Instance().registerPair(string("SHELL_SINK" ), 
							 SNK_TYPE_SHELL_SINK);

      success &=theSinkTypeMap::Instance().registerPair(string("BNDST_SINK" ), 
							SNK_TYPE_BNDST_SINK );
      
      return success;
    }
    
    const string typeIDString = "SinkType";
    const bool registered = registerAll();
  };

  //! Read an SinkType enum
  void read(XMLReader& xml_in,  const string& path, SinkType& t) {
    theSinkTypeMap::Instance().read(SinkTypeEnv::typeIDString,
				    xml_in, path,t);
  }
  
  //! Write an SinkType enum
  void write(XMLWriter& xml_out, const string& path, const SinkType& t) {
    theSinkTypeMap::Instance().write(SinkTypeEnv::typeIDString,
				     xml_out, path, t);
  }


};
