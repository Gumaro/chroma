// $Id: chroma_init.cc,v 1.5 2005/01/24 21:31:32 edwards Exp 
/*! \file
 *  \brief Initialization of Chroma
 */

#include "init/chroma_init.h"
#include "io/xmllog_io.h"

namespace Chroma 
{

//  namespace ChromaUtil
//  {

  namespace Private
  {
    //! The "DATA" filename
    string input_filename = "DATA";

    //! The "XMLDAT" filename
    string output_filename = "XMLDAT";

    //! The "XMLLOG" filename
    string log_filename = "XMLLOG";

    //! The "current working directory" -- prepended to above if set
    string cwd = ".";

    //! Has the xml output instance been created?
    bool xmlOutputP = false;

    //! Has the xml log instance been created?
    bool xmlLogP = false;
  }


  //! Get input file name
  string getXMLInputFileName() {return Private::input_filename;}

  //! Get output file name
  string getXMLOutputFileName() {return Private::output_filename;}

  //! Get log file name
  string getXMLLogFileName() {return Private::log_filename;}

  //! Get current working directory
  string getCWD() {return Private::cwd;}


  //! Set input file name
  void setXMLInputFileName(const string& name) {Private::input_filename = name;}

  //! Set output file name
  void setXMLOutputFileName(const string& name) {Private::output_filename = name;}

  //! Set output logfile name
  void setXMLLogFileName(const string& name) {Private::log_filename = name;}

  //! Set current working directory
  void setCWD(const string& name) {Private::cwd = name;}



  //! Chroma initialisation routine
  void initialize(int* argc, char ***argv) 
  {
    QDP_initialize(argc, argv);
    
    for(int i=0; i < *argc; i++) 
    {
      // Get argv[i] into a string
      string argv_i = string( (*argv)[i] );

      // Search for -i or --chroma-i
      if( argv_i == string("-h") || argv_i == string("--help") ) 
      {
	QDPIO::cerr << "Usage: " << (*argv)[0] << "  <options>" << endl
		    << "   -h           help\n"
		    << "   --help       help\n"
		    << "   -i           [" << getXMLInputFileName() << "]  xml input file name\n"
		    << "   --chroma-i   [" << getXMLInputFileName() << "]  xml input file name\n"
		    << "   -o           [" << getXMLOutputFileName() << "]  xml output file name\n"
		    << "   --chroma-p   [" << getXMLOutputFileName() << "]  xml output file name\n"
		    << "   -l           [" << getXMLLogFileName() << "]  xml output file name\n"
		    << "   --chroma-l   [" << getXMLLogFileName() << "]  xml log file name\n"
		    << "   -cwd         [" << getCWD() << "]  xml log file name\n"
		    << "   --chroma-cwd [" << getCWD() << "]  xml log file name\n"
		    << endl;
	QDP_abort(0);
      }

      // Search for -i or --chroma-i
      if( argv_i == string("-i") || argv_i == string("--chroma-i") ) 
      {
	if( i + 1 < *argc ) 
	{
	  setXMLInputFileName(string( (*argv)[i+1] ));
	  // Skip over next
	  i++;
	}
	else 
	{
	  // i + 1 is too big
	  QDPIO::cerr << "Error: dangling -i specified. " << endl;
	  QDP_abort(1);
	}
      }
 
      // Search for -o or --chroma-o
      if( argv_i == string("-o") || argv_i == string("--chroma-o") ) 
      {
	if( i + 1 < *argc ) {
	  setXMLOutputFileName(string( (*argv)[i+1] ));
	  // Skip over next
	  i++;
	}
	else {
	  // i + 1 is too big
	  QDPIO::cerr << "Error: dangling -o specified. " << endl;
	  QDP_abort(1);
	}
      }
      
      // Search for -l or --chroma-l
      if( argv_i == string("-l") || argv_i == string("--chroma-l") ) 
      {
	if( i + 1 < *argc ) {
	  setXMLLogFileName(string( (*argv)[i+1] ));
	  // Skip over next
	  i++;
	}
	else {
	  // i + 1 is too big
	  QDPIO::cerr << "Error: dangling -o specified. " << endl;
	  QDP_abort(1);
	}
      }
      
    }

    // Init done
  }


  //! Chroma finalization routine
  void finalize(void)
  {
    QDP_finalize();
  }


  //! Chroma abort routine
  void abort(int i) 
  {
    QDP_abort(i);
  }


  //! Get xml output instance
  XMLFileWriter& getXMLOutputInstance()
  {
    if (! Private::xmlOutputP)
    {
      TheXMLOutputWriter::Instance().open(Chroma::getXMLOutputFileName());
      Private::xmlOutputP = true;
    }

    return TheXMLOutputWriter::Instance();
  }
  
  //! Get xml log instance
  XMLFileWriter& getXMLLogInstance()
  {
    if (! Private::xmlLogP)
    {
      TheXMLLogWriter::Instance().open(Chroma::getXMLLogFileName());
      Private::xmlLogP = true;
    }

    return TheXMLLogWriter::Instance();
  }
  
  

//  }
}
