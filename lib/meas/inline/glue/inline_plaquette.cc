#include "meas/inline/glue/inline_plaquette.h"
#include "meas/inline/abs_inline_measurement_factory.h"
#include "meas/glue/mesplq.h"
#include "meas/inline/io/named_objmap.h"

#include "meas/inline/io/default_gauge_field.h"

namespace Chroma 
{ 

  namespace InlinePlaquetteEnv 
  { 
    namespace
    {
      AbsInlineMeasurement* createMeasurement(XMLReader& xml_in, 
					      const std::string& path) {
	
	InlinePlaquetteParams p(xml_in, path);
	return new InlinePlaquette(p);
      }

      //! Local registration flag
      bool registered = false;
    }

    const std::string name = "PLAQUETTE";

    //! Register all the factories
    bool registerAll() 
    {
      bool success = true; 
      if (! registered)
      {
	success &= TheInlineMeasurementFactory::Instance().registerObject(name, createMeasurement);
	registered = true;
      }
      return success;
    }

  }


  //! Plaquette input
  void read(XMLReader& xml, const string& path, InlinePlaquetteParams::NamedObject_t& input)
  {
    XMLReader inputtop(xml, path);

    read(inputtop, "gauge_id", input.gauge_id);
  }

  //! Plaquette output
  void write(XMLWriter& xml, const string& path, const InlinePlaquetteParams::NamedObject_t& input)
  {
    push(xml, path);

    write(xml, "gauge_id", input.gauge_id);

    pop(xml);
  }


  // Params
  InlinePlaquetteParams::InlinePlaquetteParams() 
  { 
    frequency = 0; 
    named_obj.gauge_id = InlineDefaultGaugeField::getId();
  }

  InlinePlaquetteParams::InlinePlaquetteParams(XMLReader& xml_in, const std::string& path) 
  {
    try 
    {
      XMLReader paramtop(xml_in, path);

      if (paramtop.count("Frequency") == 1)
	read(paramtop, "Frequency", frequency);
      else
	frequency = 1;

      // Ids
      read(paramtop, "NamedObject", named_obj);
    }
    catch(const std::string& e) 
    {
      QDPIO::cerr << "Caught Exception reading XML: " << e << endl;
      QDP_abort(1);
    }
  }


  void 
  InlinePlaquette::operator()(unsigned long update_no,
			      XMLWriter& xml_out) 
  {
    START_CODE();
    
    // Test and grab a reference to the gauge field
    XMLBufferWriter gauge_xml;
    try
    {
      TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(params.named_obj.gauge_id);
      TheNamedObjMap::Instance().get(params.named_obj.gauge_id).getRecordXML(gauge_xml);
    }
    catch( std::bad_cast ) 
    {
      QDPIO::cerr << InlinePlaquetteEnv::name << ": caught dynamic cast error" 
		  << endl;
      QDP_abort(1);
    }
    catch (const string& e) 
    {
      QDPIO::cerr << InlinePlaquetteEnv::name << ": map call failed: " << e 
		  << endl;
      QDP_abort(1);
    }
    const multi1d<LatticeColorMatrix>& u = 
      TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(params.named_obj.gauge_id);

    push(xml_out, "Plaquette");
    write(xml_out, "update_no", update_no);

    Double w_plaq, s_plaq, t_plaq, link; 
    multi2d<Double> plane_plaq;

    MesPlq(u, w_plaq, s_plaq, t_plaq, plane_plaq, link);
    write(xml_out, "w_plaq", w_plaq);
    write(xml_out, "s_plaq", s_plaq);
    write(xml_out, "t_plaq", t_plaq);

    if (Nd >= 2)
    {
      write(xml_out, "plane_01_plaq", plane_plaq[0][1]);
    }

    if (Nd >= 3)
    {
      write(xml_out, "plane_02_plaq", plane_plaq[0][2]);
      write(xml_out, "plane_12_plaq", plane_plaq[1][2]);
    }

    if (Nd >= 4)
    {
      write(xml_out, "plane_03_plaq", plane_plaq[0][3]);
      write(xml_out, "plane_13_plaq", plane_plaq[1][3]);
      write(xml_out, "plane_23_plaq", plane_plaq[2][3]);
    }

    write(xml_out, "link", link);
    
    pop(xml_out); // pop("Plaquette");
    
    END_CODE();
  } 

};
