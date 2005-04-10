// $Id: inline_bar3ptfn_w.cc,v 1.2 2005-04-10 17:06:22 edwards Exp $
/*! \file
 * \brief Inline measurement of bar3ptfn
 *
 * Form-factor measurements
 */

#include "meas/inline/hadron/inline_bar3ptfn_w.h"
#include "meas/inline/abs_inline_measurement_factory.h"
#include "io/qprop_io.h"
#include "meas/glue/mesplq.h"
#include "util/ft/sftmom.h"
#include "util/info/proginfo.h"
#include "meas/hadron/formfac_w.h"

namespace Chroma 
{ 
  namespace InlineBar3ptfnEnv 
  { 
    AbsInlineMeasurement* createMeasurement(XMLReader& xml_in, 
					    const std::string& path) 
    {
      return new InlineBar3ptfn(InlineBar3ptfnParams(xml_in, path));
    }

    const std::string name = "BAR3PTFN";
    const bool registered = TheInlineMeasurementFactory::Instance().registerObject(name, createMeasurement);
  };


  //! Propagator parameters
  void read(XMLReader& xml, const string& path, InlineBar3ptfnParams::Prop_t& input)
  {
    XMLReader inputtop(xml, path);

    read(inputtop, "prop_file", input.prop_file);
    read(inputtop, "seqprop_files", input.seqprop_files);

    if (inputtop.count("bar3ptfn_file") == 1)
      read(inputtop, "bar3ptfn_file", input.bar3ptfn_file);
    else
      input.bar3ptfn_file = "bar3ptfn.dat";
  }

  //! Propagator parameters
  void write(XMLWriter& xml, const string& path, const InlineBar3ptfnParams::Prop_t& input)
  {
    push(xml, path);

    write(xml, "prop_file", input.prop_file);
    write(xml, "seqprop_files", input.seqprop_files);
    write(xml, "bar3ptfn_file", input.bar3ptfn_file);

    pop(xml);
  }


  // Reader for input parameters
  void read(XMLReader& xml, const string& path, InlineBar3ptfnParams::Param_t& param)
  {
    XMLReader paramtop(xml, path);

    int version;
    read(paramtop, "version", version);

    switch (version) 
    {
    case 6:
      /**************************************************************************/
      break;

    default :
      /**************************************************************************/

      QDPIO::cerr << "Input parameter version " << version << " unsupported." << endl;
      QDP_abort(1);
    }

    read(paramtop, "mom2_max", param.mom2_max);
    read(paramtop, "nrow", param.nrow);
  }


  // Reader for input parameters
  void write(XMLWriter& xml, const string& path, const InlineBar3ptfnParams::Param_t& param)
  {
    push(xml, path);

    int version = 6;

    write(xml, "version", version);
    write(xml, "mom2_max", param.mom2_max);
    write(xml, "nrow", param.nrow);

    pop(xml);
  }


  // Param stuff
  InlineBar3ptfnParams::InlineBar3ptfnParams() { frequency = 0; }

  InlineBar3ptfnParams::InlineBar3ptfnParams(XMLReader& xml_in, const std::string& path) 
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
      read(paramtop, "Prop", prop);
    }
    catch(const std::string& e) 
    {
      QDPIO::cerr << "Caught Exception reading XML: " << e << endl;
      QDP_abort(1);
    }
  }


  void
  InlineBar3ptfnParams::write(XMLWriter& xml_out, const std::string& path) 
  {
    push(xml_out, path);
    
    // Parameters for source construction
    Chroma::write(xml_out, "Param", param);

    // Write out the output propagator/source configuration info
    Chroma::write(xml_out, "Prop", prop);

    pop(xml_out);
  }



  //--------------------------------------------------------------
  struct Output_version_t
  {
    int out_version;
  };

  struct FormFac_sequential_source_t
  {
    string            seq_src;
    multi1d<int>      t_source;
    int               t_sink;
    multi1d<int>      sink_mom;
    Complex           seq_hadron_0;
    FormFac_insertions_t  formFacs;
  };

  struct FormFac_Wilson_3Pt_fn_measurements_t
  {
    int  output_version;   // Unique id for each output version of the structures
    multi1d<FormFac_sequential_source_t> seqsrc;
  };

  struct Bar3ptfn_t
  {
    Output_version_t                      output_version;
    InlineBar3ptfnParams::Param_t         param;
    FormFac_Wilson_3Pt_fn_measurements_t  bar;
  };


  // params
  void write(BinaryWriter& bin, const Output_version_t& ver)
  {
    write(bin, ver.out_version);
  }

  // params
  void write(BinaryWriter& bin, const InlineBar3ptfnParams::Param_t& param)
  {
    write(bin, param.mom2_max);
    write(bin, param.nrow);
  }


  // 
  void write(BinaryWriter& bin, const FormFac_sequential_source_t& src)
  {
    write(bin, src.seq_src);
    write(bin, src.t_source);
    write(bin, src.t_sink);
    write(bin, src.sink_mom);
    write(bin, src.seq_hadron_0);
    write(bin, src.formFacs);
  }

  // Write a hadron measurements
  void write(BinaryWriter& bin, const FormFac_Wilson_3Pt_fn_measurements_t& had)
  {
    write(bin, had.output_version);
    write(bin, had.seqsrc);
  }

  // Write all formfactor measurements
  void write(BinaryWriter& bin, const Bar3ptfn_t& bar)
  {
    write(bin, bar.output_version);
    write(bin, bar.param);
    write(bin, bar.bar);
  }

  //--------------------------------------------------------------


  // Function call
  void 
  InlineBar3ptfn::operator()(const multi1d<LatticeColorMatrix>& u,
			     XMLBufferWriter& gauge_xml,
			     unsigned long update_no,
			     XMLWriter& xml_out) 
  {
    START_CODE();

    push(xml_out, "bar3ptfn");
    write(xml_out, "update_no", update_no);

    QDPIO::cout << " FORMFAC: Baryon form factors for Wilson fermions" << endl;

    proginfo(xml_out);    // Print out basic program info

    // Write out the input
    params.write(xml_out, "Input");

    // Write out the config info
    write(xml_out, "Config_info", gauge_xml);

    push(xml_out, "Output_version");
    write(xml_out, "out_version", 10);
    pop(xml_out);

    // First calculate some gauge invariant observables just for info.
    // This is really cheap.
    MesPlq(xml_out, "Observables", u);

    //
    // Read the quark propagator and extract headers
    //
    XMLReader prop_file_xml, prop_record_xml;
    LatticePropagator quark_propagator;
    ChromaProp_t prop_header;
    PropSource_t source_header;
    {
      QDPIO::cout << "Attempt to read forward propagator" << endl;
      readQprop(prop_file_xml, 
		prop_record_xml, quark_propagator,
		params.prop.prop_file, QDPIO_SERIAL);
   
      // Try to invert this record XML into a ChromaProp struct
      // Also pull out the id of this source
      try
      {
	read(prop_record_xml, "/Propagator/ForwardProp", prop_header);
	read(prop_record_xml, "/Propagator/PropSource", source_header);
      }
      catch (const string& e) 
      {
	QDPIO::cerr << "Error extracting forward_prop header: " << e << endl;
	QDP_abort(1);
      }

      // Save propagator input
      write(xml_out, "Propagator_file_info", prop_file_xml);
      write(xml_out, "Propagator_record_info", prop_record_xml);
    }
    QDPIO::cout << "Forward propagator successfully read" << endl;

    // Derived from input prop
    int  j_decay = source_header.j_decay;
    multi1d<int> t_source = source_header.t_source;

    // Sanity check - write out the norm2 of the forward prop in the j_decay direction
    // Use this for any possible verification
    {
      // Initialize the slow Fourier transform phases
      SftMom phases(0, true, j_decay);

      multi1d<Double> forward_prop_corr = sumMulti(localNorm2(quark_propagator), 
						   phases.getSet());

      push(xml_out, "Forward_prop_correlator");
      write(xml_out, "forward_prop_corr", forward_prop_corr);
      pop(xml_out);
    }


    //
    // Big nested structure that is image of entire file
    //
    Bar3ptfn_t  bar3pt;
    bar3pt.output_version.out_version = 10;  // bump this up everytime something changes
    bar3pt.param = params.param; // copy entire structure

    push(xml_out, "Wilson_3Pt_fn_measurements");

    // Big nested structure that is image of all form-factors
//    FormFac_Wilson_3Pt_fn_measurements_t  formfacs;
    bar3pt.bar.output_version = 3;  // bump this up everytime something changes
    bar3pt.bar.seqsrc.resize(params.prop.seqprop_files.size());

    XMLArrayWriter  xml_seq_src(xml_out, params.prop.seqprop_files.size());
    push(xml_seq_src, "Sequential_source");

    for (int seq_src_ctr = 0; seq_src_ctr < params.prop.seqprop_files.size(); ++seq_src_ctr) 
    {
      push(xml_seq_src);
      write(xml_seq_src, "seq_src_ctr", seq_src_ctr);

      // Read the sequential propagator
      // Read the quark propagator and extract headers
      LatticePropagator seq_quark_prop;
      SeqSource_t seqsource_header;
      {
	XMLReader seqprop_file_xml, seqprop_record_xml;
	readQprop(seqprop_file_xml, 
		  seqprop_record_xml, seq_quark_prop,
		  params.prop.seqprop_files[seq_src_ctr], QDPIO_SERIAL);

	// Try to invert this record XML into a ChromaProp struct
	// Also pull out the id of this source
	// NEED SECURITY HERE - need a way to cross check props. Use the ID.
	try
	{
	  read(seqprop_record_xml, "/SequentialProp/SeqSource", seqsource_header);
	}
	catch (const string& e) 
	{
	  QDPIO::cerr << "Error extracting seqprop header: " << e << endl;
	  QDP_abort(1);
	}

	// Save seqprop input
	write(xml_seq_src, "SequentialProp_file_info", seqprop_file_xml);
	write(xml_seq_src, "SequentialProp_record_info", seqprop_record_xml);
      }
      QDPIO::cout << "Sequential propagator successfully read" << endl;

      // Sanity check - write out the norm2 of the forward prop in the j_decay direction
      // Use this for any possible verification
      {
	// Initialize the slow Fourier transform phases
	SftMom phases(0, true, Nd-1);
      
	multi1d<Double> backward_prop_corr = sumMulti(localNorm2(seq_quark_prop), 
						      phases.getSet());
      
	push(xml_seq_src, "Backward_prop_correlator");
	write(xml_seq_src, "backward_prop_corr", backward_prop_corr);
	pop(xml_seq_src);
      }

      // Derived from input seqprop
      string seq_src = seqsource_header.seq_src;
      QDPIO::cout << "Seqsource name = " << seqsource_header.seq_src << endl;
      int           t_sink   = seqsource_header.t_sink;
      multi1d<int>  sink_mom = seqsource_header.sink_mom;

      write(xml_seq_src, "hadron_type", "HADRON");
      write(xml_seq_src, "seq_src", seq_src);
      write(xml_seq_src, "t_source", t_source);
      write(xml_seq_src, "t_sink", t_sink);
      write(xml_seq_src, "sink_mom", sink_mom);
	
      bar3pt.bar.seqsrc[seq_src_ctr].seq_src       = seq_src;
      bar3pt.bar.seqsrc[seq_src_ctr].t_source      = t_source;
      bar3pt.bar.seqsrc[seq_src_ctr].t_sink        = t_sink;
      bar3pt.bar.seqsrc[seq_src_ctr].sink_mom      = sink_mom;
      // This is no longer computed (or needed), but keep here to make binary happy
      bar3pt.bar.seqsrc[seq_src_ctr].seq_hadron_0  = zero;
	

      // Now the 3pt contractions
      SftMom phases(params.param.mom2_max, sink_mom, false, j_decay);
      FormFac(bar3pt.bar.seqsrc[seq_src_ctr].formFacs, 
	      u, quark_propagator, seq_quark_prop, phases, t_source[j_decay]);

      pop(xml_seq_src);   // elem
    } // end loop over sequential sources

    pop(xml_seq_src);  // Sequential_source

//    BinaryWriter  bin_out(params.prop.bar3ptfn_file);
//    write(bin_out, bar3ptfn);
//    bin_out.close();

    pop(xml_out);  // Wilson_3Pt_fn_measurements

    // Close the namelist output file XMLDAT
    pop(xml_out);     // bar3ptfn

    BinaryWriter  bin_out(params.prop.bar3ptfn_file);
    write(bin_out, bar3pt);
    bin_out.close();

    QDPIO::cout << "Baryon 3-pt ran successfully" << endl;

    END_CODE();
  } 

};
