// $Id: inline_read_obj.cc,v 1.1 2005-09-23 03:43:09 edwards Exp $
/*! \file
 * \brief Inline task to read an object from a named buffer
 *
 * Named object writing
 */

#include "meas/inline/abs_inline_measurement_factory.h"
#include "meas/inline/io/inline_read_obj.h"
#include "meas/inline/io/named_objmap.h"
#include "meas/inline/io/readobj_funcmap.h"

namespace Chroma 
{ 
  namespace InlineReadNamedObjEnv 
  { 
    AbsInlineMeasurement* createMeasurement(XMLReader& xml_in, 
					    const std::string& path) 
    {
      return new InlineReadNamedObj(InlineReadNamedObjParams(xml_in, path));
    }

    const std::string name = "READ_NAMED_OBJECT";

    bool registerAll() 
    {
      bool success = true; 

      // Datatype readers
      success &= ReadObjCallMapEnv::registered;

      // Inline measurement
      success &= TheInlineMeasurementFactory::Instance().registerObject(name, createMeasurement);

      return success;
    }

    const bool registered = registerAll();
  };


  //! Object buffer
  void write(XMLWriter& xml, const string& path, const InlineReadNamedObjParams::NamedObject_t& input)
  {
    push(xml, path);

    write(xml, "object_id", input.object_id);
    write(xml, "object_type", input.object_type);

    pop(xml);
  }

  //! File output
  void write(XMLWriter& xml, const string& path, const InlineReadNamedObjParams::File_t& input)
  {
    push(xml, path);

    write(xml, "file_name", input.file_name);

    pop(xml);
  }


  //! Object buffer
  void read(XMLReader& xml, const string& path, InlineReadNamedObjParams::NamedObject_t& input)
  {
    XMLReader inputtop(xml, path);

    read(inputtop, "object_id", input.object_id);
    read(inputtop, "object_type", input.object_type);
  }

  //! File output
  void read(XMLReader& xml, const string& path, InlineReadNamedObjParams::File_t& input)
  {
    XMLReader inputtop(xml, path);

    read(inputtop, "file_name", input.file_name);
  }


  // Param stuff
  InlineReadNamedObjParams::InlineReadNamedObjParams() { frequency = 0; }

  InlineReadNamedObjParams::InlineReadNamedObjParams(XMLReader& xml_in, const std::string& path) 
  {
    try 
    {
      XMLReader paramtop(xml_in, path);

      if (paramtop.count("Frequency") == 1)
	read(paramtop, "Frequency", frequency);
      else
	frequency = 1;

      // Parameters for source construction
      read(paramtop, "NamedObject", named_obj);

      // Read in the destinatio
      read(paramtop, "File", file);
    }
    catch(const std::string& e) 
    {
      QDPIO::cerr << __func__ << ": caught Exception reading XML: " << e << endl;
      QDP_abort(1);
    }
  }


  void
  InlineReadNamedObjParams::write(XMLWriter& xml_out, const std::string& path) 
  {
    push(xml_out, path);
    
    // Parameters for source construction
    Chroma::write(xml_out, "NamedObject", named_obj);

    // Write out destination
    Chroma::write(xml_out, "File", file);

    pop(xml_out);
  }


  void 
  InlineReadNamedObj::operator()(const multi1d<LatticeColorMatrix>& u,
				  XMLBufferWriter& gauge_xml,
				  unsigned long update_no,
				  XMLWriter& xml_out) 
  {
    START_CODE();

    push(xml_out, "read_named_obj");
    write(xml_out, "update_no", update_no);

    QDPIO::cout << InlineReadNamedObjEnv::name << ": object reader" << endl;
    StopWatch swatch;

    // Read the object
    // ONLY SciDAC output format is supported in this task
    // Other tasks could support other disk formats
    QDPIO::cout << "Attempt to read object name = " << params.named_obj.object_id << endl;
    write(xml_out, "object_id", params.named_obj.object_id);
    try
    {
      swatch.reset();

      // Read the object
      swatch.start();
      TheReadObjFuncMap::Instance().callFunction(params.named_obj.object_type,
						 params.named_obj.object_id,
						 params.file.file_name, 
						 QDPIO_SERIAL);
      swatch.stop();

      QDPIO::cout << "Object successfully read: time= " 
		  << swatch.getTimeInSeconds() 
		  << " secs" << endl;
    }
    catch( std::bad_cast ) 
    {
      QDPIO::cerr << InlineReadNamedObjEnv::name << ": caught dynamic cast error" 
		  << endl;
      QDP_abort(1);
    }
    catch (const string& e) 
    {
      QDPIO::cerr << InlineReadNamedObjEnv::name << ": map call failed: " << e 
		  << endl;
      QDP_abort(1);
    }
    
    QDPIO::cout << InlineReadNamedObjEnv::name << ": ran successfully" << endl;

    pop(xml_out);  // write_named_obj

    END_CODE();
  } 

};
