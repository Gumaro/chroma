// $Id: propagator.cc,v 1.31 2004-01-07 04:44:26 edwards Exp $
/*! \file
 *  \brief Main code for propagator generation
 */

#include <iostream>
#include <cstdio>

#include "chroma.h"

using namespace QDP;


/*
 * Input 
 */


// Parameters which must be determined from the XML input
// and written to the XML output
struct Param_t
{
  FermType        FermTypeP;
  FermActType     FermAct;
  Real            Mass;       // quark mass (bare units)
 
  ChiralParam_t   chiralParam;

  CfgType         cfg_type;   // storage order for stored gauge configuration
  PropType        prop_type;  // storage order for stored propagator

  InvertParam_t   invParam;   // Inverter parameters

  multi1d<int> nrow;
  multi1d<int> boundary;
  multi1d<int> t_srce;
};

struct Prop_t
{
  string       source_file;
  string       prop_file;
};

struct Propagator_input_t
{
  IO_version_t     io_version;
  Param_t          param;
  Cfg_t            cfg;
  Prop_t           prop;
};


//
void read(XMLReader& xml, const string& path, Prop_t& input)
{
  XMLReader inputtop(xml, path);

  read(inputtop, "source_file", input.source_file);
  read(inputtop, "prop_file", input.prop_file);
}



// Reader for input parameters
void read(XMLReader& xml, const string& path, Propagator_input_t& input)
{
  XMLReader inputtop(xml, path);


  // First, read the input parameter version.  Then, if this version
  // includes 'Nc' and 'Nd', verify they agree with values compiled
  // into QDP++

  // Read in the IO_version
  try
  {
    read(inputtop, "IO_version", input.io_version);
  }
  catch (const string& e) 
  {
    QDPIO::cerr << "Error reading data: " << e << endl;
    throw;
  }


  // Currently, in the supported IO versions, there is only a small difference
  // in the inputs. So, to make code simpler, extract the common bits 

  // Read the uncommon bits first
  try
  {
    XMLReader paramtop(inputtop, "param"); // push into 'param' group

    switch (input.io_version.version) 
    {
      /**************************************************************************/
    case 2:
      /**************************************************************************/
      break;

    default:
      /**************************************************************************/

      QDPIO::cerr << "Input parameter version " << input.io_version.version << " unsupported." << endl;
      QDP_abort(1);
    }
  }
  catch (const string& e) 
  {
    QDPIO::cerr << "Error reading data: " << e << endl;
    throw;
  }


  // Read the common bits
  try 
  {
    XMLReader paramtop(inputtop, "param"); // push into 'param' group

    read(paramtop, "FermTypeP", input.param.FermTypeP);
    read(paramtop, "FermAct", input.param.FermAct);

    if (paramtop.count("Mass") != 0)
    {
      read(paramtop, "Mass", input.param.Mass);

      if (paramtop.count("Kappa") != 0)
      {
	QDPIO::cerr << "Error: found both a Kappa and a Mass tag" << endl;
	QDP_abort(1);
      }
    }
    else if (paramtop.count("Kappa") != 0)
    {
      Real Kappa;
      read(paramtop, "Kappa", Kappa);

      input.param.Mass = kappaToMass(Kappa);    // Convert Kappa to Mass
    }
    else
    {
      QDPIO::cerr << "Error: neither Mass or Kappa found" << endl;
      QDP_abort(1);
    }    

#if 0
    for (int i=0; i < input.param.Mass.size(); ++i) {
      if (toBool(input.param.Mass[i] < 0.0)) {
	QDPIO::cerr << "Unreasonable value for Mass." << endl;
	QDPIO::cerr << "  Mass[" << i << "] = " << input.param.Mass[i] << endl;
	QDP_abort(1);
      } else {
	QDPIO::cout << " Spectroscopy Mass: " << input.param.Mass[i] << endl;
      }
    }
#endif

    if (paramtop.count("ChiralParam") != 0)
      read(paramtop, "ChiralParam", input.param.chiralParam);

    read(paramtop, "cfg_type", input.param.cfg_type);
    read(paramtop, "prop_type", input.param.prop_type);

    read(paramtop, "InvertParam", input.param.invParam);

    read(paramtop, "nrow", input.param.nrow);
    read(paramtop, "boundary", input.param.boundary);
    read(paramtop, "t_srce", input.param.t_srce);
  }
  catch (const string& e) 
  {
    QDPIO::cerr << "Error reading data: " << e << endl;
    throw;
  }


  // Read in the gauge configuration file name
  try
  {
    read(inputtop, "Cfg", input.cfg);
    read(inputtop, "Prop", input.prop);
  }
  catch (const string& e) 
  {
    QDPIO::cerr << "Error reading data: " << e << endl;
    throw;
  }
}



//! Propagator generation
/*! \defgroup propagator Propagator generation
 *  \ingroup main
 *
 * Main program for propagator generation. 
 */

int main(int argc, char **argv)
{
  // Put the machine into a known state
  QDP_initialize(&argc, &argv);

  // Input parameter structure
  Propagator_input_t  input;

  // Instantiate xml reader for DATA
  XMLReader xml_in("DATA");

  // Read data
  read(xml_in, "/propagator", input);

  // Specify lattice size, shape, etc.
  Layout::setLattSize(input.param.nrow);
  Layout::create();

  // Read in the configuration along with relevant information.
  multi1d<LatticeColorMatrix> u(Nd);
  XMLReader gauge_xml;

  switch (input.param.cfg_type) 
  {
  case CFG_TYPE_SZIN :
    readSzin(gauge_xml, u, input.cfg.cfg_file);
    break;
  default :
    QDP_error_exit("Configuration type is unsupported.");
  }


  // Read in the source along with relevant information.
  LatticePropagator quark_prop_source;
  XMLReader source_xml;

  switch (input.param.prop_type) 
  {
  case PROP_TYPE_SZIN :
//    readSzinQprop(source_xml, quark_prop_source, input.prop.source_file);
    quark_prop_source = 1;
    break;
  default :
    QDP_error_exit("Propagator type is unsupported.");
  }


  // Instantiate XML writer for XMLDAT
  XMLFileWriter xml_out("XMLDAT");
  push(xml_out, "propagator");

  // Write out the input
  write(xml_out, "Input", xml_in);

  // Write out the config header
  write(xml_out, "Config_info", gauge_xml);

  // Write out the source header
  write(xml_out, "Source_info", source_xml);

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
  Write(xml_out, w_plaq);
  Write(xml_out, s_plaq);
  Write(xml_out, t_plaq);
  Write(xml_out, link);
  pop(xml_out);

  xml_out.flush();

  /*
   * Construct fermionic BC. Need one for LatticeFermion and multi1d<LatticeFermion>
   * Note, the handle is on an ABSTRACT type
   */
  Handle< FermBC<LatticeFermion> >  fbc(new SimpleFermBC<LatticeFermion>(input.param.boundary));
  Handle< FermBC<multi1d<LatticeFermion> > >  fbc_a(new SimpleFermBC<multi1d<LatticeFermion> >(input.param.boundary));

  //
  // Loop over the source color and spin, creating the source
  // and calling the relevant propagator routines. The QDP
  // terminology is that a propagator is a matrix in color
  // and spin space
  //
  LatticePropagator quark_propagator;
  XMLBufferWriter xml_buf;
  int ncg_had;

  //
  // Initialize fermion action
  //
  switch (input.param.FermAct)
  {
  case FERM_ACT_WILSON:
  {
    QDPIO::cout << "FERM_ACT_WILSON" << endl;

    EvenOddPrecWilsonFermAct S_f(fbc,input.param.Mass);
    Handle<const ConnectState> state(S_f.createState(u));  // uses phase-multiplied u-fields

    quarkProp4(quark_propagator, xml_buf, quark_prop_source,
  	       S_f, state, 
	       input.param.invParam.invType, 
	       input.param.invParam.RsdCG, 
	       input.param.invParam.MaxCG, 
	       ncg_had);
  }
  break;

  case FERM_ACT_UNPRECONDITIONED_WILSON:
  {
    QDPIO::cout << "FERM_ACT_UNPRECONDITIONED_WILSON" << endl;

    UnprecWilsonFermAct S_f(fbc,input.param.Mass);
    Handle<const ConnectState> state(S_f.createState(u));  // uses phase-multiplied u-fields

    quarkProp4(quark_propagator, xml_buf, quark_prop_source,
  	       S_f, state, 
	       input.param.invParam.invType, 
	       input.param.invParam.RsdCG, 
	       input.param.invParam.MaxCG, 
	       ncg_had);
  }
  break;

  case FERM_ACT_DWF:
  {
    QDPIO::cout << "FERM_ACT_DWF" << endl;

    EvenOddPrecDWFermActArray S_f(fbc_a,
				  input.param.chiralParam.OverMass, 
				  input.param.Mass, 
				  input.param.chiralParam.N5);
    Handle<const ConnectState> state(S_f.createState(u));  // uses phase-multiplied u-fields

    quarkProp4(quark_propagator, xml_buf, quark_prop_source,
	       S_f, state, 
	       input.param.invParam.invType, 
	       input.param.invParam.RsdCG, 
	       input.param.invParam.MaxCG, 
	       ncg_had);
  }
  break;

  case FERM_ACT_UNPRECONDITIONED_DWF:
  {
    QDPIO::cout << "FERM_ACT_UNPRECONDITONED_DWF" << endl;

    UnprecDWFermActArray S_f(fbc_a,
			     input.param.chiralParam.OverMass, 
			     input.param.Mass, 
			     input.param.chiralParam.N5);
    Handle<const ConnectState> state(S_f.createState(u));  // uses phase-multiplied u-fields

    quarkProp4(quark_propagator, xml_buf, quark_prop_source,
  	       S_f, state, 
	       input.param.invParam.invType, 
	       input.param.invParam.RsdCG, 
	       input.param.invParam.MaxCG, 
	       ncg_had);
  }
  break;

//  case FERM_ACT_INTERNAL_UNPRECONDITIONED_DWF;
//  UnprecDWFermAct S_f(fbc_a, OverMass, Mass);

  default:
    QDPIO::cerr << "Unsupported fermion action" << endl;
    QDP_abort(1);
  }

  xml_out << xml_buf;   // write-out any output from the quarkProp4 routine

  // Debugging aid - write out the pion propagator in the Nd-1 direction
  {
    // Initialize the slow Fourier transform phases
    SftMom phases(0, true, Nd-1);

    /* The nonconserved vector current first */
    multi1d<Double> pion_corr = sumMulti(localNorm2(quark_propagator), 
					 phases.getSubset());

    push(xml_out, "Pion_correlator");
    Write(xml_out, pion_corr);
    pop(xml_out);
  }

  // Instantiate XML buffer to make the propagator header
  XMLBufferWriter prop_xml;
  push(prop_xml, "propagator");

  // Write out the input
  write(prop_xml, "Input", xml_in);

  // Write out the config header
  write(prop_xml, "Config_info", gauge_xml);

  // Write out the source header
  write(prop_xml, "Source_info", source_xml);

  pop(prop_xml);


  // Save the propagator
  switch (input.param.prop_type) 
  {
  case PROP_TYPE_SZIN:
    writeSzinQprop(quark_propagator, input.prop.prop_file, input.param.Mass);
    break;

//  case PROP_TYPE_SCIDAC:
//    writeQprop(prop_xml, quark_propagator, input.prop.prop_file);
//    break;

  default :
    QDP_error_exit("Propagator type is unsupported.");
  }


  xml_out.close();
  xml_in.close();

  // Time to bolt
  QDP_finalize();

  exit(0);
}
