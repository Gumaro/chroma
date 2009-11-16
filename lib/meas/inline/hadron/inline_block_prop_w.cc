// $Id: inline_block_prop_w.cc,v 1.2 2009-02-23 19:52:02 edwards Exp $
/*! \file
 * \brief Compute the matrix element of   M^-1 * multi1d<LatticeColorVector>
 *
 * Propagator calculation on a colorvector
 */

#include "fermact.h"
#include "meas/inline/hadron/inline_block_prop_w.h"
#include "meas/inline/abs_inline_measurement_factory.h"
#include "meas/glue/mesplq.h"
#include "util/ferm/subset_vectors.h"

#include "io/xml_group_reader.h"
#include "util/ferm/map_obj.h"
#include "util/ferm/map_obj/map_obj_factory_w.h"
#include "util/ferm/map_obj/map_obj_aggregate_w.h"

#include "util/ferm/key_block_prop.h"
#include "util/ferm/block_subset.h"
#include "util/ferm/transf.h"
#include "util/ft/sftmom.h"
#include "util/info/proginfo.h"
#include "actions/ferm/fermacts/fermact_factory_w.h"
#include "actions/ferm/fermacts/fermacts_aggregate_w.h"

#include "meas/inline/make_xml_file.h"

#include "meas/inline/io/named_objmap.h"

namespace Chroma 
{ 
  namespace InlineBlockPropEnv 
  {
    //! Propagator input
    void read(XMLReader& xml, const string& path, InlineBlockPropEnv::Params::NamedObject_t& input)
    {
      XMLReader inputtop(xml, path);

      read(inputtop, "gauge_id", input.gauge_id);
      read(inputtop, "colorvec_id", input.colorvec_id);
      read(inputtop, "prop_id", input.prop_id);
    }

    //! Propagator output
    void write(XMLWriter& xml, const string& path, const InlineBlockPropEnv::Params::NamedObject_t& input)
    {
      push(xml, path);

      write(xml, "gauge_id", input.gauge_id);
      write(xml, "colorvec_id", input.colorvec_id);
      write(xml, "prop_id", input.prop_id);

      pop(xml);
    }


    //! Propagator input
    void read(XMLReader& xml, const string& path, InlineBlockPropEnv::Params::Param_t::Contract_t& input)
    {
      XMLReader inputtop(xml, path);

      read(inputtop, "num_vecs", input.num_vecs);
      read(inputtop, "block_size", input.block_size);
      read(inputtop, "t_sources", input.t_sources);
      read(inputtop, "decay_dir", input.decay_dir);
    }

    //! Propagator output
    void write(XMLWriter& xml, const string& path, const InlineBlockPropEnv::Params::Param_t::Contract_t& input)
    {
      push(xml, path);

      write(xml, "num_vecs",  input.num_vecs);
      write(xml, "block",     input.block_size);
      write(xml, "t_sources", input.t_sources);
      write(xml, "decay_dir", input.decay_dir);

      pop(xml);
    }


    //! Propagator input
    void read(XMLReader& xml, const string& path, InlineBlockPropEnv::Params::Param_t& input)
    {
      XMLReader inputtop(xml, path);

      read(inputtop, "Propagator", input.prop);
      read(inputtop, "Contractions", input.contract);
    }

    //! Propagator output
    void write(XMLWriter& xml, const string& path, const InlineBlockPropEnv::Params::Param_t& input)
    {
      push(xml, path);

      write(xml, "Propagator", input.prop);
      write(xml, "Contractions", input.contract);

      pop(xml);
    }


    //! Propagator input
    void read(XMLReader& xml, const string& path, InlineBlockPropEnv::Params& input)
    {
      InlineBlockPropEnv::Params tmp(xml, path);
      input = tmp;
    }

    //! Propagator output
    void write(XMLWriter& xml, const string& path, const InlineBlockPropEnv::Params& input)
    {
      push(xml, path);
    
      write(xml, "Param", input.param);

      std::istringstream is( input.map_obj_params.xml );
      XMLReader reader(is);
      xml << reader;

      write(xml, "NamedObject", input.named_obj);

      pop(xml);
    }
  } // namespace InlineBlockPropEnv 


  namespace InlineBlockPropEnv 
  {
    namespace
    {
      AbsInlineMeasurement* createMeasurement(XMLReader& xml_in, 
					      const std::string& path) 
      {
	return new InlineMeas(Params(xml_in, path));
      }

      //! Local registration flag
      bool registered = false;
    }
      
    const std::string name = "BLOCK_PROPAGATOR";

    //! Register all the factories
    bool registerAll() 
    {
      bool success = true; 
      if (! registered)
      {
	success &= WilsonTypeFermActsEnv::registerAll();
	success &= TheInlineMeasurementFactory::Instance().registerObject(name, createMeasurement);
	success &= MapObjectWilson4DEnv::registerKeyBlockPropLFAll();
	registered = true;
      }
      return success;
    }


    //----------------------------------------------------------------------------
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

	// Parameters for source construction
	read(paramtop, "Param", param);

	// Read in the output propagator/source configuration info
	read(paramtop, "NamedObject", named_obj);

	// Possible alternate XML file pattern
	if (paramtop.count("xml_file") != 0) 
	{
	  read(paramtop, "xml_file", xml_file);
	}

	// If possible, read an XML Group for the MapObject factory
	if ( paramtop.count("MapObject") == 0 ) {
	  // Default is an MapObjectMemory
	  
	  map_obj_params.xml = "<MapObject><MapObjType>MAP_OBJ_MEMORY</MapObjType></MapObject>";
	  map_obj_params.path = "MapObject";
	  map_obj_params.id = "MAP_OBJ_MEMORY";
	}
	else { 
	  // User Specified MapObject tags
	  map_obj_params = readXMLGroup(paramtop, 
					"MapObject", 
					"MapObjType");
	}

      }
      catch(const std::string& e) 
      {
	QDPIO::cerr << __func__ << ": Caught Exception reading XML: " << e << endl;
	QDP_abort(1);
      }
    }



    // Function call
    void 
    InlineMeas::operator()(unsigned long update_no,
			   XMLWriter& xml_out) 
    {
      // If xml file not empty, then use alternate
      if (params.xml_file != "")
      {
	string xml_file = makeXMLFileName(params.xml_file, update_no);

	push(xml_out, "BlockProp");
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
    InlineMeas::func(unsigned long update_no,
		     XMLWriter& xml_out) 
    {
      START_CODE();

      StopWatch snoop;
      snoop.reset();
      snoop.start();

      // Test and grab a reference to the gauge field
      multi1d<LatticeColorMatrix> u;
      XMLBufferWriter gauge_xml;
      try
      {
	u = TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(params.named_obj.gauge_id);
	TheNamedObjMap::Instance().get(params.named_obj.gauge_id).getRecordXML(gauge_xml);
      }
      catch( std::bad_cast ) 
      {
	QDPIO::cerr << name << ": caught dynamic cast error" << endl;
	QDP_abort(1);
      }
      catch (const string& e) 
      {
	QDPIO::cerr << name << ": map call failed: " << e << endl;
	QDP_abort(1);
      }

      push(xml_out, "BlockProp");
      write(xml_out, "update_no", update_no);

      QDPIO::cout << name << ": propagator calculation" << endl;

      proginfo(xml_out);    // Print out basic program info

      // Write out the input
      write(xml_out, "Input", params);

      // Write out the config header
      write(xml_out, "Config_info", gauge_xml);

      push(xml_out, "Output_version");
      write(xml_out, "out_version", 1);
      pop(xml_out);

      // Calculate some gauge invariant observables just for info.
      MesPlq(xml_out, "Observables", u);

      //
      // Read in the source along with relevant information.
      // 
      XMLReader source_file_xml, source_record_xml;

      QDPIO::cout << "Snarf the source from a named buffer" << endl;
      try
      {
	TheNamedObjMap::Instance().getData< SubsetVectors<LatticeColorVector> >(params.named_obj.colorvec_id);

	// Snarf the source info. This is will throw if the colorvec_id is not there
	TheNamedObjMap::Instance().get(params.named_obj.colorvec_id).getFileXML(source_file_xml);
	TheNamedObjMap::Instance().get(params.named_obj.colorvec_id).getRecordXML(source_record_xml);

	// Write out the source header
	write(xml_out, "Source_file_info", source_file_xml);
	write(xml_out, "Source_record_info", source_record_xml);
      }    
      catch (std::bad_cast)
      {
	QDPIO::cerr << name << ": caught dynamic cast error" << endl;
	QDP_abort(1);
      }
      catch (const string& e) 
      {
	QDPIO::cerr << name << ": error extracting source_header: " << e << endl;
	QDP_abort(1);
      }
      const SubsetVectors<LatticeColorVector>& eigen_source = 
	TheNamedObjMap::Instance().getData< SubsetVectors<LatticeColorVector> >(params.named_obj.colorvec_id);

      QDPIO::cout << "Source successfully read and parsed" << endl;

      //
      // Create the output files
      //
      try
      {
	std::istringstream  xml_s(params.map_obj_params.xml);
	XMLReader MapObjReader(xml_s);
	// Create the object as a handle. 
	// This bit will and up changing to a Factory invocation
	Handle< MapObject<KeyBlockProp_t, LatticeFermion> >  new_map_obj_handle(
	  TheMapObjKeyBlockPropFactory::Instance().createObject(params.map_obj_params.id,
								MapObjReader,
								params.map_obj_params.path) );

	// Create the entry
	TheNamedObjMap::Instance().create< Handle< MapObject<KeyBlockProp_t,LatticeFermion> > >(params.named_obj.prop_id);

	// Insert
	TheNamedObjMap::Instance().getData< Handle<MapObject<KeyBlockProp_t,LatticeFermion> > >(params.named_obj.prop_id) = new_map_obj_handle;


	//	TheNamedObjMap::Instance().create< MapObject<KeyBlockProp_t,LatticeFermion> >(params.named_obj.prop_id);
      }
      catch (std::bad_cast)
      {
	QDPIO::cerr << name << ": caught dynamic cast error" << endl;
	QDP_abort(1);
      }
      catch (const string& e) 
      {
	QDPIO::cerr << name << ": error creating prop: " << e << endl;
	QDP_abort(1);
      }

      // Cast should be valid now
      MapObject<KeyBlockProp_t,LatticeFermion>& map_obj =
	*(TheNamedObjMap::Instance().getData< Handle<MapObject<KeyBlockProp_t,LatticeFermion> > >(params.named_obj.prop_id));

      // Sanity check - write out the norm2 of the source in the Nd-1 direction
      // Use this for any possible verification
      {
	// Initialize the slow Fourier transform phases
	SftMom phases(0, true, Nd-1);

	multi1d< multi1d<Double> > source_corrs(eigen_source.getNumVectors());
	for(int m=0; m < source_corrs.size(); ++m)
	{
	  source_corrs[m] = sumMulti(localNorm2(eigen_source.getEvectors()[m]), phases.getSet());
	}

	push(xml_out, "Source_correlators");
	write(xml_out, "source_corrs", source_corrs);
	pop(xml_out);
      }

      // Another sanity check
      if (params.param.contract.num_vecs > eigen_source.getNumVectors())
      {
	QDPIO::cerr << __func__ << ": num_vecs= " << params.param.contract.num_vecs
		    << " is greater than the number of available colorvectors= "
		    << eigen_source.getNumVectors() << endl;
	QDP_abort(1);
      }


      // Total number of iterations
      int ncg_had = 0;

      //
      // Try the factories
      //
      try
      {
	StopWatch swatch;
	swatch.reset();
	QDPIO::cout << "Try the various factories" << endl;

	// Typedefs to save typing
	typedef LatticeFermion               T;
	typedef multi1d<LatticeColorMatrix>  P;
	typedef multi1d<LatticeColorMatrix>  Q;

	//
	// Initialize fermion action
	//
	std::istringstream  xml_s(params.param.prop.fermact.xml);
	XMLReader  fermacttop(xml_s);
	QDPIO::cout << "FermAct = " << params.param.prop.fermact.id << endl;

	// Generic Wilson-Type stuff
	Handle< FermionAction<T,P,Q> >
	  S_f(TheFermionActionFactory::Instance().createObject(params.param.prop.fermact.id,
							       fermacttop,
							       params.param.prop.fermact.path));

	Handle< FermState<T,P,Q> > state(S_f->createState(u));

	Handle< SystemSolver<LatticeFermion> > PP = S_f->qprop(state,
							       params.param.prop.invParam);
      
	QDPIO::cout << "Suitable factory found: compute all the quark props" << endl;
	swatch.start();

	//
	// Loop over the source color and spin, creating the source
	// and calling the relevant propagator routines.
	//
	const int num_vecs            = params.param.contract.num_vecs;
	const int decay_dir           = params.param.contract.decay_dir;
	const multi1d<int>& t_sources = params.param.contract.t_sources;

	// Initialize the slow Fourier transform phases
	SftMom phases(0, true, decay_dir);

	//Make the block Set
	Set blocks; 
	blocks.make(BlockFunc(decay_dir, params.param.contract.block_size));

	// Loop over each operator 
	map_obj.openWrite(); // prepare map obj for writing

	for(int tt=0; tt < t_sources.size(); ++tt)
	{
	  int t_source = t_sources[tt];
	  QDPIO::cout << "t_source = " << t_source << endl; 

	  // All the loops
	  for(int colorvec_source=0; colorvec_source < num_vecs; ++colorvec_source)
	  {
	    QDPIO::cout << "colorvec_source = " << colorvec_source << endl; 

	    // loop over blocks                                           
	    // Pull out a time-slice of the color vector source
	    LatticeColorVector vec_srce = zero;
	    vec_srce[phases.getSet()[t_source]] = eigen_source.getEvectors()[colorvec_source];

	    for(int b=0; b < blocks.numSubsets(); b++)
	    {
	      QDPIO::cout << "block = " << b << endl;
	      LatticeColorVector blk_vec = zero;
	      blk_vec[blocks[b]] = vec_srce;

	      for(int spin_source=0; spin_source < Ns; ++spin_source)
	      {
		QDPIO::cout << "spin_source = " << spin_source << endl; 

		// Insert a ColorVector into spin index spin_source
		// This only overwrites sections, so need to initialize first
		LatticeFermion chi = zero;
		CvToFerm(blk_vec, chi, spin_source);

		LatticeFermion quark_soln = zero;

		// Do the propagator inversion
		SystemSolverResults_t res = (*PP)(quark_soln, chi);
		ncg_had = res.n_count;

		KeyBlockProp_t key;
		key.t_source = t_source;
		key.color    = colorvec_source;
		key.spin     = spin_source;
		key.block    = b;
		
		map_obj.insert(key, quark_soln);
	      } // for spin_source
	    }//for block 
	  } // for colorvec_source
	} // for t_source
	map_obj.openRead();

	swatch.stop();
	QDPIO::cout << "Propagators computed: time= " 
		    << swatch.getTimeInSeconds() 
		    << " secs" << endl;
      }
      catch (const std::string& e){
	QDPIO::cout << name << ": caught exception around qprop: " << e << endl;
	QDP_abort(1);
      }
      
      push(xml_out,"Relaxation_Iterations");
      write(xml_out, "ncg_had", ncg_had);
      pop(xml_out);
      
      pop(xml_out);  // prop_colorvec
      
      // Write the meta-data for this operator
      {
	XMLBufferWriter file_xml;
	
	push(file_xml, "BlockProp");
	write(file_xml, "num_records", map_obj.size()); 
	write(file_xml, "Params", params.param);
	write(file_xml, "Config_info", gauge_xml);
	pop(file_xml);
	
	XMLBufferWriter record_xml;
	push(record_xml, "BlockProp");
	write(record_xml, "num_records", map_obj.size()); 
	pop(record_xml);
	
	// Write the propagator xml info
	TheNamedObjMap::Instance().get(params.named_obj.prop_id).setFileXML(file_xml);
	TheNamedObjMap::Instance().get(params.named_obj.prop_id).setRecordXML(record_xml);
      }
      
      snoop.stop();
      QDPIO::cout << name << ": total time = "
		  << snoop.getTimeInSeconds() 
		  << " secs" << endl;
      
      QDPIO::cout << name << ": ran successfully" << endl;
      
      END_CODE();
    } 
    
  }
  
} // namespace Chroma
