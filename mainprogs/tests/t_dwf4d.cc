// $Id: t_dwf4d.cc,v 1.3 2004-11-16 06:09:10 bjoo Exp $

#include <iostream>
#include <cstdio>

#include "chroma.h"

#include "qdp_util.h"

using namespace QDP;
using namespace Chroma;


//! To insure linking of code, place the registered code flags here
/*! This is the bit of code that dictates what fermacts are in use */
bool linkage_hack()
{
  bool foo = true;

  // 4D actions
  foo &= EvenOddPrecWilsonFermActEnv::registered;
  foo &= UnprecWilsonFermActEnv::registered;
  foo &= OvlapPartFrac4DFermActEnv::registered;
  // 5D actions
  foo &= EvenOddPrecDWFermActArrayEnv::registered;
  foo &= UnprecDWFermActArrayEnv::registered;
  foo &= EvenOddPrecNEFFermActArrayEnv::registered;
  foo &= UnprecNEFFermActArrayEnv::registered;
  foo &= UnprecOvlapContFrac5DFermActArrayEnv::registered;
  foo &= EvenOddPrecOvlapContFrac5DFermActArrayEnv::registered;
  foo &= UnprecOvDWFermActArrayEnv::registered;
  foo &= EvenOddPrecOvDWFermActArrayEnv::registered;
  foo &= UnprecOvExtFermActArrayEnv::registered;
  foo &= UnprecZoloNEFFermActArrayEnv::registered;
  foo &= EvenOddPrecZoloNEFFermActArrayEnv::registered;
  foo &= UnprecDWFTransfFermActEnv::registered;

  // Test stuff
#if defined(BUILD_SSE_DWF_CG)
  foo &= SSEEvenOddPrecDWFermActArrayEnv::registered;
#endif

  return foo;
}


/*
 * Input 
 */
// Parameters which must be determined from the XML input
// and written to the XML output
struct Param_t
{
  multi1d<int> nrow;		// Lattice dimension
  multi1d<int> boundary;
  InvertParam_t   invParam;   // Inverter parameters
};

//! Mega-structure of all input
struct Test_input_t
{
  Param_t          param;
  Cfg_t            cfg;
  string           action_5d;
  string           action_4d;
};

//! Parameters for running code
void read(XMLReader& xml, const string& path, Param_t& param)
{
  XMLReader paramtop(xml, path);
  read(paramtop, "nrow", param.nrow);
  read(paramtop, "boundary", param.boundary);
  read(paramtop, "InvertParam", param.invParam);
}


// Reader for input parameters
void read(XMLReader& xml, const string& path, Test_input_t& input)
{
  XMLReader inputtop(xml, path);

  // Read all the input groups
  try
  {
    // Read program parameters
    read(inputtop, "Param", input.param);

    // Read in the gauge configuration info
    read(inputtop, "Cfg", input.cfg);

    // Read 4D action stuff
    {
      XMLReader xml_action(inputtop, "Action4D");
      std::ostringstream os;
      xml_action.print(os);
      input.action_4d = os.str();
    }

    // Read 5D action stuff
    {
      XMLReader xml_action(inputtop, "Action5D");
      std::ostringstream os;
      xml_action.print(os);
      input.action_5d = os.str();
    }
  }
  catch (const string& e) 
  {
    QDPIO::cerr << "Error reading test data: " << e << endl;
    throw;
  }
}




int main(int argc, char **argv)
{
  // Put the machine into a known state
  QDP_initialize(&argc, &argv);

  QDPIO::cout << "linkage=" << linkage_hack() << endl;

  // Input parameter structure
  Test_input_t  input;

  // Instantiate xml reader for DATA
  XMLReader xml_in("DATA");

  // Read data
  read(xml_in, "/t_dwf4d", input);

  // Specify lattice size, shape, etc.
  Layout::setLattSize(input.param.nrow);
  Layout::create();

  QDPIO::cout << "t_dwf4d" << endl;

  // Read in the configuration along with relevant information.
  multi1d<LatticeColorMatrix> u(Nd);
  XMLReader gauge_file_xml, gauge_xml;

  // Start up the gauge field
  gaugeStartup(gauge_file_xml, gauge_xml, u, input.cfg);

  // Instantiate XML writer for XMLDAT
  XMLFileWriter xml_out("t_dwf4d.xml");
  push(xml_out, "t_dwf4d");

  proginfo(xml_out);    // Print out basic program info

  // Write out the input
  write(xml_out, "Input", xml_in);

  // Write out the config header
  write(xml_out, "Config_info", gauge_xml);

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


  // Create a FermBC
  Handle< FermBC<LatticeFermion> >  fbc(new SimpleFermBC<LatticeFermion>(input.param.boundary));
  Handle< FermBC<multi1d<LatticeFermion> > >  fbc_a(new SimpleFermBC<multi1d<LatticeFermion> >(input.param.boundary));

  //
  // Initialize fermion action
  //
  std::istringstream  xml_s_5d(input.action_5d);
  std::istringstream  xml_s_4d(input.action_4d);
  XMLReader  fermacttop_5d(xml_s_5d);
  XMLReader  fermacttop_4d(xml_s_4d);
  const string fermact_path_5d = "/Action5D/FermionAction";
  const string fermact_path_4d = "/Action4D/FermionAction";
  string fermact_5d;
  string fermact_4d;

  try
    {
      read(fermacttop_5d, fermact_path_5d + "/FermAct", fermact_5d);
      read(fermacttop_4d, fermact_path_4d + "/FermAct", fermact_4d);
    }
  catch (const std::string& e) 
    {
      QDPIO::cerr << "Error reading fermact: " << e << endl;
      throw;
    }
  catch (const char* e) 
    {
      QDPIO::cerr << "Error reading fermact: " << e << endl;
      throw;
    }

  QDPIO::cout << "FermAct5D = " << fermact_5d << endl;
  QDPIO::cout << "FermAct4D = " << fermact_4d << endl;


  // Deal with auxiliary (and polymorphic) state information
  // eigenvectors, eigenvalues etc. The XML for this should be
  // stored as a string called "stateInfo" in the param struct.

  try {

  // Make a reader for the stateInfo
  const string state_info_path_5d = "/Action5D/StateInfo";
  const string state_info_path_4d = "/Action4D/StateInfo";
  XMLReader state_info_xml_5d(fermacttop_5d,state_info_path_5d);
  XMLReader state_info_xml_4d(fermacttop_4d,state_info_path_4d);


  // DWF-like 5D Wilson-Type stuff 
  bool success = false; 

  QDPIO::cerr << "create dwf = " << fermact_5d << endl;

  Handle< EvenOddPrecDWFermActBaseArray<LatticeFermion> >
    S_f_5d(TheEvenOddPrecDWFermActBaseArrayFactory::Instance().createObject(fermact_5d,
								       fbc_a,
								       fermacttop_5d,
								       fermact_path_5d));
  
  Handle<const ConnectState> state_5d(S_f_5d->createState(u,
							  state_info_xml_5d,
							    state_info_path_5d));
  
    // Overlap-like stuff
  QDPIO::cerr << "create overlap" << endl;
  Handle< WilsonTypeFermAct<LatticeFermion> >
    S_f_4d(TheWilsonTypeFermActFactory::Instance().createObject(fermact_4d,
								fbc,
								fermacttop_4d,
								fermact_path_4d));
  
  
  Handle<const ConnectState> state_4d(S_f_4d->createState(u,
							  state_info_xml_4d,
							  state_info_path_4d));
  
  
  //-------------------------------------------------------------------------------
  Handle<const LinearOperator<LatticeFermion> > A5(S_f_5d->linOp4D(state_5d,input.param.invParam));
  Handle<const LinearOperator<LatticeFermion> > A4(S_f_4d->linOp(state_4d));
  
  LatticeFermion psi, chi;
  
  random(psi);
  random(chi);
    
  LatticeFermion tmp1;
  (*A5)(tmp1, psi, PLUS);
  DComplex nn5 = innerProduct(chi, tmp1);
  
  LatticeFermion tmp2;
  (*A4)(tmp2, psi, PLUS);
  DComplex nn4 = innerProduct(chi, tmp2);
  
  push(xml_out,"innerprods");
  write(xml_out, "nn5", nn5);
  write(xml_out, "nn4", nn4);
  write(xml_out, "norm_diff", Real(norm2(tmp1-tmp2)));
  pop(xml_out);
  
  }
  catch (const std::string& e) 
    {
      QDPIO::cerr << "Error in t_dwf4d: " << e << endl;
      throw;
    }
  catch (const char* e) 
    {
      QDPIO::cerr << "Error in t_dwf4d: " << e << endl;
      throw;
    }
  
  pop(xml_out);

  // Time to bolt
  QDP_finalize();

  exit(0);
}
