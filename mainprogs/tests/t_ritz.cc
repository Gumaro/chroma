// $Id: t_ritz.cc,v 1.2 2004-01-16 14:16:14 bjoo Exp $

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include <cstdio>

#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#include "chroma.h"
#include "meas/eig/ritz.h"


using namespace QDP;
using namespace std;

enum GaugeStartType { HOT_START = 0, COLD_START = 1, FILE_START = 2 };
enum GaugeFormat { SZIN_GAUGE_FORMAT = 0, NERSC_GAUGE_FORMAT = 1 };



// Struct for test parameters
//
typedef struct {
  multi1d<int> nrow;
  multi1d<int> boundary;
  multi1d<int> rng_seed;
  int gauge_start_type;
  int gauge_file_format;
  string gauge_filename;
  
  Real quark_mass;
  Real rsd_r;
  int  n_eig;
  int max_cg;
} Param_t;

// Declare routine to read the parameters
void readParams(const string& filename, Param_t& params)
{
  XMLReader reader(filename);

  try {
    // Read Params
    read(reader, "/params/lattice/nrow", params.nrow);
    read(reader, "/params/lattice/boundary", params.boundary);
    read(reader, "/params/RNG/seed", params.rng_seed);
    read(reader, "/params/quarkMass", params.quark_mass);

    read(reader, "/params/Cfg/startType", params.gauge_start_type);
    if( params.gauge_start_type == FILE_START ) { 
      read(reader, "/params/Cfg/gaugeFilename", params.gauge_filename);
      read(reader, "/params/Cfg/gaugeFileFormat", params.gauge_file_format);
    }
   
   read(reader, "/params/eig/RsdR", params.rsd_r);
   read(reader, "/params/eig/MaxCG", params.max_cg);
   read(reader, "/params/eig/Neig", params.n_eig);

  }
  catch(const string& e) { 
    throw e;
  }
}

void dumpParams(XMLWriter& writer, Param_t& params)
{
  push(writer, "params");
  push(writer, "lattice");
  write(writer, "nrow", params.nrow);
  write(writer, "boundary", params.boundary);
  pop(writer); // lattice
  push(writer, "RNG");
  write(writer, "seed", params.rng_seed);
  pop(writer); // RNG

  write(writer, "quarkMass", params.quark_mass);
  push(writer, "Cfg");
  write(writer, "startType", params.gauge_start_type);
  if( params.gauge_start_type == FILE_START ) { 
    write(writer, "gaugeFileFormat", params.gauge_file_format);
    write(writer, "gaugeFilename", params.gauge_filename);
  }
  pop(writer); // Cfg

  push(writer, "eig");
  write(writer, "RsdR", params.rsd_r);
  write(writer, "MaxCG", params.max_cg);
  write(writer, "Neig", params.n_eig);
  pop(writer); // Eig

  pop(writer); // params
}

  
int main(int argc, char **argv)
{
  // Put the machine into a known state
  QDP_initialize(&argc, &argv);

  // Read the parameters 
  Param_t params;

  try { 
    readParams("./DATA", params);
  }
  catch(const string& s) { 
    QDPIO::cerr << "Caught exception " << s << endl;
    exit(1);
  }


  // Setup the lattice
  Layout::setLattSize(params.nrow);
  Layout::create();

  // Write out the params
  XMLFileWriter xml_out("t_ritz.xml");
  push(xml_out, "ritzTest");

  dumpParams(xml_out, params);


  // Create a FermBC
  Handle<FermBC<LatticeFermion> >  fbc(new SimpleFermBC<LatticeFermion>(params.boundary));
  
  // The Gauge Field
  multi1d<LatticeColorMatrix> u(Nd);
  
  switch ((GaugeStartType)params.gauge_start_type) { 
  case COLD_START:
    for(int j = 0; j < Nd; j++) { 
      u(j) = Real(1);
    }
    break;
  case HOT_START:
    // Hot (disordered) start
    for(int j=0; j < Nd; j++) { 
      random(u(j));
      reunit(u(j));
    }
    break;
  case FILE_START:

    // Start from File 
    switch( (GaugeFormat)params.gauge_file_format) { 
    case SZIN_GAUGE_FORMAT:
      {
	XMLReader szin_xml;
	readSzin(szin_xml, u, params.gauge_filename);
	try { 
	  push(xml_out, "GaugeInfo");
	  xml_out << szin_xml;
	  pop(xml_out);

	}
	catch(const string& e) {
	  cerr << "Error: " << e << endl;
	}
	
      }
      break;

    case NERSC_GAUGE_FORMAT:
      {
	XMLReader nersc_xml;
	readArchiv(nersc_xml, u, params.gauge_filename);

	try { 
	  push(xml_out, "GaugeInfo");
	  xml_out << nersc_xml;
	  pop(xml_out);

	}
	catch(const string& e) {
	  cerr << "Error: " << e << endl;
	}
      }
      break;

    default:
      ostringstream file_read_error;
      file_read_error << "Unknown gauge file format" << params.gauge_file_format ;
      throw file_read_error.str();
    }
    break;
  default:
    ostringstream startup_error;
    startup_error << "Unknown start type " << params.gauge_start_type <<endl;
    throw startup_error.str();
  }


  // Measure the plaquette on the gauge
  Double w_plaq, s_plaq, t_plaq, link;
  MesPlq(u, w_plaq, s_plaq, t_plaq, link);
  push(xml_out, "plaquette");
  Write(xml_out, w_plaq);
  Write(xml_out, s_plaq);
  Write(xml_out, t_plaq);
  Write(xml_out, link);
  pop(xml_out);

  //! Wilsoniums;
  // Put this puppy into a handle to allow Zolo to copy it around as a **BASE** class
  // WARNING: the handle now owns the data. The use of a bare S_w below is legal,
  // but just don't delete it.
  UnprecWilsonFermAct  S_w(fbc, params.quark_mass);

  Handle< const ConnectState > connect_state = S_w.createState(u);

  Handle< const LinearOperator<LatticeFermion> > MM = S_w.lMdagM(connect_state);

  // Try and get lowest eigenvalue of MM
  multi1d<Real> lambda(params.n_eig);
  multi1d<Real> check_norm(params.n_eig);
  multi1d<LatticeFermion> psi(params.n_eig);
  
  int n_renorm = 10;
  int n_min = 5;
  int n_max = params.max_cg + 1;
  bool ProjApsiP = true;
  int n_count;
  Real delta_cycle = Real(1);
  Real gamma_factor = Real(1);

  for(int i = 1 ; i <= params.n_eig; i++) { 
    gaussian(psi[i-1]);
    
    Ritz(*MM, lambda[i-1], psi, i, params.rsd_r, n_renorm,
	 n_min, n_max, params.max_cg, ProjApsiP, n_count, 
	 false, delta_cycle, gamma_factor);

    // Check back.
    LatticeFermion lambda_psi = lambda[i-1]*psi[i-1];
    LatticeFermion D_psi;
    (*MM)(D_psi, psi[i-1], PLUS);
    LatticeFermion r = D_psi - lambda_psi;
    check_norm[i-1] = Real(sqrt(norm2(r)));
 
    QDPIO::cout << "Check:  || M^M psi[" << i <<"] - lambda[ " << i <<"] psi[" << i <<"] || = " << check_norm[i-1] << endl;
  }

  push(xml_out, "LowEig");
  
  write(xml_out, "lambda", lambda);
  write(xml_out, "checkNorm", check_norm);

  for(int i=0; i < params.n_eig; i++) { 
    lambda[i] = sqrt(lambda[i]);               // Eval of Sqrt(MM)
    lambda[i] /= (Nd + params.quark_mass);     // SZIN normalisation
  }
  write(xml_out, "SZINLambdaAbs", lambda);
  pop(xml_out);

  pop(xml_out);
  QDP_finalize();
    
  exit(0);
}
