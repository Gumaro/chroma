// $Id: eig_spec_bj_w.cc,v 1.4 2004-01-19 17:58:26 bjoo Exp $
/*! \file
 *  \brief Compute low lying eigenvalues of the hermitian H
 */


#include "chromabase.h"

#include <sstream>

using namespace std;

#include "meas/eig/eig_spec_bj_w.h"
#include "meas/eig/ritz.h"
#include "meas/eig/sn_jacob.h"


using namespace QDP;

//! Compute low lying eigenvalues of the hermitian H 
/*!
 * \ingroup eig
 *
 *  Compute low lying eigenvalues of the hermitian H 
 *  using the Ritz functional minimization routine,
 *  if desired in the Kalkreuter-Simma algorithm
 *  H		The operator        		(Read)
 *  lambda_H    Eigenvalues                     (Modify)
 *  psi		Eigenvectors			(Modify)
 *  N_eig	No of eigenvalues		(Read)
 *  N_min	Minimal CG iterations		(Read)
 *  N_max	Maximal CG iterations		(Read)
 *  Kalk_Sim	Use Kalkreuter-Simma criterion	(Read)
 *  n_renorm	Renormalize every n_renorm iter.	(Read)
 *  N_KS_max	Max number of Kalkreuter-Simma iterations	(Read)
 *  RsdR_r	(relative) residue		(Read)
 *  Cv_fact	"Convergence factor" required	(Read)
 *  NCG_tot	Total number of CG iter		(Write)
 *  n_KS	total number of Kalkreuter-Simma iterations	(Write)
 *  n_valid	number of valid eigenvalues	(Write)
 *  ProjApsiP	flag for projecting A.psi	(Read)
 */


void EigSpecRitzCG(const LinearOperator<LatticeFermion>& M, // Herm pos def operator
		 multi1d<Real>& lambda_H,            // E-values
		 multi1d<LatticeFermion>& psi,       // E-vectors
		 int n_eig,                        // No of e-values to find
		 int n_renorm,                      // renorm frequency
		 int n_min,                         // minimum iters / e_value
		 int MaxCG,                         // Max no of CG iters
		 const Real& Rsd_r,                 // relative residuum of each 
	     // e-value
		 const bool ProjApsiP,              // Project in Ritz?
	     
		 int& n_cg_tot,                     // Total no of CG iters
		 XMLBufferWriter& xml_out           // Diagnostics
	     )
{
  START_CODE("EigSpecRitzCG");
  
  
  push(xml_out, "EigSpecRitzCG");


  n_cg_tot = 0;
  int n_count = 0;
  int n_max = MaxCG+1;

  int n, i;
  Real final_grad;

  multi1d<Real> resid_rel(n_eig);
  for(n = 1, i = 0; n <= n_eig;  n++, i++) {

    // Initialise lambda[i] = 1
    lambda_H[i] = Real(1);
    
    // Do the Ritz
    Ritz(M, lambda_H[i], psi, n, Rsd_r, n_renorm, n_min, n_max, 	   
	 MaxCG, ProjApsiP, n_count, final_grad, false, Real(1), Real(1));

    // Add n_count
    n_cg_tot += n_count;
    
    // Check e-value
    LatticeFermion   D_e;
    LatticeFermion  lambda_e;
    M(D_e, psi[i], PLUS);
    lambda_e = lambda_H[i]*psi[i];
    D_e -= lambda_e;
    Double r_norm = sqrt(norm2(D_e));
    resid_rel[i] = Real(r_norm)/lambda_H[i];
    QDPIO::cout << "Evalue["<<n<<"]: eigen_norm = " << r_norm << " resid_rel = " << resid_rel[i] << endl << endl;
    // Dump info 
    ostringstream s;
    s << "eval" << n;
    push(xml_out, s.str());
    write(xml_out, "n_count", n_count);
    write(xml_out, "e_norm", r_norm);
    write(xml_out, "rel_e_norm", resid_rel[i]);
    pop(xml_out);
  }

  // All EV-s done. Dump-em
  push(xml_out, "eValues");
  write(xml_out, "lambda", lambda_H);
  pop(xml_out);
  write(xml_out, "n_cg_tot", n_cg_tot);

  pop(xml_out); // EigSpecRitz

      
  END_CODE("EigSpecRitzCG");
}


void EigSpecRitzKS(const LinearOperator<LatticeFermion>& M, // Herm pos def operator
		   multi1d<Real>& lambda_H,          // E-values
		   multi1d<LatticeFermion>& psi,     // E-vectors
		   int n_eig,                       // no of eig wanted
		   int n_dummy,                     // No of Dummy e-vals to use

		   int n_renorm,                    // renorm frequency
		   int n_min,                       // minimum iters / e_value      
		   int n_max,                       // max iters / e_value
		   int n_max_KS,                    // max KS cycles
		   const Real& gamma_factor,        // the KS gamma factor

		   int MaxCG,                       // Max no of CG iters
		   const Real& Rsd_r,               // relative residuum of each 
		                                  // e-value
		   const Real& Rsd_jacobi,          // Separate tolerance for Jacobi
		   const bool ProjApsiP,            // Project in Ritz?
		   
		   int& n_cg_tot,                   // Total no of CG iters
		   int& n_KS,                       // Total no of KS cycles
		   int& n_jacob_tot,
		   XMLBufferWriter& xml_out         // Diagnostics
	      )
{
  START_CODE("EigSpecRitzKS");

  // Sanity Checks: 
  // Make sure lambda_H is large enough
  if( lambda_H.size() < (n_eig+n_dummy) ) { 
    QDP_error_exit("lambda_H is too small to hold n_eig + n_dummy values\n");
  }

  // Make sure psi is large enough
  if( psi.size() < (n_eig + n_dummy) ) {
    QDP_error_exit("psi is too small to hold n_eig + n_dummy values\n");
  }

  // Make sure algorithm is applicable
  if( n_eig <=1 ) { 
    QDP_error_exit("Kalkreuter Simma does not work for just 1 e-value. Use EigSpecRitzCG instead");
  }

  // Internal lambda's
  // lambda_H holds initial guesses. Copy these over.
  int n_working_eig = n_eig+n_dummy;
  multi1d<Real> lambda_intern(n_working_eig);
  multi1d<Real> lambda_previous(n_working_eig);
  multi1d<Real> delta_cycle(n_working_eig);
  int i;
  for(i = 0; i < n_working_eig; i++) { 
    lambda_intern[i] = lambda_H[i];
    lambda_previous[i] = Real(1);
    delta_cycle[i] = Real(1);
  }

  // Off diag elements of hermitian matrix to diagonalise
  multi1d<Complex> off_diag((n_working_eig)*(n_working_eig-1)/2);

  
  multi1d<Real> final_grad(n_working_eig);

  int n_count = 0;
  n_cg_tot = 0;
  n_KS = 0;
  n_jacob_tot = 0;

  int ev, j, ij;
  int n_jacob;

  push(xml_out, "EigSpecRitzKS");
  
  bool convP = false;

  for( int KS_iter = 0; KS_iter < n_max_KS; KS_iter++) {
    n_KS++;

    push(xml_out, "KS_iter");

    // KS Step 1: for each k=0, n-1 in succession compute
    // approximations to the eigenvectors of M by performing
    // only a certain number of CG iterations (governed by gamma_factor)
    for(ev = 1, i=0; ev <= n_working_eig; ev++, i++) {

      // Do the Ritz for all working eigenvalues
      Ritz(M, lambda_intern[i], psi, ev, Rsd_r, n_renorm, n_min, n_max, 
	   MaxCG, ProjApsiP, n_count, final_grad[i], true, Real(1), 
	   gamma_factor);

      // Count the CG cycles
      n_cg_tot += n_count;
      push(xml_out, "ev");
      write(xml_out, "n_count", n_count);
      write(xml_out, "final_grad", final_grad[i]);
      pop(xml_out);

    }
    
    // Construct the hermitian matrix M to diagonalise.
    // Note only the off diagonal elements are needed
    // the lambda_intern[i] are the diagonal elements

    // M_ij = (psi_j, M psi_i )
    //
    LatticeFermion tmp;
    for(i=0, ij=0; i < n_working_eig; i++) { 
      M(tmp, psi[i], PLUS);
      for(j=0; j < i; j++) { 
	off_diag[ij] = innerProduct(psi[j], tmp);
	ij++;
      }
    }



    // Now diagonalise it, rotate evecs, and sort
    // 
    SN_Jacob(psi, n_working_eig, lambda_intern, off_diag, Rsd_jacobi, 50, n_jacob);
    n_jacob_tot += n_jacob;

    write(xml_out, "n_jacob", n_jacob);

    // delta_cycle = | mu - mu' | / (1 - gamma );
    // push(xml_out, "delta_cycle");
    // for(i=0; i < n_working_eig; i++) {
    // delta_cycle[i] = fabs( lambda_previous[i] - lambda_intern[i]);
    // delta_cycle[i] /= Real(1) - gamma_factor;
    //  write(xml_out, "dc", delta_cycle[i]);
    //  lambda_previous[i] = lambda_intern[i];
    // }
    // pop(xml_out);

    // Now we should check convergence
    // Pessimistic but safe criterion || g ||/lambda  < omega  sqrt(n_eig) 
    // we only check convergence of the wanted n_eig
    bool convP = true;
    for(i=0; i < n_eig; i++) { 
      convP &= ( toBool(final_grad[i] < Rsd_r*fabs(lambda_intern[i])));
      // convP = toBool(delta_cycle[i] < Rsd_r*fabs(lambda_intern[i]) );
    }

    pop(xml_out); // KS_iter

    // All the wanted e-values converged
    if( convP ) {

      // Do final Jacobi without the dummies.
      // Make the matrix
      for(i=0, ij=0; i < n_eig; i++) { 
	M(tmp, psi[i], PLUS);
	for(j=0; j < i; j++) { 
	  off_diag[ij] = innerProduct(psi[j], tmp);
	  ij++;
	}
      }

      // Diagonalise, rotate, sort
      SN_Jacob(psi, n_eig, lambda_intern, off_diag, Rsd_jacobi, 50, n_jacob);
      write(xml_out, "final_n_jacob", n_jacob);
      write(xml_out, "n_cg_tot", n_cg_tot);
      write(xml_out, "n_KS", n_KS);

      
      // Copy lambda_intern back into lambda_H and return
      for(i=0; i < n_eig; i++) { 
	lambda_H[i] = lambda_intern[i];
      }

      write(xml_out, "lambda_H", lambda_H);
      pop(xml_out); // EigSpecRitzKS
      END_CODE("EigSpecRitzKS");
      return;
    }

  }

  write(xml_out, "n_cg_tot", n_cg_tot);
  write(xml_out, "n_KS", n_KS);
  pop(xml_out); //EigSpecRitzKS
  
  // If we reached here then we have done more than n_max KS
  QDP_error_exit("n_max_KS reached with no convergence");
  END_CODE("EigSpecRitzKS");
}
