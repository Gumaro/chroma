#ifndef _XMLLOG_IO_H_
#define _XMLLOG_IO_H_


#include "chromabase.h"
#include "singleton.h"


namespace Chroma { 
  
  typedef SingletonHolder< XMLFileWriter > TheXMLOutputWriter;

  /*
  typedef SingletonHolder< XMLReader > TheXMLInputReader;
  */
}; // End namespace Chroma

#endif
