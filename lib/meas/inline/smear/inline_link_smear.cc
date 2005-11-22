// $Id: inline_link_smear.cc,v 2.1 2005-11-22 22:00:27 edwards Exp $
/*! \file
 *  \brief Inline Link smearing
 */

#include "meas/inline/smear/inline_link_smear.h"
#include "meas/inline/abs_inline_measurement_factory.h"
#include "meas/glue/mesplq.h"
#include "util/gauge/unit_check.h"
#include "meas/inline/io/named_objmap.h"
#include "meas/smear/link_smearing_factory.h"


namespace Chroma 
{ 
  // Reader for out gauge file
  void read(XMLReader& xml, const string& path, InlineLinkSmearEnv::Params::NamedObject_t& input)
  {
    XMLReader inputtop(xml, path);
    read(inputtop, "linksmear_id", input.linksmear_id);
  }

  // Reader for out gauge file
  void write(XMLWriter& xml, const string& path, const InlineLinkSmearEnv::Params::NamedObject_t& input)
  {
    push(xml, path);

    write(xml, "linksmear_id", input.linksmear_id);

    pop(xml);
  }


  namespace InlineLinkSmearEnv 
  { 
    AbsInlineMeasurement* createMeasurement(XMLReader& xml_in, 
					    const std::string& path) 
    {
      return new InlineMeas(Params(xml_in, path));
    }

    const std::string name = "LINK_SMEAR";
    const bool registered = TheInlineMeasurementFactory::Instance().registerObject(name, createMeasurement);


    // Param stuff
    Params::Params() { frequency = 0; }

    Params::Params(XMLReader& xml_in, const std::string& path) 
    {
      try 
      {
	XMLReader paramtop(xml_in, path);

	if (paramtop.count("Frequency") == 1)
	  read(paramtop, "Frequency", frequency);
	else
	  frequency = 1;
      
	// Read program parameters
	{
	  XMLReader xml_tmp(paramtop, "Param");
	  std::ostringstream os;
	  xml_tmp.print(os);
	  read(xml_tmp, "LinkSmearingType", link_smearing_type);
	  link_smearing = os.str();
	}

	// Read in the linksmear outfile
	read(paramtop, "NamedObject", named_obj);
      }
      catch(const std::string& e) 
      {
	QDPIO::cerr << "Caught Exception reading XML: " << e << endl;
	QDP_abort(1);
      }
    }


    // Write params
    void
    Params::write(XMLWriter& xml, const std::string& path) 
    {
      push(xml, path);
      
      xml << link_smearing;
      Chroma::write(xml, "NamedObject", named_obj);

      pop(xml);
    }


    void 
    InlineMeas::operator()(const multi1d<LatticeColorMatrix>& u,
			   XMLBufferWriter& gauge_xml,
			   unsigned long update_no,
			   XMLWriter& xml_out) 
    {
      push(xml_out, "linksmear");
      write(xml_out, "update_no", update_no);
    
      // Write out the input
      params.write(xml_out, "Input");

      // Write out the config header
      write(xml_out, "Config_info", gauge_xml);

      push(xml_out, "Output_version");
      write(xml_out, "out_version", 1);
      pop(xml_out);

      // Calculate some gauge invariant observables
      MesPlq(xml_out, "Observables", u);

      //
      // Smear the gauge field if needed
      //
      multi1d<LatticeColorMatrix> u_smr = u;
      linkSmear(u_smr, std::string("/Param"), params.link_smearing, params.link_smearing_type);

      // Check if the smeared gauge field is unitary
      unitarityCheck(u_smr);
  
      // Again calculate some gauge invariant observables
      MesPlq(xml_out, "Link_observables", u_smr);

      // Now store the configuration to a memory object
      {
	XMLBufferWriter file_xml, record_xml;
	push(file_xml, "gauge");
	write(file_xml, "id", int(0));
	pop(file_xml);
	record_xml << gauge_xml;

	// Store the gauge field
	TheNamedObjMap::Instance().create< multi1d<LatticeColorMatrix> >(params.named_obj.linksmear_id);
	TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(params.named_obj.linksmear_id) = u_smr;
	TheNamedObjMap::Instance().get(params.named_obj.linksmear_id).setFileXML(file_xml);
	TheNamedObjMap::Instance().get(params.named_obj.linksmear_id).setRecordXML(record_xml);
      }

      pop(xml_out);

      END_CODE();
    } 

  }

}
