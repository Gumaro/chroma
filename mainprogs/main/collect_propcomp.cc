// $Id: collect_propcomp.cc,v 1.8 2004-11-17 15:23:00 bjoo Exp $
// $Log: collect_propcomp.cc,v $
// Revision 1.8  2004-11-17 15:23:00  bjoo
// t_su3 removed from make check. Throws stringified
//
// Revision 1.7  2004/09/08 14:41:36  edwards
// Ifdef'ed out old FermActHandle code to get to compile.
//
// Revision 1.6  2004/07/28 03:08:04  edwards
// Added START/END_CODE to all routines. Changed some to not pass an
// argument.
//
// Revision 1.5  2004/04/28 14:56:11  bjoo
// Tested propagator_comp and collect_propcomp
//
// Revision 1.3  2004/04/23 11:23:38  bjoo
// Added component based propagator (non multishift) and added Zolotarev5D operator to propagator and propagator_comp. Reworked propagator collection scripts
//
// Revision 1.2  2004/04/22 16:25:25  bjoo
// Added overlap_state_info Param struct and gauge startup convenience function. Tidyed up propagator zolo4d case
//
// Revision 1.1  2004/04/20 13:08:12  bjoo
// Added multi mass component based solves and propagator collection program
//
// Revision 1.3  2004/04/16 22:03:59  bjoo
// Added Zolo 4D test files and tidyed up
//
// Revision 1.2  2004/04/16 20:18:03  bjoo
// Zolo seems to work now
//
// Revision 1.1  2004/04/16 17:04:49  bjoo
// Added multi_propagator for Zolo4D multi massery. Seems to work even
//
// Revision 1.49  2004/04/15 14:43:25  bjoo
// Added generalised qprop_io FermAct reading
//
// Revision 1.48  2004/04/06 04:20:33  edwards
// Added SZINQIO support.
//
// Revision 1.47  2004/04/01 18:10:22  edwards
// Added support for non-relativistic quark props.
//
// Revision 1.46  2004/02/23 03:13:58  edwards
// Major overhaul of input/output model! Now using EXCLUSIVELY
// SciDAC propagator format for propagators. Now, Param part of input
// files directly matches source/sink/propagator/seqprop headers
// of propagators. All ``known'' input of a propagator is derived
// from its header(s) and used for subsequent calculations.
//
// Revision 1.45  2004/02/11 12:51:35  bjoo
// Stripped out Read() and Write()
//
// Revision 1.44  2004/02/07 04:51:58  edwards
// Removed SSE hack - pushed it down into the SSE code where it belongs.
//
// Revision 1.43  2004/02/06 22:31:00  edwards
// Put in sse hack for the short term.
//
// Revision 1.42  2004/02/06 17:39:05  edwards
// Added a flush to xml_out.
//
// Revision 1.41  2004/02/05 04:18:56  edwards
// Changed call of quarkProp4 to write to xml_out instead of xml buffer.
//
// Revision 1.40  2004/02/04 17:01:55  edwards
// Changed getSubset() to the now correct getSet().
//
// Revision 1.39  2004/01/31 23:22:01  edwards
// Added proginfo call.
//
// Revision 1.38  2004/01/30 21:35:22  kostas
// added calls to calculate mres for dwf
// 
/*! \file
 *  \brief Main code for propagator generation
 */

#include <iostream>
#include <cstdio>
#include <iomanip>
using namespace std;
#include "chroma.h"

using namespace QDP;



/*
 * Input 
 */
struct Prop_t
{
  string          source_file;
  string          prop_file;
  QDP_volfmt_t    prop_volfmt;
};


struct Component_t { 
  int color;
  int spin;
};

struct PropagatorComponent_input_t
{
  ChromaProp_t     param;
  Cfg_t            cfg;
  Prop_t           prop;
  multi1d<Component_t> components;
};


void read(XMLReader& xml, const string& path, Component_t &comp)
{
  XMLReader top(xml,path);
  try {
    read(top, "color", comp.color);
    read(top, "spin",  comp.spin);
  }
  catch( const string& e ) {
    QDPIO::cerr << "Caught Exception : " << e << endl;
    QDP_abort(1);
  }  
  if( comp.color < 0 || comp.color >= Nc ) { 
    QDPIO::cerr << "Component color >= Nc. color = " << comp.color << endl;
    QDP_abort(1);
  }

  if( comp.spin < 0 || comp.spin >= Ns ) { 
    QDPIO::cerr << "Component spin >= Ns.  spin = " << comp.spin << endl;
    QDP_abort(1);
  }
}

void write(XMLWriter& xml, const string& path, const Component_t &comp)
{
  
  push( xml, path );

  write(xml, "color", comp.color);
  write(xml, "spin",  comp.spin);
  
  pop( xml );
}


// Propagator parameters
void read(XMLReader& xml, const string& path, Prop_t& input)
{
  XMLReader inputtop(xml, path);

  read(inputtop, "source_file", input.source_file);
  read(inputtop, "prop_file", input.prop_file);
  read(inputtop, "prop_volfmt", input.prop_volfmt);  // singlefile or multifile
}


// Reader for input parameters
void read(XMLReader& xml, const string& path, PropagatorComponent_input_t& input)
{
  XMLReader inputtop(xml, path);

  // Read the input
  try
  {
    // The parameters holds the version number
    read(inputtop, "Param", input.param);

    // Read in the gauge configuration info
    read(inputtop, "Cfg", input.cfg);

    // Read in the source/propagator info
    read(inputtop, "Prop", input.prop);

    read(inputtop, "Components", input.components);
  }
  catch (const string& e) 
  {
    QDPIO::cerr << "Error reading data: " << e << endl;
    throw;
  }
}

/*!
 * Main program for propagator generation. 
 */

int main(int argc, char **argv)
{
  // Put the machine into a known state
  QDP_initialize(&argc, &argv);

  START_CODE();

  // Input parameter structure
  PropagatorComponent_input_t  input;

  // Instantiate xml reader for DATA
  XMLReader xml_in("DATA");

  // Read data
  try { 
    read(xml_in, "/propagatorComp", input);
  }
  catch ( const string& e ) { 
    QDPIO::cerr << "Caught exception " << e << endl;
    QDP_abort(1);
  }

  // Specify lattice size, shape, etc.
  Layout::setLattSize(input.param.nrow);
  Layout::create();

  QDPIO::cout << "propagatorComp" << endl;

  // Read in the configuration along with relevant information.
  multi1d<LatticeColorMatrix> u(Nd);
  XMLReader gauge_file_xml, gauge_xml;


 // Start up the gauge field
  gaugeStartup(gauge_file_xml, gauge_xml, u, input.cfg);

  // Read in the source along with relevant information.
  LatticePropagator quark_prop_source;
  XMLReader source_file_xml, source_record_xml;

  int t0;
  int j_decay;
  multi1d<int> boundary;
  bool make_sourceP = false;;
  bool seqsourceP = false;
  {
    // ONLY SciDAC mode is supported for propagators!!
    QDPIO::cout << "Attempt to read source" << endl;
    readQprop(source_file_xml, 
	      source_record_xml, quark_prop_source,
	      input.prop.source_file, QDPIO_SERIAL);
    QDPIO::cout << "Forward propagator successfully read" << endl;

    // Try to invert this record XML into a source struct
    try
    {
      // First identify what kind of source might be here
      if (source_record_xml.count("/MakeSource") != 0)
      {
	PropSource_t source_header;

	read(source_record_xml, "/MakeSource/PropSource", source_header);
	j_decay = source_header.j_decay;
	t0 = source_header.t_source[j_decay];
	boundary = input.param.boundary;
	make_sourceP = true;
      }
      else if (source_record_xml.count("/SequentialSource") != 0)
      {
	ChromaProp_t prop_header;
	PropSource_t source_header;
	SeqSource_t seqsource_header;

	read(source_record_xml, "/SequentialSource/SeqSource", seqsource_header);
	// Any source header will do for j_decay
	read(source_record_xml, "/SequentialSource/ForwardProps/elem[1]/ForwardProp", 
	     prop_header);
	read(source_record_xml, "/SequentialSource/ForwardProps/elem[1]/PropSource", 
	     source_header);
	j_decay = source_header.j_decay;
	t0 = seqsource_header.t_sink;
	boundary = prop_header.boundary;
	seqsourceP = true;
      }
      else
	throw std::string("No appropriate header found");
    }
    catch (const string& e) 
    {
      QDPIO::cerr << "Error extracting source_header: " << e << endl;
      QDP_abort(1);
    }
  }    

  // Sanity check
  if (seqsourceP)
  {
    for(int i=0; i < boundary.size(); ++i)
      if (boundary[i] != input.param.boundary[i])
      {
	QDPIO::cerr << "Incompatible boundary between input and seqsource" << endl;
	QDP_abort(1);
      }
  }


  // Instantiate XML writer for XMLDAT
  XMLFileWriter xml_out("XMLDAT");
  push(xml_out, "collectPropcomp");

  proginfo(xml_out);    // Print out basic program info

  // Write out the input
  write(xml_out, "Input", xml_in);

  // Write out the config header
  write(xml_out, "Config_info", gauge_xml);

  // Write out the source header
  write(xml_out, "Source_file_info", source_file_xml);
  write(xml_out, "Source_record_info", source_record_xml);

  push(xml_out, "Output_version");
  write(xml_out, "out_version", 1);
  pop(xml_out);

  xml_out.flush();


  // Check if the gauge field configuration is unitarized
  unitarityCheck(u);

  // Calculate some gauge invariant observables just for info.
  Double w_plaq, s_plaq, t_plaq, link;
  MesPlq(u, w_plaq, s_plaq, t_plaq, link);

  push(xml_out, "Observables");
  write(xml_out, "w_plaq", w_plaq);
  write(xml_out, "s_plaq", s_plaq);
  write(xml_out, "t_plaq", t_plaq);
  write(xml_out, "link", link);
  pop(xml_out);

  // Sanity check - write out the norm2 of the source in the Nd-1 direction
  // Use this for any possible verification
  {
    // Initialize the slow Fourier transform phases
    SftMom phases(0, true, Nd-1);

    multi1d<Double> source_corr = sumMulti(localNorm2(quark_prop_source), 
					   phases.getSet());

    push(xml_out, "Source_correlator");
    write(xml_out, "source_corr", source_corr);
    pop(xml_out);
  }

  xml_out.flush();

  // Loop over the source color and spin, creating the source
  // and calling the relevant propagator routines. The QDP
  // terminology is that a propagator is a matrix in color
  // and spin space
  //  
  LatticeFermion psi;
  LatticePropagator quark_prop;

  XMLReader file_xml_in;
  XMLReader record_xml_in;

  int max_spin;
  if( make_sourceP ) { 
    max_spin = Ns;
  }
  else if (seqsourceP ) { 
    if( input.param.nonRelProp ) {
      max_spin = Ns/2;
    }
    else {
      max_spin = Ns;
    }
  }

  for(int spin=0; spin < max_spin; spin++) { 
    for(int color=0; color < Nc; color++) {
      ostringstream filename ;
      filename << input.prop.prop_file << "_component_s" << spin
	       << "_c" << color ;
      
      QDPIO::cout << "Attempting to read " << filename.str() << endl;
      
      // Write the source
      readFermion(file_xml_in, record_xml_in, psi,
		  filename.str(), QDPIO_SERIAL);
      
      FermToProp(psi, quark_prop, color, spin);
    }
  }

  if( seqsourceP ) {
    if( input.param.nonRelProp ) { 
      for(int spin = max_spin; spin < Ns; spin++) {
	for(int color =0; color < Nc; color++) { 
	  
	  PropToFerm(quark_prop,psi, color, spin-max_spin);
	  
	  FermToProp((-psi), quark_prop, color, spin);
	}
      }
    }
  }

  SftMom phases(0, true, Nd-1);

  multi1d<Double> prop_corr = sumMulti(localNorm2(quark_prop), 
					 phases.getSet());
    
#if 0
  // Need to fix FermActHandle here
  push(xml_out, "Prop_correlator");
  write(xml_out, "Mass", input.param.FermActHandle->getMass());
  write(xml_out, "prop_corr", prop_corr);
  pop(xml_out);
#endif
  
  
  xml_out.flush();
  
  XMLBufferWriter file_xml;
  push(file_xml, "propagator");
  int id = 0;    // NEED TO FIX THIS - SOMETHING NON-TRIVIAL NEEDED
  write(file_xml, "id", id);
  pop(file_xml);
  
    
  
  XMLBufferWriter record_xml;
  if( make_sourceP) {
  
  // Jiggery pokery. Substitute the ChromaMultiProp_t with a 
  // ChromaProp. This is a pisser because of the FermActParams
  // THIS IS NOT TOTALLY KOSHER AS IT CHANGES THE MASS IN INPUT
  // PARAM as well. However, at this stage we have no further need
  // for input param.
  // I Will eventually write Copy Constructors.
    XMLReader xml_tmp(source_record_xml, "/MakeSource");
  
    push(record_xml, "Propagator");
    write(record_xml, "ForwardProp", input.param);
    record_xml << xml_tmp;
    pop(record_xml);
  }
  else if (seqsourceP) {
      XMLReader xml_tmp(source_record_xml, "/SequentialSource");

      push(record_xml, "SequentialProp");
      write(record_xml, "SeqProp", input.param);
      record_xml << xml_tmp;  // write out all the stuff under SequentialSource
      pop(record_xml);
    }


  QDPIO::cout << "Attempting to write " << input.prop.prop_file << endl;
  
  // Write the source
  writeQprop(file_xml, record_xml, quark_prop,
	     input.prop.prop_file, input.prop.prop_volfmt, QDPIO_SERIAL);


    
  pop(xml_out);  // propagator
  
  xml_out.close();
  xml_in.close();
  
  END_CODE();

  // Time to bolt
  QDP_finalize();
  
  exit(0);
}


