// $Id: t_ritz_KS.cc,v 1.13 2004-10-15 10:54:48 bjoo Exp $

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include <cstdio>

#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#include "chroma.h"
#include "io/param_io.h"
#include "io/eigen_io.h"


using namespace QDP;
using namespace std;
using namespace Chroma;

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

  return foo;
}

void RitzCode5D(Handle< const LinearOperator<multi1d<LatticeFermion> > >& MM,
		const ChromaWilsonRitz_t& input,
		XMLWriter& xml_out);

void RitzCode4D(Handle< const LinearOperator<LatticeFermion> >& MM,
		const ChromaWilsonRitz_t& input,
		XMLWriter& xml_out);

void RitzCode4DHw(Handle< const LinearOperator<LatticeFermion> >& MM,
		  Handle< const LinearOperator<LatticeFermion> >& H,
		  const ChromaWilsonRitz_t& input,
		  XMLWriter& xml_out);

  
int main(int argc, char **argv)
{
  // Put the machine into a known state
  QDP_initialize(&argc, &argv);

  ChromaWilsonRitz_t input;
  XMLReader xml_in("DATA");

  try { 
    read(xml_in, "/WilsonRitzEigen", input);
  }
  catch( const string& e ) { 
    QDPIO::cerr << "Caught Exception: " << e << endl;
    QDP_error_exit("Exiting\n");
  }

  // Setup the lattice
  Layout::setLattSize(input.nrow);
  Layout::create();


  QDP::RNG::setrn(input.seed);

  QDPIO::cout << "RitzEigen" << endl;

  multi1d<LatticeColorMatrix> u(Nd);
  XMLReader gauge_file_xml, gauge_xml;
  gaugeStartup(gauge_file_xml, gauge_xml, u, input.cfg);

  XMLFileWriter xml_out("XMLDAT");
  push(xml_out, "RitzEigen");
  proginfo(xml_out);

  // Write out the input
  write(xml_out, "Input", xml_in);
  write(xml_out, "Config_info", gauge_xml);

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

  /* Construct fermionic BC Need one for LatticeFermion and 
   * multi1d<LatticeFermion>   
   * Note, the handle is on an ABSTRACT type
   */
  Handle< FermBC<LatticeFermion> >  fbc(new SimpleFermBC<LatticeFermion>(input.boundary));
  Handle< FermBC<multi1d<LatticeFermion> > >  fbc_a(new SimpleFermBC<multi1d<LatticeFermion> >(input.boundary));


  // Initialise Fermion action
  std::istringstream xml_fermact_string(input.fermact);
  XMLReader fermacttop(xml_fermact_string);
  const string fermact_path = "/FermionAction";
  string fermact;
  try
  {
    read(fermacttop, fermact_path + "/FermAct", fermact);
  }
  catch (const std::string& e) 
  {
    QDPIO::cerr << "Error reading fermact: " << e << endl;
    throw;
  }

  QDPIO::cout << "FermAct = " << fermact << endl;

  // Make a reader for the stateInfo
  std::istringstream state_info_is(input.state_info);
  XMLReader state_info_xml(state_info_is);
  string state_info_path="/StateInfo";

  bool success = false;

  if( ! success ) { 
    try { 
      QDPIO::cout << "Trying unprec DWF-like actions" << endl;

      // DWF-like 5D Wilson-Type stuff
      Handle< UnprecDWFermActBaseArray<LatticeFermion> >
	S_f(TheUnprecDWFermActBaseArrayFactory::Instance().createObject(fermact,
									fbc_a,
									fermacttop,
									fermact_path));

      Handle<const ConnectState> state(S_f->createState(u,
							state_info_xml,
							state_info_path));

      Handle< const LinearOperator<multi1d<LatticeFermion> > > MM(S_f->lMdagM(state));


      RitzCode5D(MM, input, xml_out);

      success = true;
    }
    catch(const std::string& e ) { 
       QDPIO::cout << "Unprec DWF-like: " << e << endl;
    }
  }

  if( ! success ) { 
    try { 
      QDPIO::cout << "Trying  even odd prec DWF-like actions" << endl;

      // DWF-like 5D Wilson-Type stuff
      Handle< EvenOddPrecDWFermActBaseArray<LatticeFermion> >
	S_f(TheEvenOddPrecDWFermActBaseArrayFactory::Instance().createObject(fermact,
									fbc_a,
									fermacttop,
									fermact_path));

      Handle<const ConnectState> state(S_f->createState(u,
							state_info_xml,
							state_info_path));

      Handle< const LinearOperator<multi1d<LatticeFermion> > > MM(S_f->lMdagM(state));

      RitzCode5D(MM, input, xml_out);

      success = true;
    }
    catch(const std::string& e ) { 
       QDPIO::cout << "EvenOddPrec DWF-like: " << e << endl;
    }
  }

  if( ! success ) { 
    try { 
      QDPIO::cout << "Trying 5D Wilson like actions" << endl;

      // DWF-like 5D Wilson-Type stuff
      Handle< WilsonTypeFermAct< multi1d<LatticeFermion> > >
	S_f(TheWilsonTypeFermActArrayFactory::Instance().createObject(fermact,
									fbc_a,
									fermacttop,
									fermact_path));

      Handle<const ConnectState> state(S_f->createState(u,
							state_info_xml,
							state_info_path));

      Handle< const LinearOperator<multi1d<LatticeFermion> > > MM(S_f->lMdagM(state));

      QDPIO::cout << "LINOP CREATED" << endl << flush <<endl;
      RitzCode5D(MM, input, xml_out);

      success = true;
    }
    catch(const std::string& e ) { 
       QDPIO::cout << "5D Wilson like: " << e << endl;
    }
  }

  if( ! success ) { 
    try { 
      
      // Special case UNPRECONDITIONED_WILSON
      QDPIO::cout << "Trying 4D Wilson Like actions: " << endl;

      if( fermact == "UNPRECONDITIONED_WILSON") {
	QDPIO::cout << "Special case. Computing Hw e-values and evecs too" << endl;
	// DWF-like 5D Wilson-Type stuff
	Handle< WilsonTypeFermAct<LatticeFermion> >
	  S_f(TheWilsonTypeFermActFactory::Instance().createObject(fermact,
								   fbc,
								   fermacttop,
								   fermact_path));
	
	Handle<const ConnectState> state(S_f->createState(u,
							  state_info_xml,
							  state_info_path));

	Handle< const LinearOperator<LatticeFermion> > MM(S_f->lMdagM(state));

	Handle< const LinearOperator<LatticeFermion> > H(S_f->gamma5HermLinOp(state));

	RitzCode4DHw(MM, H, input, xml_out);

	success = true;
      }
      else {
// DWF-like 5D Wilson-Type stuff
	Handle< WilsonTypeFermAct<LatticeFermion> >
	  S_f(TheWilsonTypeFermActFactory::Instance().createObject(fermact,
								   fbc,
								   fermacttop,
								   fermact_path));
	
	Handle<const ConnectState> state(S_f->createState(u,
							  state_info_xml,
							  state_info_path));

	Handle< const LinearOperator<LatticeFermion> > MM(S_f->lMdagM(state));

	RitzCode4D(MM, input, xml_out);

	success = true;
      }
    }
    catch(const std::string& e ) { 
       QDPIO::cout << "4D: " << e << endl;
    }
  }






  pop(xml_out);
  xml_out.close();
  QDP_finalize();
    
  exit(0);
}


void RitzCode4D(Handle< const LinearOperator<LatticeFermion> >& MM,
		const ChromaWilsonRitz_t& input,
		XMLWriter& xml_out)
{

  // Try and get lowest eigenvalue of MM
  const OrderedSubset& s = MM->subset();
  
  multi1d<Real> lambda(input.ritz_params.Neig+input.ritz_params.Ndummy);
  multi1d<Real> check_norm(input.ritz_params.Neig);
  multi1d<LatticeFermion> psi(input.ritz_params.Neig
			      +input.ritz_params.Ndummy);
      
  
  for(int i =0; i < input.ritz_params.Neig + input.ritz_params.Ndummy; i++){
    psi[i] = zero;
    gaussian(psi[i],s);
    lambda[i] = Real(1);
  }

    
  int n_CG_count;
  Real delta_cycle = Real(1);
  XMLBufferWriter eig_spec_xml;
  int n_KS_count = 0;
  int n_jacob_count = 0;
  EigSpecRitzKS(*MM, 
		lambda, 
		psi, 
		input.ritz_params.Neig,
		input.ritz_params.Ndummy,                // No of dummies
		input.ritz_params.Nrenorm, 
		input.ritz_params.MinKSIter, 
		input.ritz_params.MaxKSIter,             // Max iters / KS cycle
		input.ritz_params.MaxKS,            // Max no of KS cycles
		input.ritz_params.GammaFactor,       // Gamma factor
		input.ritz_params.MaxCG,
		input.ritz_params.RsdR,
		input.ritz_params.RsdA,  
		input.ritz_params.RsdZero,
		input.ritz_params.ProjApsiP,
		n_CG_count,
		n_KS_count,
		n_jacob_count,
		eig_spec_xml);
  
  // Dump output
  xml_out << eig_spec_xml;
  write(xml_out, "lambda_Msq", lambda); 
  
  // Check norms
  for(int i=0; i < input.ritz_params.Neig; i++) { 
    LatticeFermion Me;
    LatticeFermion lambda_e;
    (*MM)(Me, psi[i], PLUS);
    lambda_e[s] = lambda[i]*psi[i];
    
    
    LatticeFermion r_norm;
    r_norm[s] = Me - lambda_e;
    
    check_norm[i] = norm2(r_norm,s);
    check_norm[i] = sqrt(check_norm[i]);
  }
  write(xml_out, "check_norm", check_norm);
  
  for(int i=0; i < input.ritz_params.Neig; i++) {
    check_norm[i] /= fabs(lambda[i]);
  }
  write(xml_out, "check_norm_rel", check_norm);
  
  
  // Now get the absolute value of the  highest e-value
  // Work with H^{dag}H = M^{dag}M
  Real hi_RsdR = 1.0e-4;
  Real hi_RsdA = 1.0e-4;

  Handle<const LinearOperator<LatticeFermion> > MinusMM = new lopscl<LatticeFermion, Real>(MM, Real(-1.0));
  
  multi1d<Real> lambda_high_aux(1);
  multi1d<LatticeFermion> lambda_high_vec(1);
  gaussian(lambda_high_vec[0],s);
  lambda_high_vec[0][s] /= sqrt(norm2(lambda_high_vec[0],s));

  int n_cg_high;
  XMLBufferWriter high_xml;
  
  // Initial guess -- upper bound on spectrum
  lambda_high_aux[0] = Real(8);
  
  
  push(high_xml, "LambdaHighRitz");
      
  // Minus MM ought to produce a negative e-value
  // since MM is herm_pos_def
  // ie minus MM is hermitian -ve definite
  EigSpecRitzCG( *MinusMM,
		 lambda_high_aux,
		 lambda_high_vec,
		 1,
		 input.ritz_params.Nrenorm,
		 input.ritz_params.MinKSIter,
		 input.ritz_params.MaxCG,
		 hi_RsdR,
		 hi_RsdA,
		 input.ritz_params.RsdZero,
		 input.ritz_params.ProjApsiP,
		 n_cg_high,
		 high_xml);
  
  lambda_high_aux[0] = sqrt(fabs(lambda_high_aux[0]));
  QDPIO::cout << "|| lambda_hi || = " << lambda_high_aux[0]  << " hi_Rsd_r = " << hi_RsdR << endl;
  
  pop(high_xml);
  xml_out << high_xml;
  
  push(xml_out, "Highest");
  write(xml_out, "lambda_Msq_hi", lambda_high_aux[0]);
  pop(xml_out);
}

void RitzCode4DHw(Handle< const LinearOperator<LatticeFermion> >& MM,
		  Handle< const LinearOperator<LatticeFermion> >& H,
		  const ChromaWilsonRitz_t& input,
		  XMLWriter& xml_out)
{

  // Try and get lowest eigenvalue of MM
  const OrderedSubset& s = MM->subset();
  
  multi1d<Real> lambda(input.ritz_params.Neig+input.ritz_params.Ndummy);
  multi1d<Real> check_norm(input.ritz_params.Neig);
  multi1d<LatticeFermion> psi(input.ritz_params.Neig
			      +input.ritz_params.Ndummy);
      
  
  for(int i =0; i < input.ritz_params.Neig + input.ritz_params.Ndummy; i++){
    psi[i] = zero;
    gaussian(psi[i],s);
    lambda[i] = Real(1);
  }

    
  int n_CG_count;
  Real delta_cycle = Real(1);
  XMLBufferWriter eig_spec_xml;
  int n_KS_count = 0;
  int n_jacob_count = 0;
  EigSpecRitzKS(*MM, 
		lambda, 
		psi, 
		input.ritz_params.Neig,
		input.ritz_params.Ndummy,                // No of dummies
		input.ritz_params.Nrenorm, 
		input.ritz_params.MinKSIter, 
		input.ritz_params.MaxKSIter,             // Max iters / KS cycle
		input.ritz_params.MaxKS,            // Max no of KS cycles
		input.ritz_params.GammaFactor,       // Gamma factor
		input.ritz_params.MaxCG,
		input.ritz_params.RsdR,
		input.ritz_params.RsdA,  
		input.ritz_params.RsdZero,
		input.ritz_params.ProjApsiP,
		n_CG_count,
		n_KS_count,
		n_jacob_count,
		eig_spec_xml);
  
  // Dump output
  xml_out << eig_spec_xml;
  write(xml_out, "lambda_Msq", lambda); 
  
  // Check norms
  for(int i=0; i < input.ritz_params.Neig; i++) { 
    LatticeFermion Me;
    LatticeFermion lambda_e;
    (*MM)(Me, psi[i], PLUS);
    lambda_e[s] = lambda[i]*psi[i];
    
    
    LatticeFermion r_norm;
    r_norm[s] = Me - lambda_e;
    
    check_norm[i] = norm2(r_norm,s);
    check_norm[i] = sqrt(check_norm[i]);
  }
  write(xml_out, "check_norm", check_norm);
  
  for(int i=0; i < input.ritz_params.Neig; i++) {
    check_norm[i] /= fabs(lambda[i]);
  }
  write(xml_out, "check_norm_rel", check_norm);
  
  // Fix to ev-s of gamma_5 wilson...
  // Try to get one:
  multi1d<bool> valid_eig(input.ritz_params.Neig);
  int n_valid;
  int n_jacob;
  
  fixMMev2Mev(*H, 
	      lambda, 
	      psi, 
	      input.ritz_params.Neig, 
	      input.ritz_params.RsdR,
	      input.ritz_params.RsdA, 
	      input.ritz_params.RsdZero, 
	      valid_eig, 
	      n_valid, 
	      n_jacob);
  
  push(xml_out, "eigFix");
  write(xml_out, "lambda_Hw", lambda);
  write(xml_out, "n_valid", n_valid);
  write(xml_out, "valid_eig", valid_eig);
  
  for(int i=0; i < input.ritz_params.Neig; i++) { 
    LatticeFermion Me;
    (*H)(Me, psi[i], PLUS);
    
    bool zeroP = toBool( fabs(lambda[i]) <  input.ritz_params.RsdZero );
    if( zeroP ) {
      check_norm[i] = norm2(Me,s);
      check_norm[i] = sqrt(check_norm[i]);
    }
    else {
      LatticeFermion lambda_e;
      LatticeFermion r_norm;
      
      lambda_e[s] = lambda[i]*psi[i];
      r_norm[s] = Me - lambda_e;
      
      
      check_norm[i] = norm2(r_norm,s);
      check_norm[i] = sqrt(check_norm[i]);
    }
    
    QDPIO::cout << "check_norm["<<i+1<<"] = " << check_norm[i] << endl;
  }
  write(xml_out, "check_norm", check_norm);
  
  for(int i=0; i < input.ritz_params.Neig; i++) { 
    check_norm[i] /= fabs(lambda[i]);
    QDPIO::cout << "check_norm_rel["<< i+1 <<"] = " << check_norm[i] << endl;
  }
  write(xml_out, "check_norm_rel", check_norm);
  pop(xml_out);
  
  
  // Now get the absolute value of the  highest e-value
  // Work with H^{dag}H = M^{dag}M
  Real hi_RsdR = 1.0e-4;
  Real hi_RsdA = 1.0e-4;
  
  multi1d<Real> lambda_high_aux(1);
  multi1d<LatticeFermion> lambda_high_vec(1);
  gaussian(lambda_high_vec[0],s);
  lambda_high_vec[0][s] /= sqrt(norm2(lambda_high_vec[0],s));

  int n_cg_high;
  XMLBufferWriter high_xml;
  
  Handle<const LinearOperator<LatticeFermion> > MinusMM = new lopscl<LatticeFermion, Real>(MM, Real(-1.0));
  // Initial guess -- upper bound on spectrum
  lambda_high_aux[0] = Real(8);
  
  
  push(high_xml, "LambdaHighRitz");
      
  // Minus MM ought to produce a negative e-value
  // since MM is herm_pos_def
  // ie minus MM is hermitian -ve definite
  EigSpecRitzCG( *MinusMM,
		 lambda_high_aux,
		 lambda_high_vec,
		 1,
		 input.ritz_params.Nrenorm,
		 input.ritz_params.MinKSIter,
		 input.ritz_params.MaxCG,
		 hi_RsdR,
		 hi_RsdA,
		 input.ritz_params.RsdZero,
		 input.ritz_params.ProjApsiP,
		 n_cg_high,
		 high_xml);
  
  lambda_high_aux[0] = sqrt(fabs(lambda_high_aux[0]));
  QDPIO::cout << "|| lambda_hi || = " << lambda_high_aux[0]  << " hi_Rsd_r = " << hi_RsdR << endl;
  
  pop(high_xml);
  xml_out << high_xml;
  
  push(xml_out, "Highest");
  write(xml_out, "lambda_Msq_hi", lambda_high_aux[0]);
  pop(xml_out);

  QDPIO::cout << "Writing low eigenvalues/vectors" << endl;
  writeEigen(input, lambda, psi, lambda_high_aux[0], QDPIO_SERIAL);

}

void RitzCode5D(Handle< const LinearOperator<multi1d<LatticeFermion> > >& MM,
		const ChromaWilsonRitz_t& input,
		XMLWriter& xml_out)
{
  // Try and get lowest eigenvalue of MM
  int N5 = MM->size();
  const OrderedSubset& s = MM->subset();
  
  multi1d<Real> lambda(input.ritz_params.Neig+input.ritz_params.Ndummy);
  multi1d<Real> check_norm(input.ritz_params.Neig);
  multi2d<LatticeFermion> psi(input.ritz_params.Neig
			      +input.ritz_params.Ndummy, N5);
      
  
  for(int i =0; i < input.ritz_params.Neig + input.ritz_params.Ndummy; i++){
    for(int n=0; n < N5; n++) { 
      psi[i][n] = zero;
      gaussian(psi[i][n],s);
    }
    lambda[i] = Real(1);
  }
  
  
  int n_CG_count;
  Real delta_cycle = Real(1);
  XMLBufferWriter eig_spec_xml;
  int n_KS_count = 0;
  int n_jacob_count = 0;
  
  EigSpecRitzKS(*MM, 
		lambda, 
		psi, 
		input.ritz_params.Neig,
		input.ritz_params.Ndummy,                // No of dummies
		input.ritz_params.Nrenorm, 
		input.ritz_params.MinKSIter, 
		input.ritz_params.MaxKSIter,             // Max iters / KS cycle
		input.ritz_params.MaxKS,            // Max no of KS cycles
		input.ritz_params.GammaFactor,       // Gamma factor
		input.ritz_params.MaxCG,
		input.ritz_params.RsdR,
		input.ritz_params.RsdA,  
		input.ritz_params.RsdZero,
		input.ritz_params.ProjApsiP,
		n_CG_count,
		n_KS_count,
		n_jacob_count,
		eig_spec_xml);
  
  /*
  EigSpecRitzCG( *MM,
		 lambda,
		 psi,
		 input.ritz_params.Neig,
		 input.ritz_params.Nrenorm,
		 input.ritz_params.MinKSIter,
		 input.ritz_params.MaxCG,
		 input.ritz_params.RsdR,
		 input.ritz_params.RsdA,
		 input.ritz_params.RsdZero,
		 input.ritz_params.ProjApsiP,
		 n_CG_count,
		 eig_spec_xml);
  */
  // Dump output
  xml_out << eig_spec_xml;
  write(xml_out, "lambda_Msq", lambda); 
  
  // Check norms
  for(int i=0; i < input.ritz_params.Neig; i++) { 
    multi1d<LatticeFermion> Me(N5);
    multi1d<LatticeFermion> lambda_e(N5);
    (*MM)(Me, psi[i], PLUS);
    for(int n =0; n < N5; n++) { 
      lambda_e[n][s] = lambda[i]*psi[i][n];
    }
    
    multi1d<LatticeFermion> r_norm(N5);
    for(int n=0; n < N5; n++) { 
      r_norm[n][s] = Me[n] - lambda_e[n];
    }
    
    check_norm[i] = norm2(r_norm[0],s);
    for(int n=1; n < N5; n++) {
      check_norm[i] += norm2(r_norm[n],s);
    }
    
    check_norm[i] = sqrt(check_norm[i]);
  }
  write(xml_out, "check_norm", check_norm);
  
  for(int i=0; i < input.ritz_params.Neig; i++) {
    check_norm[i] /= fabs(lambda[i]);
  }
  write(xml_out, "check_norm_rel", check_norm);
  
  // Now get the absolute value of the  highest e-value
  // Work with H^{dag}H = M^{dag}M
  
  
  Real hi_RsdR = 1.0e-4;
  Real hi_RsdA = 1.0e-4;
  Handle<const LinearOperator<multi1d<LatticeFermion> > > MinusMM = new lopscl<multi1d<LatticeFermion>, Real>(MM, Real(-1.0));
  
  multi1d<Real> lambda_high_aux(1);
  multi2d<LatticeFermion> lambda_high_vec(1,N5);
  for(int n=0; n < N5; n++) {
    gaussian(lambda_high_vec[0][n],s);
    lambda_high_vec[0][n][s] /= sqrt(norm2(lambda_high_vec[0],s));
  }
  
  int n_cg_high;
  XMLBufferWriter high_xml;
  
  // Initial guess -- upper bound on spectrum
  lambda_high_aux[0] = Real(8);
  
  
  push(high_xml, "LambdaHighRitz");
      
  // Minus MM ought to produce a negative e-value
  // since MM is herm_pos_def
  // ie minus MM is hermitian -ve definite
  EigSpecRitzCG( *MinusMM,
		 lambda_high_aux,
		 lambda_high_vec,
		 1,
		 input.ritz_params.Nrenorm,
		 input.ritz_params.MinKSIter,
		 input.ritz_params.MaxCG,
		 hi_RsdR,
		 hi_RsdA,
		 input.ritz_params.RsdZero,
		 input.ritz_params.ProjApsiP,
		 n_cg_high,
		 high_xml);
  
  lambda_high_aux[0] = sqrt(fabs(lambda_high_aux[0]));
  QDPIO::cout << "|| lambda_hi || = " << lambda_high_aux[0]  << " hi_Rsd_r = " << hi_RsdR << endl;
  
  pop(high_xml);
  xml_out << high_xml;
  
  push(xml_out, "Highest");
  write(xml_out, "lambda_Msq_hi", lambda_high_aux[0]);
  pop(xml_out);
}
