// $Id: inline_qqq_w.cc,v 1.4 2005-09-25 20:41:09 edwards Exp $
/*! \file
 * \brief Inline construction of qqq_w
 *
 * QQQ calcs
 */

#include "meas/inline/hadron/inline_qqq_w.h"
#include "meas/inline/abs_inline_measurement_factory.h"
#include "meas/glue/mesplq.h"
#include "meas/hadron/barcomp_w.h"
#include "util/ft/sftmom.h"
#include "util/info/proginfo.h"
#include "util/ferm/diractodr.h"
#include "meas/inline/io/named_objmap.h"

namespace Chroma 
{ 
  namespace InlineQQQEnv 
  { 
    AbsInlineMeasurement* createMeasurement(XMLReader& xml_in, 
					    const std::string& path) 
    {
      return new InlineQQQ(InlineQQQParams(xml_in, path));
    }

    const std::string name = "QQQ";
    const bool registered = TheInlineMeasurementFactory::Instance().registerObject(name, createMeasurement);
  };


  //! Param input
  void read(XMLReader& xml, const string& path, InlineQQQParams::Param_t& input)
  {
    XMLReader paramtop(xml, path);

    int version;
    read(paramtop, "version", version);

    switch (version) 
    {
    case 4:
      break;

    default:
      /**************************************************************************/
      QDPIO::cerr << "Input parameter version " << version << " unsupported." << endl;
      QDP_abort(1);
    }

    read(paramtop, "Dirac_basis", input.Dirac_basis);
    read(paramtop, "nrow", input.nrow);
  }

  //! Param output
  void write(XMLWriter& xml, const string& path, const InlineQQQParams::Param_t& input)
  {
    push(xml, path);

    int version = 4;
    write(xml, "version", version);
    write(xml, "Dirac_basis", input.Dirac_basis);
    write(xml, "nrow", input.nrow);

    pop(xml);
  }

  //! Propagator input
  void read(XMLReader& xml, const string& path, InlineQQQParams::NamedObject_t& input)
  {
    XMLReader inputtop(xml, path);

    read(inputtop, "prop_ids", input.prop_ids);
    read(inputtop, "qqq_file", input.qqq_file);
  }

  //! Propagator output
  void write(XMLWriter& xml, const string& path, const InlineQQQParams::NamedObject_t& input)
  {
    push(xml, path);

    write(xml, "prop_ids", input.prop_ids);
    write(xml, "qqq_file", input.qqq_file);

    pop(xml);
  }


  // Param stuff
  InlineQQQParams::InlineQQQParams() { frequency = 0; }

  InlineQQQParams::InlineQQQParams(XMLReader& xml_in, const std::string& path) 
  {
    try 
    {
      XMLReader paramtop(xml_in, path);

      if (paramtop.count("Frequency") == 1)
	read(paramtop, "Frequency", frequency);
      else
	frequency = 1;

      // Parameters for source construction
      read(paramtop, "Param", param);

      // Read in the output propagator/source configuration info
      read(paramtop, "NamedObject", named_obj);
    }
    catch(const std::string& e) 
    {
      QDPIO::cerr << __func__ << ": Caught Exception reading XML: " << e << endl;
      QDP_abort(1);
    }
  }


  void
  InlineQQQParams::write(XMLWriter& xml_out, const std::string& path) 
  {
    push(xml_out, path);
    
    // Parameters for source construction
    Chroma::write(xml_out, "Param", param);

    // Write out the output propagator/source configuration info
    Chroma::write(xml_out, "NamedObject", named_obj);

    pop(xml_out);
  }


  // Function call
  void 
  InlineQQQ::operator()(const multi1d<LatticeColorMatrix>& u,
			XMLBufferWriter& gauge_xml,
			unsigned long update_no,
			XMLWriter& xml_out) 
  {
    START_CODE();

    push(xml_out,"qqq");
    write(xml_out, "update_no", update_no);

    QDPIO::cout << InlineQQQEnv::name << ": Generalized propagator generation" << endl;
    StopWatch swatch;

    // Write out the input
    params.write(xml_out, "Input");

    proginfo(xml_out);    // Print out basic program info

    // Write out the input
    params.write(xml_out, "Input");

    // Write out the config info
    write(xml_out, "Config_info", gauge_xml);

    push(xml_out, "Output_version");
    write(xml_out, "out_version", 4);
    pop(xml_out);

    // Calculate some gauge invariant observables just for info.
    MesPlq(xml_out, "Observables", u);

    // Check to make sure there are 3 ids
    const int Nprops = 3;
    if (params.named_obj.prop_ids.size() != Nprops)
    {
      QDPIO::cerr << "Error on input params - expecting 3 buffers" << endl;
      QDP_abort(1);
    }

    write(xml_out, "propIds", params.named_obj.prop_ids);
    write(xml_out, "barcompFile", params.named_obj.qqq_file);

    /*
     * Read the quark propagators and extract headers
     */
    multi1d<LatticePropagator> quark_propagator(Nprops);
    QQQBarcomp_t  qqq;
    qqq.Dirac_basis = false;
    qqq.forward_props.resize(Nprops);
    for(int i=0; i < Nprops; ++i)
    {
      QDPIO::cout << InlineQQQEnv::name << ": parse id = " << params.named_obj.prop_ids[i] << endl;
      try
      {
	// Snarf the data into a copy
	quark_propagator[i] =
	  TheNamedObjMap::Instance().getData<LatticePropagator>(params.named_obj.prop_ids[i]);

	// Snarf the prop info. This is will throw if the prop_ids is not there.
	XMLReader prop_file_xml, prop_record_xml;
	TheNamedObjMap::Instance().get(params.named_obj.prop_ids[i]).getFileXML(prop_file_xml);
	TheNamedObjMap::Instance().get(params.named_obj.prop_ids[i]).getRecordXML(prop_record_xml);
	
	// Try to invert this record XML into a ChromaProp struct
	// Also pull out the id of this source
	{
	  read(prop_record_xml, "/SinkSmear/PropSink", qqq.forward_props[i].sink_header);
	  read(prop_record_xml, "/SinkSmear/ForwardProp", qqq.forward_props[i].prop_header);
	  read(prop_record_xml, "/SinkSmear/PropSource", qqq.forward_props[i].source_header);
	}
      }
      catch( std::bad_cast ) 
      {
	QDPIO::cerr << InlineQQQEnv::name << ": caught dynamic cast error" 
		    << endl;
	QDP_abort(1);
      }
      catch (const string& e) 
      {
	QDPIO::cerr << InlineQQQEnv::name << ": error message: " << e 
		    << endl;
	QDP_abort(1);
      }
      QDPIO::cout << InlineQQQEnv::name << ": object successfully parsed" << endl;
    }


    // Save prop input
    write(xml_out, "Propagator_input", qqq);

    // Derived from input prop
    int j_decay = qqq.forward_props[0].source_header.j_decay;
    multi1d<int> boundary = getFermActBoundary(qqq.forward_props[0].prop_header.fermact);
    multi1d<int> t_source = qqq.forward_props[0].source_header.t_source;
    int t0      = t_source[j_decay];
    int bc_spec = boundary[j_decay];

    // Initialize the slow Fourier transform phases
    SftMom phases(0, true, j_decay);

    // Sanity check - write out the propagator (pion) correlator in the j_decay direction
    for(int i=0; i < Nprops; ++i)
    {
      multi1d<Double> prop_corr = sumMulti(localNorm2(TheNamedObjMap::Instance().getData<LatticePropagator>(params.named_obj.prop_ids[i])), 
					   phases.getSet());

      push(xml_out, "SinkSmearedProp_correlator");
      write(xml_out, "correlator_num", i);
      write(xml_out, "sink_smeared_prop_corr", prop_corr);
      pop(xml_out);
    }

    /*
     * Generalized propagator calculation
     */
    multiNd<Complex> barprop;

    // Switch to Dirac-basis if desired.
    if (params.param.Dirac_basis)
    {
      qqq.Dirac_basis = true;

      // The spin basis matrix
      SpinMatrix U = DiracToDRMat();

      LatticePropagator q_tmp;
      for(int i=0; i < Nprops; ++i)
      {
	q_tmp = adj(U) * quark_propagator[i] * U;   // DeGrand-Rossi ---> Dirac
	quark_propagator = q_tmp;
      }
    }

    // Compute generation propagator
    barcomp(barprop,
	    quark_propagator[0],
	    quark_propagator[1],
	    quark_propagator[2],
	    phases, t0, bc_spec);

    // Convert the data into a mult1d
    multi1d<Complex> barprop_1d;
    convertBarcomp(barprop_1d, barprop, j_decay);

    // Save the qqq output
    // ONLY SciDAC output format is supported!
    {
      XMLBufferWriter file_xml;
      push(file_xml, "qqq");
      int id = 0;    // NEED TO FIX THIS - SOMETHING NON-TRIVIAL NEEDED
      write(file_xml, "id", id);
      pop(file_xml);

      XMLBufferWriter record_xml;
      push(record_xml, "QQQ");
      write(record_xml, ".", qqq);  // do not write the outer group
      write(record_xml, "Config_info", gauge_xml);
      pop(record_xml);  // QQQ

      // Write the scalar data
      QDPFileWriter to(file_xml, params.named_obj.qqq_file, 
		       QDPIO_SINGLEFILE, QDPIO_SERIAL, QDPIO_OPEN);
      write(to,record_xml,barprop_1d);
      close(to);
    }

    pop(xml_out);    // qqq

    QDPIO::cout << "QQQ finished" << endl;

    END_CODE();
  } 

};
