// $Id: inline_mesonspec_w.cc,v 3.3 2006-05-24 21:09:41 edwards Exp $
/*! \file
 * \brief Inline construction of meson spectrum
 *
 * Meson spectrum calculations
 */

#include "handle.h"
#include "meas/inline/hadron/inline_mesonspec_w.h"
#include "meas/inline/abs_inline_measurement_factory.h"
#include "meas/glue/mesplq.h"
#include "util/ft/sftmom.h"
#include "util/info/proginfo.h"
#include "io/enum_io/enum_plusminus_io.h"
#include "io/param_io.h"
#include "io/qprop_io.h"
#include "meas/inline/make_xml_file.h"
#include "meas/inline/io/named_objmap.h"

#include "meas/hadron/spin_insertion_factory.h"
#include "meas/hadron/spin_insertion_aggregate.h"

namespace Chroma 
{ 
  namespace InlineMesonSpecEnv 
  { 
    AbsInlineMeasurement* createMeasurement(XMLReader& xml_in, 
					    const std::string& path) 
    {
      return new InlineMesonSpec(InlineMesonSpecParams(xml_in, path));
    }

    const std::string name = "MESON_SPECTRUM";

    //! Register all the factories
    bool registerAll()
    {
      bool foo = true;
      foo &= SpinInsertionEnv::registered;
      foo &= TheInlineMeasurementFactory::Instance().registerObject(name, createMeasurement);
      return foo;
    }

    //! Register the source construction
    const bool registered = registerAll();

  };



  //! Reader for parameters
  void read(XMLReader& xml, const string& path, InlineMesonSpecParams::Param_t& param)
  {
    XMLReader paramtop(xml, path);

    int version;
    read(paramtop, "version", version);

    switch (version) 
    {
    case 1:
      break;

    default:
      QDPIO::cerr << "Input parameter version " << version << " unsupported." << endl;
      QDP_abort(1);
    }

    read(paramtop, "mom2_max", param.mom2_max);
    read(paramtop, "avg_equiv_mom", param.avg_equiv_mom);
  }


  //! Writer for parameters
  void write(XMLWriter& xml, const string& path, const InlineMesonSpecParams::Param_t& param)
  {
    push(xml, path);

    int version = 1;
    write(xml, "version", version);

    write(xml, "mom2_max", param.mom2_max);
    write(xml, "avg_equiv_mom", param.avg_equiv_mom);

    pop(xml);
  }


  //! Propagator input
  void read(XMLReader& xml, const string& path, InlineMesonSpecParams::NamedObject_t::Props_t::Sinks_t& input)
  {
    XMLReader inputtop(xml, path);

    read(inputtop, "first_id", input.first_id);
    read(inputtop, "second_id", input.second_id);
    read(inputtop, "operation", input.operation);

    input.source_spin_insertion = readXMLGroup(inputtop, "SourceSpinInsertion", "SpinInsertionType");
    input.sink_spin_insertion   = readXMLGroup(inputtop, "SinkSpinInsertion", "SpinInsertionType");
  }

  //! Propagator output
  void write(XMLWriter& xml, const string& path, const InlineMesonSpecParams::NamedObject_t::Props_t::Sinks_t& input)
  {
    push(xml, path);

    write(xml, "first_id", input.first_id);
    write(xml, "second_id", input.second_id);
    xml << input.source_spin_insertion.xml;
    xml << input.sink_spin_insertion.xml;
    write(xml, "operation", input.operation);

    pop(xml);
  }


  //! Propagator input
  void read(XMLReader& xml, const string& path, InlineMesonSpecParams::NamedObject_t::Props_t& input)
  {
    XMLReader inputtop(xml, path);

    read(inputtop, "sink_ids", input.sink_ids);
  }

  //! Propagator output
  void write(XMLWriter& xml, const string& path, const InlineMesonSpecParams::NamedObject_t::Props_t& input)
  {
    push(xml, path);

    write(xml, "sink_ids", input.sink_ids);

    pop(xml);
  }


  //! Propagator input
  void read(XMLReader& xml, const string& path, InlineMesonSpecParams::NamedObject_t& input)
  {
    XMLReader inputtop(xml, path);

    read(inputtop, "gauge_id", input.gauge_id);
    read(inputtop, "prop_ids", input.prop_ids);
  }

  //! Propagator output
  void write(XMLWriter& xml, const string& path, const InlineMesonSpecParams::NamedObject_t& input)
  {
    push(xml, path);

    write(xml, "gauge_id", input.gauge_id);
    write(xml, "prop_ids", input.prop_ids);

    pop(xml);
  }


  // Param stuff
  InlineMesonSpecParams::InlineMesonSpecParams() { frequency = 0; }

  InlineMesonSpecParams::InlineMesonSpecParams(XMLReader& xml_in, const std::string& path) 
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

      // Possible alternate XML file pattern
      if (paramtop.count("xml_file") != 0) 
      {
	read(paramtop, "xml_file", xml_file);
      }
    }
    catch(const std::string& e) 
    {
      QDPIO::cerr << __func__ << ": Caught Exception reading XML: " << e << endl;
      QDP_abort(1);
    }
  }


  // Writer
  void
  InlineMesonSpecParams::write(XMLWriter& xml_out, const std::string& path) 
  {
    push(xml_out, path);
    
    Chroma::write(xml_out, "Param", param);
    Chroma::write(xml_out, "NamedObject", named_obj);
    QDP::write(xml_out, "xml_file", xml_file);

    pop(xml_out);
  }


  // Anonymous namespace
  namespace 
  {
    //! Useful structure holding sink props
    struct SinkPropContainer_t
    {
      ForwardProp_t prop_header;
      LatticePropagator quark_propagator;
      Real Mass;
    
      multi1d<int> bc; 
    
      // Now loop over the various fermion masses
      string source_type;
      string source_disp_type;
      string sink_type;
      string sink_disp_type;
    };


    //! Useful structure holding sink props
    struct AllSinks_t
    {
      SinkPropContainer_t  sink_prop_1;
      SinkPropContainer_t  sink_prop_2;
    };


    //! Read a sink prop
    void readSinkProp(SinkPropContainer_t& s, const std::string& id)
    {
      try
      {
	// Snarf the data into a copy
	s.quark_propagator =
	  TheNamedObjMap::Instance().getData<LatticePropagator>(id);
	
	// Snarf the prop info. This is will throw if the prop_id is not there
	XMLReader prop_file_xml, prop_record_xml;
	TheNamedObjMap::Instance().get(id).getFileXML(prop_file_xml);
	TheNamedObjMap::Instance().get(id).getRecordXML(prop_record_xml);
   
	// Try to invert this record XML into a ChromaProp struct
	// Also pull out the id of this source
	{
	  read(prop_record_xml, "/SinkSmear", s.prop_header);
	  
	  read(prop_record_xml, "/SinkSmear/PropSource/Source/SourceType", s.source_type);
	  read(prop_record_xml, "/SinkSmear/PropSource/Source/Displacement/DisplacementType", 
	       s.source_disp_type);

	  read(prop_record_xml, "/SinkSmear/PropSink/Sink/SinkType", s.sink_type);
	  read(prop_record_xml, "/SinkSmear/PropSink/Sink/Displacement/DisplacementType", 
	       s.sink_disp_type);
	}
      }
      catch( std::bad_cast ) 
      {
	QDPIO::cerr << InlineMesonSpecEnv::name << ": caught dynamic cast error" 
		    << endl;
	QDP_abort(1);
      }
      catch (const string& e) 
      {
	QDPIO::cerr << InlineMesonSpecEnv::name << ": error message: " << e 
		    << endl;
	QDP_abort(1);
      }


      // Derived from input prop
      // Hunt around to find the mass
      // NOTE: this may be problematic in the future if actions are used with no
      // clear def. of a Mass
      std::istringstream  xml_s(s.prop_header.prop_header.fermact);
      XMLReader  fermacttop(xml_s);
      const string fermact_path = "/FermionAction";
      string fermact;
      
      QDPIO::cout << "Try action and mass" << endl;
      try
      {
	XMLReader top(fermacttop, fermact_path);

	read(top, "FermAct", fermact);

	// Yuk - need to hop some hoops. This should be isolated.
	if (top.count("Mass") != 0) 
	{
	  read(top, "Mass", s.Mass);
	}
	else if (top.count("Kappa") != 0)
	{
	  Real Kappa;
	  read(top, "Kappa", Kappa);
	  s.Mass = kappaToMass(Kappa);    // Convert Kappa to Mass
	}
	else if (top.count("m_q") != 0) 
	{
	  read(top, "m_q", s.Mass);
	}
	else
	{
	  QDPIO::cerr << "Neither Mass nor Kappa found" << endl;
	  throw std::string("Neither Mass nor Kappa found");
	}
	s.bc = getFermActBoundary(s.prop_header.prop_header.fermact);
      }
      catch (const string& e) 
      {
	QDPIO::cerr << "Error reading fermact or mass: " << e << endl;
	QDP_abort(1);
      }
    
      QDPIO::cout << "FermAct = " << fermact << endl;
      QDPIO::cout << "Mass = " << s.Mass << endl;
    }


    //! Read all sinks
    void readAllSinks(multi1d<AllSinks_t>& s, 
		      multi1d<InlineMesonSpecParams::NamedObject_t::Props_t::Sinks_t> sink_ids)
    {
      s.resize(sink_ids.size());

      for(int i=0; i < sink_ids.size(); ++i)
      {
	QDPIO::cout << "Attempt to parse forward propagator = " << sink_ids[i].first_id << endl;
	readSinkProp(s[i].sink_prop_1, sink_ids[i].first_id);
	QDPIO::cout << "Forward propagator successfully parsed" << endl;

	QDPIO::cout << "Attempt to parse forward propagator = " << sink_ids[i].second_id << endl;
	readSinkProp(s[i].sink_prop_2, sink_ids[i].second_id);
	QDPIO::cout << "Forward propagator successfully parsed" << endl;
      }
    }

  } // namespace anonymous



  // Function call
  void 
  InlineMesonSpec::operator()(unsigned long update_no,
			      XMLWriter& xml_out) 
  {
    // If xml file not empty, then use alternate
    if (params.xml_file != "")
    {
      string xml_file = makeXMLFileName(params.xml_file, update_no);

      push(xml_out, "MesonSpectrum");
      write(xml_out, "update_no", update_no);
      write(xml_out, "xml_file", xml_file);
      pop(xml_out);

      XMLFileWriter xml(xml_file);
      func(update_no, xml);
    }
    else
    {
      func(update_no, xml_out);
    }
  }



  // Real work done here
  void 
  InlineMesonSpec::func(unsigned long update_no,
			XMLWriter& xml_out) 
  {
    START_CODE();

    StopWatch snoop;
    snoop.reset();
    snoop.start();

    // Test and grab a reference to the gauge field
    XMLBufferWriter gauge_xml;
    try
    {
      TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(params.named_obj.gauge_id);
      TheNamedObjMap::Instance().get(params.named_obj.gauge_id).getRecordXML(gauge_xml);
    }
    catch( std::bad_cast ) 
    {
      QDPIO::cerr << InlineMesonSpecEnv::name << ": caught dynamic cast error" 
		  << endl;
      QDP_abort(1);
    }
    catch (const string& e) 
    {
      QDPIO::cerr << InlineMesonSpecEnv::name << ": map call failed: " << e 
		  << endl;
      QDP_abort(1);
    }
    const multi1d<LatticeColorMatrix>& u = 
      TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(params.named_obj.gauge_id);

    push(xml_out, "MesonSpectrum");
    write(xml_out, "update_no", update_no);

    QDPIO::cout << " MESONSPEC: Meson spectroscopy for Wilson-like fermions" << endl;
    QDPIO::cout << endl << "     Gauge group: SU(" << Nc << ")" << endl;
    QDPIO::cout << "     volume: " << QDP::Layout::lattSize()[0];
    for (int i=1; i<Nd; ++i) {
      QDPIO::cout << " x " << QDP::Layout::lattSize()[i];
    }
    QDPIO::cout << endl;

    proginfo(xml_out);    // Print out basic program info

    // Write out the input
    params.write(xml_out, "Input");

    // Write out the config info
    write(xml_out, "Config_info", gauge_xml);

    push(xml_out, "Output_version");
    write(xml_out, "out_version", 2);
    pop(xml_out);


    // First calculate some gauge invariant observables just for info.
    MesPlq(xml_out, "Observables", u);

    // Keep an array of all the xml output buffers
    push(xml_out, "Wilson_hadron_measurements");

    // Now loop over the various fermion masses
    for(int lpair=0; lpair < params.named_obj.prop_ids.size(); ++lpair)
    {
      const InlineMesonSpecParams::NamedObject_t::Props_t named_obj = params.named_obj.prop_ids[lpair];

      push(xml_out, "elem");

      multi1d<AllSinks_t> all_sinks;
      readAllSinks(all_sinks, named_obj.sink_ids);

      // Derived from input prop
      int j_decay = all_sinks[0].sink_prop_1.prop_header.source_header.j_decay;
      int t0      = all_sinks[0].sink_prop_1.prop_header.source_header.t_source;
      int bc_spec = all_sinks[0].sink_prop_1.bc[j_decay] ;

      // Sanity checks
      for(int loop=0; loop < all_sinks.size(); ++loop)
      {
	if (all_sinks[loop].sink_prop_2.prop_header.source_header.j_decay != j_decay)
	{
	  QDPIO::cerr << "Error!! j_decay must be the same for all propagators " << endl;
	  QDP_abort(1);
	}
	if (all_sinks[loop].sink_prop_2.bc[j_decay] != bc_spec)
	{
	  QDPIO::cerr << "Error!! bc must be the same for all propagators " << endl;
	  QDP_abort(1);
	}
	if (all_sinks[loop].sink_prop_2.prop_header.source_header.t_source != 
	    all_sinks[loop].sink_prop_1.prop_header.source_header.t_source)
	{
	  QDPIO::cerr << "Error!! t_source must be the same for all propagators " << endl;
	  QDP_abort(1);
	}
      }
	

      // Initialize the slow Fourier transform phases
      SftMom phases(params.param.mom2_max, params.param.avg_equiv_mom, j_decay);

      // Keep a copy of the phases with NO momenta
      SftMom phases_nomom(0, true, j_decay);

      // Masses
      write(xml_out, "Mass_1", all_sinks[0].sink_prop_1.Mass);
      write(xml_out, "Mass_2", all_sinks[0].sink_prop_2.Mass);
      write(xml_out, "t0", t0);

      // Save prop input
      push(xml_out, "Forward_prop_eaders");
      for(int loop=0; loop < all_sinks.size(); ++loop)
      {
	push(xml_out, "elem");
	write(xml_out, "First_forward_prop", all_sinks[loop].sink_prop_1.prop_header);
	write(xml_out, "Second_forward_prop", all_sinks[loop].sink_prop_2.prop_header);
	pop(xml_out);
      }
      pop(xml_out);

      // Sanity check - write out the norm2 of the forward prop in the j_decay direction
      // Use this for any possible verification
      push(xml_out, "Forward_prop_correlator");
      for(int loop=0; loop < all_sinks.size(); ++loop)
      {
	push(xml_out, "elem");
	write(xml_out, "forward_prop_corr_1", sumMulti(localNorm2(all_sinks[loop].sink_prop_1.quark_propagator), 
						       phases.getSet()));
	write(xml_out, "forward_prop_corr_2", sumMulti(localNorm2(all_sinks[loop].sink_prop_2.quark_propagator), 
						       phases.getSet()));
	pop(xml_out);
      }
      pop(xml_out);


      push(xml_out, "SourceSinkType");
      for(int loop=0; loop < all_sinks.size(); ++loop)
      {
	push(xml_out, "elem");
	QDPIO::cout << "Source_type_1 = " << all_sinks[loop].sink_prop_1.source_type << endl;
	QDPIO::cout << "Sink_type_1 = " << all_sinks[loop].sink_prop_1.sink_type << endl;
	QDPIO::cout << "Source_type_2 = " << all_sinks[loop].sink_prop_2.source_type << endl;
	QDPIO::cout << "Sink_type_2 = " << all_sinks[loop].sink_prop_2.sink_type << endl;

	write(xml_out, "source_type_1", all_sinks[loop].sink_prop_1.source_type);
	write(xml_out, "source_disp_type_1", all_sinks[loop].sink_prop_1.source_disp_type);
	write(xml_out, "sink_type_1", all_sinks[loop].sink_prop_1.sink_type);
	write(xml_out, "sink_disp_type_1", all_sinks[loop].sink_prop_1.sink_disp_type);

	write(xml_out, "source_type_2", all_sinks[loop].sink_prop_1.source_type);
	write(xml_out, "source_disp_type_2", all_sinks[loop].sink_prop_1.source_disp_type);
	write(xml_out, "sink_type_2", all_sinks[loop].sink_prop_1.sink_type);
	write(xml_out, "sink_disp_type_2", all_sinks[loop].sink_prop_1.sink_disp_type);
	pop(xml_out);
      }
      pop(xml_out);


      // Do the mesons
      push(xml_out, "Mesons");
      {
	// Length of lattice in decay direction
	int length = phases.numSubsets();

	// Construct the anti-quark propagator from quark_propagator[1]
	int G5 = Ns*Ns-1;

	// Construct the meson correlation function
	LatticeComplex corr_fn = zero;

	for(int loop=0; loop < all_sinks.size(); ++loop)
	{
	  LatticePropagator prop_1, prop_2;

	  // Factory constructions
	  try
	  {
	    // Create the source spin insertion object
	    {
	      std::istringstream  xml_s(named_obj.sink_ids[loop].source_spin_insertion.xml);
	      XMLReader  inserttop(xml_s);
	      const string insert_path = "/SourceSpinInsertion";
	
	      Handle< SpinInsertion<LatticePropagator> > sourceSpinInsertion(
		ThePropSpinInsertionFactory::Instance().createObject(
		  named_obj.sink_ids[loop].source_spin_insertion.id,
		  inserttop,
		  insert_path));

	      prop_1 = (*sourceSpinInsertion)(all_sinks[loop].sink_prop_1.quark_propagator);
	    }

	    // Create the sink spin insertion object
	    {
	      std::istringstream  xml_s(named_obj.sink_ids[loop].sink_spin_insertion.xml);
	      XMLReader  inserttop(xml_s);
	      const string insert_path = "/SinkSpinInsertion";
	
	      Handle< SpinInsertion<LatticePropagator> > sinkSpinInsertion(
		ThePropSpinInsertionFactory::Instance().createObject(
		  named_obj.sink_ids[loop].sink_spin_insertion.id,
		  inserttop,
		  insert_path));

	      prop_2 = (*sinkSpinInsertion)(all_sinks[loop].sink_prop_2.quark_propagator);
	    }
	  }
	  catch(const std::string& e) 
	  {
	    QDPIO::cerr << InlineMesonSpecEnv::name << ": Caught Exception inserting: " 
			<< e << endl;
	    QDP_abort(1);
	  }


	  LatticeComplex tmp = trace((Gamma(G5) * adj(prop_2) * Gamma(G5)) * prop_1);

	  switch(named_obj.sink_ids[loop].operation)
	  {
	  case PLUS:
	    corr_fn += tmp;
	    break;

	  case MINUS:
	    corr_fn -= tmp;
	    break;

	  default:
	    QDPIO::cerr << InlineMesonSpecEnv::name 
			<< ": illegal value of operation" << endl;
	    QDP_abort(1);
	  }
	}

	multi2d<DComplex> hsum;
	hsum = phases.sft(corr_fn);

	// Loop over sink momenta
	XMLArrayWriter xml_sink_mom(xml_out,phases.numMom());
	push(xml_sink_mom, "momenta");

	for (int sink_mom_num=0; sink_mom_num < phases.numMom(); ++sink_mom_num) 
	{
	  push(xml_sink_mom);
	  write(xml_sink_mom, "sink_mom_num", sink_mom_num);
	  write(xml_sink_mom, "sink_mom", phases.numToMom(sink_mom_num));

	  multi1d<DComplex> mesprop(length);
	  for (int t=0; t < length; ++t) 
	  {
	    int t_eff = (t - t0 + length) % length;
	    mesprop[t_eff] = hsum[sink_mom_num][t];
	  }

	  write(xml_sink_mom, "mesprop", mesprop);
	  pop(xml_sink_mom);
	
	} // end for(sink_mom_num)
 
	pop(xml_sink_mom);
      }
      pop(xml_out);  // Mesons

      pop(xml_out);  // array element
    }
    pop(xml_out);  // Wilson_spectroscopy
    pop(xml_out);  // mesonspec

    snoop.stop();
    QDPIO::cout << InlineMesonSpecEnv::name << ": total time = "
		<< snoop.getTimeInSeconds() 
		<< " secs" << endl;

    QDPIO::cout << InlineMesonSpecEnv::name << ": ran successfully" << endl;

    END_CODE();
  } 

};
