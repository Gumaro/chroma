/*
 *  $Id: hypsmear.cc,v 1.2 2003-10-09 20:32:37 edwards Exp $
 *
 *  This is the top-level routine for HYP smearing.
 *  It is a wrapper for Urs' and Robert's implmenetation of the HYP
 *  smearing algorithm
 */


#include <iostream>
#include <cstdio>

#define MAIN

#include "chroma.h"


enum CfgType {
  CFG_TYPE_MILC = 0,
  CFG_TYPE_NERSC,
  CFG_TYPE_SCIDAC,
  CFG_TYPE_SZIN,
  CFG_TYPE_UNKNOWN
} ;

using namespace QDP;

/*
 *  This is the reader for the input parameters
 */

/*
 * Input 
 */

struct IO_version_t
{
  int version;
};

// Parameters which must be determined from the XML input
// and written to the XML output
struct Param_t
{
  CfgType  cfg_type_in;       // storage order for stored gauge configuration
  CfgType  cfg_type_out;

  Real alpha1;			// Smearing parameters
  Real alpha2;
  Real alpha3;


  multi1d<int> nrow;		// Lattice dimension
  int j_decay;			// Direction corresponding to time
  /*
   *  Now some various rules for truncating the configuration
   */

  int trunc;			// Whether to truncate the output
  int t_start;			// Starting time slice
  int t_end;			// Ending time slice
};

struct Cfg_t
{
  string       cfg_file;
};

struct Hyp_t
{
  string hyp_file;
};

struct Hypsmear_input_t
{
  IO_version_t     io_version;
  Param_t          param;
  Cfg_t            cfg;
  Hyp_t            hyp;
};



void read(XMLReader& xml, const string& path, Hypsmear_input_t& input)
{
  XMLReader inputtop(xml, path);


  // First, read the input parameter version.  Then, if this version
  // includes 'Nc' and 'Nd', verify they agree with values compiled
  // into QDP++

  // Read in the IO_version
  try
  {
    read(inputtop, "IO_version/version", input.io_version.version);
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
    case 1 :
      /**************************************************************************/
      break;

    default :
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

    QDPIO::cout << " HYPSMEAR: HYP smearing of gauge config" << endl;

    {
      string cfg_type_str;

      // First the input cfg type


      read(paramtop, "cfg_type_in", cfg_type_str);
      if (cfg_type_str == "SZIN") {
	input.param.cfg_type_in = CFG_TYPE_SZIN;
      } 
      else if (cfg_type_str == "NERSC"){
	input.param.cfg_type_in = CFG_TYPE_NERSC;
      }
      else{
	input.param.cfg_type_in = CFG_TYPE_UNKNOWN;
      }


      read(paramtop, "cfg_type_out", cfg_type_str);
      if (cfg_type_str == "SZIN") {
	input.param.cfg_type_out = CFG_TYPE_SZIN;
      } else {
	input.param.cfg_type_out = CFG_TYPE_UNKNOWN;
      }

      // Now the output cfg type

    }
    
    read(paramtop, "alpha1", input.param.alpha1);
    read(paramtop, "alpha2", input.param.alpha2);
    read(paramtop, "alpha3", input.param.alpha3);

    read(paramtop, "nrow", input.param.nrow);

    /*
     *  Now information about whether to truncate the configuration
     */

    read(paramtop, "trunc", input.param.trunc);
    switch(input.param.trunc){
    case 1:
      read(paramtop, "t_start", input.param.t_start);
      read(paramtop, "t_end", input.param.t_end);
      read(paramtop, "j_decay", input.param.j_decay);
      break;
    default:
      break;
    }

  }
  catch (const string& e) 
  {
    QDPIO::cerr << "Error reading data: " << e << endl;
    throw;
  }



  // Read in the gauge configuration file name
  try
  {
    read(inputtop, "Cfg/cfg_file",input.cfg.cfg_file);
  }
  catch (const string& e) 
  {
    QDPIO::cerr << "Error reading data: " << e << endl;
    throw;
  }

  // Read in the hyp configuration file name
  try
  {
    read(inputtop, "Hyp/hyp_file",input.hyp.hyp_file);
  }
  catch (const string& e) 
  {
    QDPIO::cerr << "Error reading data: " << e << endl;
    throw;
  }
}

int main(int argc, char *argv[])
{
  // Put the machine into a known state
  QDP_initialize(&argc, &argv);

  // Parameter structure for the input

  Hypsmear_input_t input;


  // Instantiate xml reader for DATA
  XMLReader xml_in("DATA");

  // Read data
  read(xml_in, "/hypsmear", input);

  Layout::setLattSize(input.param.nrow);
  Layout::create();

  // Read in the configuration along with relevant information.
  multi1d<LatticeColorMatrix> u(Nd);
  XMLReader gauge_xml;

  switch (input.param.cfg_type_in) 
  {
  case CFG_TYPE_SZIN :
    printf("About to read szin gauge\n");
    readSzin(gauge_xml, u, input.cfg.cfg_file);
    break;
  case CFG_TYPE_NERSC:
    printf("About to read nersc gauge\n");
    readArchiv(gauge_xml, u, input.cfg.cfg_file);
    break;
  default :
    QDP_error_exit("Configuration type is unsupported.");
  }

  // Instantiate XML writer for XMLDAT
  XMLFileWriter xml_out("XMLDAT");
  push(xml_out, "propagator");

  // Write out the input
  write(xml_out, "Input", xml_in);

  // Write out the config header
  write(xml_out, "Config_info", gauge_xml);

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


  // Now hyp smear
  multi1d<LatticeColorMatrix> u_hyp(Nd);

  Real BlkAccu = 1.0e-5;
  int BlkMax = 100;


  Hyp_Smear(u, u_hyp, 
	    input.param.alpha1, input.param.alpha2, input.param.alpha3, 
	    BlkAccu, BlkMax);

  // Calculate some gauge invariant observables just for info.

  MesPlq(u_hyp, w_plaq, s_plaq, t_plaq, link);

  push(xml_out, "HYP_observables");
  Write(xml_out, w_plaq);
  Write(xml_out, s_plaq);
  Write(xml_out, t_plaq);
  Write(xml_out, link);
  pop(xml_out);

  // Now write the configuration to disk

  SzinGauge_t szin_out;

  switch (input.param.cfg_type_out) 
  {
  case CFG_TYPE_SZIN :
    szinGaugeInit(szin_out);

    switch(input.param.trunc){
    case 1:
      writeSzinTrunc(szin_out, u_hyp, input.param.j_decay,
		     input.param.t_start, input.param.t_end,
		     input.hyp.hyp_file);
    break;
    default:
      writeSzin(szin_out, u_hyp,
		input.hyp.hyp_file);
      break;
    }
    break;
  default :
    QDP_error_exit("Configuration type is unsupported.");
  }




  // Time to bolt
  QDP_finalize();

  exit(0);
}
