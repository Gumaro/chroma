// -*- C++ -*-
// $Id: stout_fermstate_w.cc,v 1.9 2006-08-17 20:45:33 bjoo Exp $
/*! @file 
 *  @brief Connection State for Stout state (.cpp file)
 */

#include "chromabase.h"
#include "actions/ferm/fermacts/stout_fermstate_w.h"
#include "util/gauge/expmat.h"
#include "util/gauge/taproj.h"

#include "actions/ferm/fermacts/ferm_createstate_factory_w.h"
#include "actions/ferm/fermacts/ferm_createstate_aggregate_w.h"
#include "actions/ferm/fermbcs/fermbcs_reader_w.h"


namespace Chroma 
{ 

  /*! \ingroup fermacts */
  namespace CreateStoutFermStateEnv 
  { 
    CreateFermState<LatticeFermion,
		    multi1d<LatticeColorMatrix>, 
		    multi1d<LatticeColorMatrix> >* createFerm(XMLReader& xml, 
							      const std::string& path) 
    {
      return new CreateStoutFermState(WilsonTypeFermBCEnv::reader(xml, path),
				      StoutFermStateParams(xml, path));
    }

    const std::string name = "STOUT_FERM_STATE";

    //! Register all the factories
    bool registerAll()
    {
      bool foo = true;
      foo &= Chroma::TheCreateFermStateFactory::Instance().registerObject(name, 
									  createFerm);
      return foo;
    }

    const bool registered = registerAll();
  }
  /*! \ingroup fermacts */
  namespace CreateSLICFermStateEnv 
  { 
    CreateFermState<LatticeFermion,
		    multi1d<LatticeColorMatrix>, 
		    multi1d<LatticeColorMatrix> >* createFerm(XMLReader& xml, 
							      const std::string& path) 
    {
      return new CreateSLICFermState(WilsonTypeFermBCEnv::reader(xml, path),
				     StoutFermStateParams(xml, path));
    }

    const std::string name = "SLIC_FERM_STATE";

    //! Register all the factories
    bool registerAll()
    {
      bool foo = true;
      foo &= Chroma::TheCreateFermStateFactory::Instance().registerObject(name, 
									  createFerm);
      return foo;
    }

    const bool registered = registerAll();
  }


  namespace StoutLinkTimings { 
    static double smearing_secs = 0;
    double getSmearingTime() { 
      return smearing_secs;
    }

    static double force_secs = 0;
    double getForceTime() { 
      return force_secs;
    }

    static double functions_secs = 0;
    double getFunctionsTime() { 
      return functions_secs;
    }
  }

  void StoutFermState::getFsAndBs(const LatticeColorMatrix& Q,
				  const LatticeColorMatrix& QQ,
				  multi1d<LatticeDComplex>& f,
				  multi1d<LatticeDComplex>& b1,
				  multi1d<LatticeDComplex>& b2,
				  bool dobs) const
  {
    START_CODE();
    QDP::StopWatch swatch;
    swatch.reset();
    swatch.start();

    f.resize(3);
    if( dobs ) { 
      b1.resize(3);
      b2.resize(3);
    }
    int num_sites = Layout::sitesOnNode();

    // Drop into a site loop here...
    for(int site=0; site < num_sites; site++) { 
      PColorMatrix<QDP::RComplex<REAL>, 3>  Q_site = Q.elem(site).elem();
      PColorMatrix<QDP::RComplex<REAL>, 3>  QQ_site = QQ.elem(site).elem();
      PColorMatrix<QDP::RComplex<REAL>, 3>  QQQ = QQ_site*Q_site;
    
      Double trQQQ; 
      trQQQ.elem()  = real(trace(QQQ));
      Double trQQ;
      trQQ.elem()   = real(trace(QQ_site));
			  
      Double c0    = (Double(1)/Double(3)) * trQQQ;  // eq 13
      Double c1    = (Double(1)/Double(2)) * trQQ;	 // eq 15 

      Boolean c0_negativeP = c0 < Double(0);
      Double c0abs = fabs(c0);
      Double c0max = Double(2)*pow( c1/Double(3), Double(1.5));
    

      Double theta;
      Boolean c0max_is_zeroP  =( c0max == 0 );

      theta = acos( c0abs/c0max );
      

      Double u = sqrt(c1/Double(3))*cos(theta/Double(3));
      Double w = sqrt(c1)*sin(theta/Double(3));
    
      Double u_sq = u*u;
      Double w_sq = w*w;
    
      Double xi0,xi1;
      {
	Boolean w_smallP  = fabs(w) < Double(0.05);
	if( toBool( w_smallP ) ) { 
	  xi0 = Double(1) - (Double(1)/Double(6))*w_sq*(Double(1) - (Double(1)/Double(20))*w_sq*(Double(1) - (Double(1)/Double(42))*w_sq));
	}
	else {
	  xi0 = sin(w)/w;
	}

	if( dobs==true) {

	  if( toBool( w_smallP ) ) { 
	    xi1 = Double(-1)*((Double(1)/Double(3)) - ( Double(1)/Double(30) )*w_sq*(Double(1) - (Double(1)/Double(28))*w_sq*(Double(1)- (Double(1)/Double(54))*w_sq)));
	  }
	  else { 
	    xi1 = cos(w)/w_sq - sin(w)/(w_sq*w);
	  }
	}
      }
      
    
      Double cosu = cos(u);
      Double sinu = sin(u);
      Double cosw = cos(w);
      Double sinw = sin(w);
      Double sin2u = sin(2*u);
      Double cos2u = cos(2*u);

    // exp(2iu) and exp(-iu)
    //LatticeDComplex exp2iu = cmplx(( 2*cosu*cosu - 1), 2*cosu*sinu);
      DComplex exp2iu = cmplx( cos2u, sin2u );
      DComplex expmiu = cmplx(cosu, -sinu);

      Double denum = 9*u_sq - w_sq;
      
      multi1d<DComplex> f_site(3);

      // f_i = f_i(c0, c1). Expand f_i by c1, if c1 is small.
      f_site[0] = ((u_sq - w_sq) * exp2iu + expmiu * cmplx(8*u_sq*cosw, 2*u*(3*u_sq+w_sq)*xi0))/denum;
      
      
      f_site[1] = (2*u*exp2iu - expmiu * cmplx(2*u*cosw, (w_sq-3*u_sq)*xi0))/denum;
      
      f_site[2] = (exp2iu - expmiu * cmplx(cosw, 3*u*xi0))/denum;


      if( dobs == true ) {
	
	multi1d<DComplex> r_1(3);
	multi1d<DComplex> r_2(3);
      
	r_1[0]=Double(2)*cmplx(u, u_sq-w_sq)*exp2iu
	  + 2.0*expmiu*( cmplx(8.0*u*cosw, -4.0*u_sq*cosw)
			 + cmplx(u*(3.0*u_sq+w_sq),9.0*u_sq+w_sq)*xi0 );
	
	r_1[1]=cmplx(2.0, 4.0*u)*exp2iu
	  + expmiu*cmplx(-2.0*cosw-(w_sq-3.0*u_sq)*xi0,
			 2.0*u*cosw+6.0*u*xi0);
	
	r_1[2]=2.0*timesI(exp2iu)
	  +expmiu*cmplx(-3.0*u*xi0, cosw-3*xi0);
	
	
	r_2[0]=-2.0*exp2iu + 2*cmplx(0,u)*expmiu*cmplx(cosw+xi0+3*u_sq*xi1,
						       4*u*xi0);
	
	r_2[1]= expmiu*cmplx(cosw+xi0-3.0*u_sq*xi1, 2.0*u*xi0);
	r_2[1] = timesMinusI(r_2[1]);
	
	r_2[2]=expmiu*cmplx(xi0, -3.0*u*xi1);
    
	multi1d<DComplex> b1_site(3);
	multi1d<DComplex> b2_site(3);

      
	Double b_denum=2.0*(9.0*u_sq -w_sq)*(9.0*u_sq-w_sq);
      
	for(int j=0; j < 3; j++) { 
	
	  // This has to be a little more careful	
	  b1_site[j]=( 2.0*u*r_1[j]+(3.0*u_sq-w_sq)*r_2[j]-2.0*(15.0*u_sq+w_sq)*f_site[j] )/b_denum;
	  b2_site[j]=( r_1[j]-3.0*u*r_2[j]-24.0*u*f_site[j] )/b_denum;
		
	}
      
	if( toBool(c0_negativeP) ) { 
	  b1_site[0] = conj(b1_site[0]);
	  b1_site[1] = -conj(b1_site[1]);
	  b1_site[2] = conj(b1_site[2]);
	  b2_site[0] = -conj(b2_site[0]);
	  b2_site[1] = conj(b2_site[1]);
	  b2_site[2] = -conj(b2_site[2]);
	}

	for(int j=0; j < 3; j++) { 
	  b1[j].elem(site).elem().elem()  = b1_site[j].elem().elem().elem();	  
	  b2[j].elem(site).elem().elem() = b2_site[j].elem().elem().elem();
	}

      }

      if( toBool(c0_negativeP) ) { 
	f_site[0] = conj(f_site[0]);
	f_site[1] = -conj(f_site[1]);
	f_site[2] = conj(f_site[2]);
      }

      for(int j=0; j < 3; j++) { 
	f[j].elem(site).elem().elem() = f_site[j].elem().elem().elem();
      }
    }
    
    swatch.stop();
    StoutLinkTimings::functions_secs += swatch.getTimeInSeconds();
    END_CODE();
  }


  void StoutFermState::getQsandCs(const multi1d<LatticeColorMatrix>& u, LatticeColorMatrix& Q, 
				  LatticeColorMatrix& QQ,
				  LatticeColorMatrix& C, 
				  int mu) const
  {
    START_CODE();

    C = zero;

    // If rho is nonzero in this direction then accumulate the staples
    for(int nu=0; nu < Nd; nu++) { 
	
      // Accumulate mu-nu staple
      if( (mu != nu) && params.smear_in_this_dirP[nu] ) {
	LatticeColorMatrix U_nu_plus_mu = shift(u[nu], FORWARD, mu);
	LatticeColorMatrix tmp_mat;
	LatticeColorMatrix tmp_mat2; 
	
	
	// Forward staple
	//             2
	//       ^ ---------> 
	//       |          |
	//    1  |          |  3
	//       |          |
	//       |          V
	//       x          x + mu
	//
	tmp_mat = shift(u[mu],FORWARD, nu);
	tmp_mat2 = u[nu]*tmp_mat;
	tmp_mat  = tmp_mat2*adj(U_nu_plus_mu);
	
	
	// Backward staple
	//             
	//       |          ^ 
	//       |          |
	//    1  |          |  3
	//       |     2    |
	//       V--------->|          
	//       x-nu        x - nu  + mu
	//
	//
	//  we construct it on x-nu and shift it up to x, 
	// (with a backward shift)
	
	// This is the staple on x-nu:
	// tmp_1(x) = u_dag(x,nu)*u(x,mu)*u(x+mu,nu)
	{
	  LatticeColorMatrix tmp_mat3;
	    tmp_mat3 = adj(u[nu])*u[mu];
	    tmp_mat2 = tmp_mat3*U_nu_plus_mu;
	    tmp_mat += shift(tmp_mat2, BACKWARD, nu);
	    tmp_mat *= params.rho(mu,nu);
	}
	C += tmp_mat;
	  
      }
    }

    // Now I can form the Q
    LatticeColorMatrix Omega;
    Omega = C*adj(u[mu]); // Q_mu is Omega mu here (eq 2 part 2)
    
    LatticeColorMatrix tmp2 = adj(Omega) - Omega;
    LatticeColorMatrix tmp3 = trace(tmp2);
    tmp3 *= Real(1)/Real(Nc);
    tmp2 -= tmp3;
    tmp2 *= Real(0.5);
    Q = timesI(tmp2);
    QQ = Q*Q;
    
    END_CODE();
  }
  
  // Do the force recursion from level i+1, to level i
  // The input fat_force F is modified.
  void StoutFermState::deriv_recurse(multi1d<LatticeColorMatrix>& F,
				     const int level ) const
  {
    START_CODE();

    // Things I need
    // C_{\mu} = staple multiplied appropriately by the rho
    // Lambda matrices asper eq(73) 
    multi1d<LatticeColorMatrix> F_plus(Nd);

    // Save the fat force
    F_plus = F;

   
    multi1d<LatticeColorMatrix> Lambda(Nd);
    multi1d<LatticeColorMatrix> C(Nd);

    // The links at this level (unprimed in the paper).
    const multi1d<LatticeColorMatrix>& u = smeared_links[level];

    for(int mu=0; mu < Nd; mu++) 
    {
      if( params.smear_in_this_dirP[mu] ) { 
	LatticeColorMatrix Q,QQ;   // This is the C U^{dag}_mu suitably antisymmetrized
  
	// Get Q, Q^2, C, c0 and c1 -- this code is the same as used in stout_smear()
	getQsandCs(u, Q, QQ, C[mu], mu);
	
	// Now work the f-s and b-s
	multi1d<LatticeDComplex> f;
	multi1d<LatticeDComplex> b_1;
	multi1d<LatticeDComplex> b_2;
	
	// Get the fs and bs  -- does internal resize to make them arrays of length 3
	getFsAndBs(Q,QQ, f, b_1, b_2, true);
	
	
	LatticeColorMatrix B_1 = b_1[0] + b_1[1]*Q + b_1[2]*QQ;
	LatticeColorMatrix B_2 = b_2[0] + b_2[1]*Q + b_2[2]*QQ;

	
	// Construct the Gamma ( eq 74 and 73 )
	LatticeColorMatrix USigma = u[mu]*F_plus[mu];
	LatticeColorMatrix Gamma = f[1]*USigma + f[2]*(USigma*Q + Q*USigma)
	  + trace(B_1*USigma)*Q
	  + trace(B_2*USigma)*QQ;

	// Take the traceless hermitian part to form Lambda_mu (eq 72)
	Lambda[mu] = Gamma + adj(Gamma);    // Make it hermitian
	LatticeColorMatrix tmp3 = (Double(1)/Double(Nc))*trace(Lambda[mu]); // Subtract off the trace
	Lambda[mu] -= tmp3;
	Lambda[mu] *= Double(0.5);         // overall factor of 1/2

	// The first 3 terms of eq 75
	// Now the Fat force * the exp(iQ)
	F[mu]  = F_plus[mu]*(f[0] + f[1]*Q + f[2]*QQ);
           
      } // End of if( smear_in_this_dirP[mu] )
      // else what is in F_mu is the right force
    }
    
    // At this point we should have 
    //
    //  F[mu] = F_plus[mu]*exp(iQ) 
    //
    //  We need the 8 staple terms left in dOmega/dU (last 6 terms in eq 75 + the iC{+}Lambda
    //  term in eq 75 which in reality just covers 2 staples.
 
    //  We have to make this a separate loop from the above, because we need to know the 
    //  Lambda[mu] and [nu] for all the avaliable mu-nu combinations
    for(int mu = 0; mu < Nd; mu++) { 
      if( params.smear_in_this_dirP[mu] ) { 

	LatticeColorMatrix staple_sum = zero;
	// LatticeColorMatrix staple_sum_dag = adj(C[mu])*Lambda[mu];
	for(int nu = 0; nu < Nd; nu++) { 
	  if((mu != nu) && params.smear_in_this_dirP[nu] ) { 
	    LatticeColorMatrix U_nu_plus_mu = shift(u[nu],FORWARD, mu);
	    LatticeColorMatrix U_mu_plus_nu = shift(u[mu],FORWARD, nu);
	    LatticeColorMatrix Lambda_nu_plus_mu = shift(Lambda[nu], FORWARD, mu);
	    LatticeColorMatrix tmp_mat;
	    LatticeColorMatrix tmp_mat2;
	    
	    
	    //  THe three upward staples
	    //  Staples 1 5 and 6 in the paper
	    // 
	    //  Staple 1
	    //      rho(nu,mu)  *                ( [ U_nu(x+mu) U^+_mu(x+nu) ] U^+_nu(x) ) Lambda_nu(x)
	    //  Staple 5 
	    //    - rho(nu,mu) * Lambda_nu(x+mu)*( [ U_nu(x+mu) U^+_mu(x+nu) ] U^+_nu(x) )
	    //  Staple 6
	    //      rho(mu,nu) *                   [ U_nu(x+mu) U^+_mu(x+nu) ] Lambda_mu(x + nu) U^{+}_nu(x)
	    //
	    //
	    //  Here the suggestive [] are common to all three terms.
	    //  Also Staple 1 and 5 share the additional U^+_nu(x) as indicated by suggestive ()
	    //  Staple 1 and 5 also share the same rho(nu,mu) but have different sign
	    
	    {
	      LatticeColorMatrix tmp_mat3;
	      LatticeColorMatrix tmp_mat4;
	      
	      tmp_mat = U_nu_plus_mu*adj(U_mu_plus_nu); // Term in square brackets common to all
	      tmp_mat2 = tmp_mat*adj(u[nu]);            // Term in round brackets common to staples 1 and 5
	      tmp_mat3 = tmp_mat2*Lambda[nu];           // Staple 1
	      tmp_mat4 = Lambda_nu_plus_mu*tmp_mat2;
	      tmp_mat3 -= tmp_mat4;                     // Staple 5 and minus sign
	      tmp_mat3 *= params.rho(nu,mu);            // Common factor on staple 1 and 5
	      
	      tmp_mat4 = shift(Lambda[mu],FORWARD,nu);
	      tmp_mat2 = tmp_mat*tmp_mat4;                     // Staple 6
	      tmp_mat = tmp_mat2*adj(u[nu]);                   // and again
	      tmp_mat *= params.rho(mu,nu);                    // rho(mu, nu) factor on staple 6
	      tmp_mat += tmp_mat3;                          // collect staples 1 5 and 6 onto staple sum
	      staple_sum += tmp_mat;                           // slap onto the staple sum
	      
	      // Tmp 3 disappears here
	    }
	    
	    // The three downward staples
	    //
	    // Paper Staples 2, 3, and 4;
	    //
	    // Staple 2:
	    //     rho_mu_nu * U^{+}_nu(x-nu+mu) [  U^+_mu(x-nu) Lambda_mu(x-nu)     ] U_nu(x-nu)
	    // Staple 3
	    //     rho_nu_mu * U^{+}_nu(x-nu+mu) [ Lambda_nu(x-nu+mu) U^{+}_mu(x-nu) ] U_nu(x-nu)
	    // Staple 4
	    //   - rho_nu_mu * U^{+}_nu(x-nu+mu) [ U^{+}_mu(x-nu) Lambda_nu(x-nu)    ] U_nu(x-nu)
	    //
	    // I have suggestively placed brackets to show that all these staples share a common
	    // first and last term. Secondly staples 3 and 4 share the same value of rho (but opposite sign)
	    //
	    // Finally this can all be communicated on site and then shifted altoghether to x-nu
	    {
	      LatticeColorMatrix tmp_mat4;
	      
	      tmp_mat  = Lambda_nu_plus_mu*adj(u[mu]);      // Staple 3 term in brackets
	      tmp_mat4 = adj(u[mu])*Lambda[nu];
	      tmp_mat -= tmp_mat4;             // Staple 4 term in brackets and -ve sign
	      tmp_mat *= params.rho(nu,mu);                 // Staple 3 & 4 common rho value
	      tmp_mat2 = adj(u[mu])*Lambda[mu];             // Staple 2 term in brackets
	      tmp_mat2 *= params.rho(mu,nu);                // Staple 2 rho factor
	      tmp_mat += tmp_mat2;                          // Combine terms in brackets, signs and rho factors
	      
	      tmp_mat2 = adj(U_nu_plus_mu)*tmp_mat;         // Common first matrix
	      tmp_mat = tmp_mat2*u[nu];                     // Common last matrix
	      
	      staple_sum += shift(tmp_mat, BACKWARD, nu);   // Shift it all back to x-nu
	    }

	  } // end of if mu != nu
	} // end nu loop
  
	// Add on this term - there is a relative minus sign which will be corrected by the sign on 
	// on accumulation to F
	staple_sum -= adj(C[mu])*Lambda[mu];
  
	F[mu] -= timesI(staple_sum);
      } // End of if(smear_in_this_dirP[mu]
      // Else nothing needs done to the force
    } // end mu loop

  

    // Done
    END_CODE();
  }


  //! Drive the recursion for the force term
  //  Parameters: F is the force computed with the FAT Links
  void StoutFermState::fatForceToThin(const multi1d<LatticeColorMatrix>& F_fat, multi1d<LatticeColorMatrix>& F_thin) const
  {
    START_CODE();
   
    F_thin.resize(Nd);
    F_thin = F_fat;

    // Undo antiperiodic BC-s / force fixed BCs - this should really be unmodify
    // but essentially it works OK for everything except maybe twisted bc-s
    fbc->modify(F_thin);

    // Zero out fixed BCs
    fbc->zero(F_thin);

    // Now if the state is smeared recurse down.

    for(int level=params.n_smear; level > 0; level--) {
      deriv_recurse(F_thin,level-1);

      fbc->zero(F_thin);
    }


    END_CODE();
  }

  void StoutFermState::deriv(multi1d<LatticeColorMatrix>& F) const 
  {
    START_CODE();
    StopWatch swatch;
    swatch.reset();
    swatch.start();

    multi1d<LatticeColorMatrix> F_tmp(Nd);
    // Function resizes F_tmp
    fatForceToThin(F,F_tmp);
    

    // Multiply in by the final U term to close off the links
    for(int mu=0; mu < Nd; mu++) { 
      F[mu] = (smeared_links[0])[mu]*F_tmp[mu];
    }

    swatch.stop();
    StoutLinkTimings::force_secs += swatch.getTimeInSeconds();

    END_CODE();
  }

  void StoutFermState::smear_links(const multi1d<LatticeColorMatrix>& current, 
				   multi1d<LatticeColorMatrix>& next)
  {
    START_CODE();


    for(int mu = 0; mu < Nd; mu++) { 
      if( params.smear_in_this_dirP[mu] ) { 

	LatticeColorMatrix Q, QQ;
	LatticeColorMatrix C; // Dummy - not used here
	
	// Q contains the staple term. C is a throwaway
	getQsandCs(current, Q, QQ, C, mu);
	
	// Now compute the f's -- use the same function as for computing the fs, bs etc in derivative
	// but don't compute the b-'s
	multi1d<LatticeDComplex> f;   // routine will resize these
	multi1d<LatticeDComplex> b_1; // Dummy - not used      -- throwaway -- won't even get resized
	multi1d<LatticeDComplex> b_2; // Dummy - not used here -- throwaway -- won't even get resized
	
	
	getFsAndBs(Q,QQ,f,b_1,b_2,false);   // This routine computes the f-s
	
	// Assemble the stout links exp(iQ)U_{mu} 
	next[mu]=(f[0] + f[1]*Q + f[2]*QQ)*current[mu];      
      }
      else { 
	next[mu]=current[mu];  // Unsmeared
      }

    }
    END_CODE();
  }



  // Constructor only from a parameter structure
  StoutFermState::StoutFermState(Handle< FermBC<T,P,Q> > fbc_, 
				 const StoutFermStateParams& p_,
				 const multi1d<LatticeColorMatrix>& u_)
  {
    START_CODE();
    create(fbc_, p_, u_);
    END_CODE();
  }


  // create function
  void StoutFermState::create(Handle< FermBC<T,P,Q> > fbc_, 
			      const StoutFermStateParams& p_,
			      const multi1d<LatticeColorMatrix>& u_)
  { 
    START_CODE();
    StopWatch swatch;
    swatch.reset();
    swatch.start();

    fbc = fbc_;
    params = p_;

    // Allocate smeared and thin links
    smeared_links.resize(params.n_smear + 1);
    for(int i=0; i <= params.n_smear; i++) { 
      smeared_links[i].resize(Nd);
    }
    

    // Copy thin links into smeared_links[0]
    for(int mu=0; mu < Nd; mu++) { 
      (smeared_links[0])[mu] = u_[mu];
    }
    
    if( fbc->nontrivialP() ) {
      fbc->modify( smeared_links[0] );    
    }

    // Iterate up the smearings
    for(int i=1; i <= params.n_smear; i++) { 
 
 

      smear_links(smeared_links[i-1], smeared_links[i]);
      if( fbc->nontrivialP() ) {
	fbc->modify( smeared_links[i] );    
      }

    }

    // ANTIPERIODIC BCs only -- modify only top level smeared thing
    fat_links_with_bc.resize(Nd);
    fat_links_with_bc = smeared_links[params.n_smear];
    fbc->modify(fat_links_with_bc);


    swatch.stop();
    StoutLinkTimings::smearing_secs += swatch.getTimeInSeconds();
    END_CODE();
  }


} // End namespace Chroma
