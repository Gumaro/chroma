// $Id: inline_grid_prop_matelem_w.cc,v 3.8 2009-03-05 04:01:07 edwards Exp $
/*! \file
 * \brief Compute the matrix element of  LatticeFermion*M^-1*LatticeFermion
 *
 * Propagator calculation
 */

#include "fermact.h"
#include "meas/inline/hadron/inline_grid_prop_matelem_w.h"
#include "meas/inline/hadron/inline_grid_prop_w.h"
#include "meas/inline/abs_inline_measurement_factory.h"
#include "meas/glue/mesplq.h"
#include "util/ferm/subset_vectors.h"
#include "util/ferm/map_obj.h"
#include "util/ferm/key_grid_prop.h"
#include "util/ferm/key_val_db.h"
#include "util/ft/sftmom.h"
#include "util/info/proginfo.h"
#include "meas/inline/make_xml_file.h"

#include "meas/sources/diluteGrid_source_const.h"

#include "meas/inline/io/named_objmap.h"

namespace Chroma 
{ 
  namespace InlineGridPropMatElemEnv 
  {
    //! Propagator input
    void read(XMLReader& xml, const string& path, InlineGridPropMatElemEnv::Params::NamedObject_t& input)
    {
      XMLReader inputtop(xml, path);

      read(inputtop, "gauge_id", input.gauge_id);
      read(inputtop, "prop_id", input.prop_id);
      read(inputtop, "prop_op_file", input.prop_op_file);
    }

    //! Propagator output
    void write(XMLWriter& xml, const string& path, const InlineGridPropMatElemEnv::Params::NamedObject_t& input)
    {
      push(xml, path);

      write(xml, "gauge_id", input.gauge_id);
      write(xml, "prop_id", input.prop_id);
      write(xml, "prop_op_file", input.prop_op_file);

      pop(xml);
    }


    //! Propagator input
    void read(XMLReader& xml, const string& path, InlineGridPropMatElemEnv::Params::Param_t& input)
    {
      XMLReader inputtop(xml, path);
      read(inputtop, "mass_label", input.mass_label);
    }

    //! Propagator output
    void write(XMLWriter& xml, const string& path, const InlineGridPropMatElemEnv::Params::Param_t& input)
    {
      push(xml, path);

      write(xml, "mass_label", input.mass_label);

      pop(xml);
    }


    //! Propagator input
    void read(XMLReader& xml, const string& path, InlineGridPropMatElemEnv::Params& input)
    {
      InlineGridPropMatElemEnv::Params tmp(xml, path);
      input = tmp;
    }

    //! Propagator output
    void write(XMLWriter& xml, const string& path, const InlineGridPropMatElemEnv::Params& input)
    {
      push(xml, path);
    
      write(xml, "Param", input.param);
      write(xml, "NamedObject", input.named_obj);

      pop(xml);
    }
  } // namespace InlineGridPropMatElemEnv 


  namespace InlineGridPropMatElemEnv 
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
      
    const std::string name = "GRID_PROP_MATELEM";

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


    //----------------------------------------------------------------------------
    //! Prop operator
    struct KeyGridPropElem_t
    {
      int                t_slice;       /*!< Propagator time slice */
      int                t_source;      /*!< Source time slice */
      //int                spin_src;      /*!< Source spin index */
      //int                spin_snk;      /*!< Sink spin index */
      //int                color_src;      /*!< Source color index */
      //int                color_snk;      /*!< Sink color index */
      std::string        mass_label;    /*!< A mass label */
    };


    //! Prop operator
    class ValGridPropElem_t
    {
    private:
      void allocate(){
	m.resize(Ns,Ns);
        for(int s(0);s<Ns;s++)
          for(int ss(0);ss<Ns;ss++)
	    m(s,ss).resize(Nc,Nc);
      }
    public:
      int Ngrids ;
      multi2d< multi2d< multi2d<ComplexD> > > m;   /*!< spin color grid matels */
      
      void setGrids(int ng){
	Ngrids=ng ;
	for(int s(0);s<Ns;s++)
          for(int ss(0);ss<Ns;ss++)
	    for(int c(0);c<Nc;c++)
	      for(int cc(0);cc<Nc;cc++)
		m(s,ss)(c,cc).resize(Ngrids,Ngrids);
      }
      
      ValGridPropElem_t():Ngrids(0){
	allocate();
      }
      ValGridPropElem_t(int ng){
	allocate();
	setGrids(ng);
      }
    };


    //----------------------------------------------------------------------------
    //! Holds key and value as temporaries
    struct KeyValGridPropElem_t{
      SerialDBKey<KeyGridPropElem_t>  key;
      SerialDBData<ValGridPropElem_t> val;	
    };

    //----------------------------------------------------------------------------
    //! GridPropElem reader
    void read(BinaryReader& bin, KeyGridPropElem_t& param)
    {
      read(bin, param.t_slice);
      read(bin, param.t_source);
      //read(bin, param.spin_src);
      //read(bin, param.spin_snk);
      read(bin, param.mass_label, 32);
    }

    //! GridPropElem write
    void write(BinaryWriter& bin, const KeyGridPropElem_t& param)
    {
      write(bin, param.t_slice);
      write(bin, param.t_source);
      //write(bin, param.spin_src);
      //write(bin, param.spin_snk);
      write(bin, param.mass_label);
    }

    //! GridPropElem reader
    void read(XMLReader& xml, const std::string& path, KeyGridPropElem_t& param)
    {
      XMLReader paramtop(xml, path);
    
      read(paramtop, "t_slice", param.t_slice);
      read(paramtop, "t_source", param.t_source);
      //read(paramtop, "spin_src", param.spin_src);
      //read(paramtop, "spin_snk", param.spin_snk);
      read(paramtop, "mass_label", param.mass_label);
    }

    //! GridPropElem writer
    void write(XMLWriter& xml, const std::string& path, const KeyGridPropElem_t& param)
    {
      push(xml, path);

      write(xml, "t_slice", param.t_slice);
      write(xml, "t_source", param.t_source);
      //write(xml, "spin_src", param.spin_src);
      //write(xml, "spin_snk", param.spin_snk);
      write(xml, "mass_label", param.mass_label);

      pop(xml);
    }


    //----------------------------------------------------------------------------
    //! GridPropElem reader
    void read(BinaryReader& bin, ValGridPropElem_t& param)
    {

      int Ng;
      int mNc;
      int mNs;
      read(bin, mNs);    // the size is always written, even if 0
      if(mNs!=Ns){
	QDPIO::cerr << __func__ << ": Matrix N spins is not " << Ns << endl;
        QDP_abort(1);
      }
      read(bin, mNc);    // the size is always written, even if 0
      if(mNc!=Nc){
	QDPIO::cerr << __func__ << ": Matrix N colors is not " << Nc << endl;
        QDP_abort(1);
      }
      read(bin, Ng ); 
      param.setGrids(Ng);
      for(int s(0);s<Ns;s++)
	for(int ss(0);ss<Ns;ss++)
	  for(int c(0);c<Nc;c++)
	    for(int cc(0);cc<Nc;cc++)
	      for(int g(0);g<Ng;g++)
		for(int gg(0);gg<Ng;gg++)
		  read(bin, param.m(ss,s)(cc,c)(gg,g));
    }

    //! GridPropElem write
    void write(BinaryWriter& bin, const ValGridPropElem_t& param)
    {
      write(bin, Ns);    // always write the size
      write(bin, Nc);    // always write the size
      write(bin, param.Ngrids);
      for(int s(0);s<Ns;s++)
        for(int ss(0);ss<Ns;ss++)
          for(int c(0);c<Nc;c++)
            for(int cc(0);cc<Nc;cc++)
	      for(int g(0);g<param.Ngrids;g++)
		for(int gg(0);gg<param.Ngrids;gg++)
                  write(bin, param.m(ss,s)(cc,c)(gg,g));
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

	push(xml_out, "GridPropMatElem");
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

      push(xml_out, "GridPropMatElem");
      write(xml_out, "update_no", update_no);

      QDPIO::cout <<name<< ": grid propagator matrix element calculation" << endl;

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

      XMLReader prop_file_xml, prop_record_xml;

      QDPIO::cout << "Check for the prop map" << endl;
      try
      {
	TheNamedObjMap::Instance().getData< MapObject<KeyGridProp_t,LatticeFermion> >(params.named_obj.prop_id);

	//This is will throw if the prop_id is not there
	TheNamedObjMap::Instance().get(params.named_obj.prop_id).getFileXML(prop_file_xml);
	TheNamedObjMap::Instance().get(params.named_obj.prop_id).getRecordXML(prop_record_xml);

	// Write out the source header
	write(xml_out, "Prop_file_info", prop_file_xml);
	write(xml_out, "Prop_record_info", prop_record_xml);
      }    
      catch (std::bad_cast)
      {
	QDPIO::cerr << name << ": caught dynamic cast error" << endl;
	QDP_abort(1);
      }
      catch (const string& e) 
      {
	QDPIO::cerr << name << ": error extracting source_header or prop map: " << e << endl;
	QDP_abort(1);
      }
      // Cast should be valid now
      const MapObject<KeyGridProp_t,LatticeFermion>& map_obj =
	TheNamedObjMap::Instance().getData< MapObject<KeyGridProp_t,LatticeFermion> >(params.named_obj.prop_id);

      QDPIO::cout << "Prop map successfully found and parsed" << endl;

      //Hunt for the source info 
      DiluteGridQuarkSourceConstEnv::Params srcParams ;
      //DiluteGridQuarkSourceConstEnv::Params snkParams ;
      InlineGridPropEnv::Params::Param_t::Sources_t src_t;
      {
	
	//XMLReader prop(prop_file_xml,"/GridProp/Params/Propagator");
	try{
	  XMLReader src(prop_file_xml, "/GridProp/Params");
	  InlineGridPropEnv::read(src,"Sources", src_t);
	  srcParams.j_decay = src_t.decay_dir ;
	  srcParams.spatial_mask_size = src_t.spatial_mask_size ;
	  if(src_t.smear){
	    srcParams.smear = true ;
	    srcParams.smr = src_t.smr ;
	    srcParams.displace = src_t.displace ;
	    srcParams.link_smear = src_t.link_smear ;
	  }
	  
	}
	catch(const string& e){
	  QDPIO::cerr << name << ": caught GridProp source parsing error: ";
	  QDPIO::cerr << e << endl;
	  QDP_abort(1);
	}
	
      }

      //snkParams=srcParams ;

      //
      // DB storage
      //
      BinaryStoreDB<SerialDBKey<KeyGridPropElem_t>, SerialDBData<ValGridPropElem_t> > qdp_db;

      // Open the file, and write the meta-data and the binary for this operator
      {
	XMLBufferWriter file_xml;

	push(file_xml, "DBMetaData");
	write(file_xml, "id", string("gridPropElemOp"));
	write(file_xml, "lattSize", QDP::Layout::lattSize());
	write(file_xml, "numGrids", params.param.Ngrids);
	write(file_xml, "decay_dir", params.param.decay_dir);
	write(file_xml, "Params", params.param);
	write(file_xml, "Config_info", gauge_xml);
	pop(file_xml);

	std::string file_str(file_xml.str());
	qdp_db.setMaxUserInfoLen(file_str.size());

	qdp_db.open(params.named_obj.prop_op_file, O_RDWR | O_CREAT, 0664);

	qdp_db.insertUserdata(file_str);
      }


      // Create grid props
      try{
	StopWatch swatch;
	swatch.reset();
	swatch.start();
	
	SftMom phases(0, true, src_t.decay_dir);
	//
	// Loop over t  color and spin, creating the sources
	//  srcParama.spin = -1 
	//  srcParams.t_source = -1 
	// and have the DiluteGridSource do all spins and time together
	// i.e. no spin time dilution, then pick the right spin and time 
	// when contractions are done
	// this will cut memory by 4 and computation by 4*Nt
	// seems the way to go. Grid source is now modified...
	StopWatch roloi;
	roloi.reset();
	roloi.start();
	multi2d<LatticeFermion> snk(Nc,src_t.spatial_masks.size()) ;
	srcParams.spin = -1 ;// do all spins at once
	srcParams.t_source = -1 ; // do all time slices at once
	for(int c(0);c<Nc;c++){// loop over colors
	  srcParams.color = c;
	  for(int g(0);g<src_t.spatial_masks.size();g++){
	    srcParams.spatial_mask =src_t.spatial_masks[g];
	    QDPIO::cout << name << ": Doing color " << c 
			<< " and  grid " << g<< " ( of " 
			<< src_t.spatial_masks.size() << ")"<<endl;
	    DiluteGridQuarkSourceConstEnv::SourceConst<LatticeFermion> GridSrc(srcParams);
	    snk(c,g) = GridSrc(u);
	    
	  }//g
	}//c
	roloi.stop();
	QDPIO::cout << "Sources constructed: time= "
                    << roloi.getTimeInSeconds()
                    << " secs" << endl;

	for(int t0(0);t0<src_t.t_sources.size();t0++){//loop over source timeslices
	  QDPIO::cout << name << ": Doing source timeslice " 
		      << src_t.t_sources[t0] << endl ;
	  KeyGridProp_t key;
	  key.t_source  = src_t.t_sources[t0] ;

	  // Invert the time - make it an independent key
	  multi1d<KeyValGridPropElem_t> buf(phases.numSubsets());
	  for(int t(0); t<phases.numSubsets(); t++){
	    buf[t].key.key().t_slice      = t;    
	    buf[t].key.key().t_source     = src_t.t_sources[t0] ; 
	    buf[t].key.key().mass_label   = params.param.mass_label; 
	    buf[t].val.data().setGrids(src_t.spatial_masks.size()); 
	  }                

          for(int src_s(0);src_s<Ns;src_s++){// loop over spins
	    key.spin      = src_s ;
	    for(int src_c(0);src_c<Nc;src_c++){// loop over colors
	      key.color     = src_c ;
	      //loop over grids
	      for(int src_g(0);src_g<src_t.spatial_masks.size();src_g++){
		key.grid      = src_g ;
		for(int snk_s(0);snk_s<Ns;snk_s++){// loop over spins
		  for(int snk_c(0);snk_c<Nc;snk_c++){// loop over colors
		    //loop over grids
		    for(int snk_g(0);snk_g<src_t.spatial_masks.size();snk_g++){
		      LatticeFermion snk_vec = zero ;
		      pokeSpin(snk_vec, 
			       peekSpin(snk(snk_c,snk_g),snk_s), snk_s) ;
		      multi1d<ComplexD> 
		      hsum(sumMulti(localInnerProduct(snk_vec,map_obj[key]),
				    phases.getSet()));
		      for(int t(0); t<hsum.size(); t++)
			buf[t].val.data().m(snk_s,src_s)(snk_c,src_c)(snk_g,src_g)=hsum[t];  
		    }//g snk
		  }//c snk
		}//s snk
	      }// g src
	    }//c src
	  }//s src
	  for(int t(0); t<phases.numSubsets(); t++)
	    qdp_db.insert(buf[t].key, buf[t].val);   
	}//t0
	swatch.stop();
	QDPIO::cout << "Matelem computed: time= " 
		    << swatch.getTimeInSeconds() 
		    << " secs" << endl;
      }
      catch (const std::string& e){
	QDPIO::cout << name << ": caught exception around matelem: " << e << endl;
	QDP_abort(1);
      }
     
      pop(xml_out);  // grid_prop_matelem

      snoop.stop();
      QDPIO::cout << name << ": total time = "
		  << snoop.getTimeInSeconds() 
		  << " secs" << endl;

      QDPIO::cout << name << ": ran successfully" << endl;

      END_CODE();
    } 

  }

} // namespace Chroma
