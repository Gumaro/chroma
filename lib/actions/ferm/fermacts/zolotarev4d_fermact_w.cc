// $Id: zolotarev4d_fermact_w.cc,v 1.18 2004-04-26 15:47:20 bjoo Exp $
/*! \file
 *  \brief 4D Zolotarev variant of Overlap-Dirac operator
 */
#include <iostream>
#include <sstream>
#include <chromabase.h>
#include <linearop.h>

#include "actions/ferm/fermacts/unprec_wilson_fermact_w.h"
#include "actions/ferm/fermacts/zolotarev4d_fermact_w.h"
#include "actions/ferm/fermacts/zolotarev.h"
#include "actions/ferm/linop/lovlapms_w.h"
#include "actions/ferm/linop/lovddag_w.h"
#include "actions/ferm/linop/lmdagm.h"
#include "meas/eig/ischiral_w.h"

using namespace std;


Zolotarev4DFermAct::Zolotarev4DFermAct(Handle<FermBC<LatticeFermion> > fbc_,
				       const Zolotarev4DFermActParams& params,
				       XMLWriter& writer_) : fbc(fbc_), m_q( params.Mass), RatPolyDeg(params.RatPolyDeg), RsdCGinner(params.RsdCGInner), MaxCGinner(params.MaxCGInner), writer(writer_), ReorthFreqInner(params.ReorthFreqInner)
{

  UnprecWilsonTypeFermAct<LatticeFermion>* S_aux;
  switch( params.AuxFermActHandle->getFermActType() ) {
  case FERM_ACT_WILSON:
    {
      // Upcast
      const WilsonFermActParams& wils = dynamic_cast<const WilsonFermActParams &>( *(params.AuxFermActHandle));

      //Get the FermAct
      S_aux = new UnprecWilsonFermAct(fbc, wils.Mass);
      if( S_aux == 0x0 ) { 
	QDPIO::cerr << "Unable to instantiate S_aux " << endl;
	QDP_abort(1);
      }
      
    }
    break;
  default:
    QDPIO::cerr << "Auxiliary Fermion Action Unsupported" << endl;
    QDP_abort(1);
  }
  
    
  // Drop AuxFermAct into a Handle immediately.
  // This should free things up at the end
  Handle<UnprecWilsonTypeFermAct<LatticeFermion> >  S_w(S_aux);
  Mact = S_w;
}


//! Creation routine
/*! */
void 
Zolotarev4DFermAct::init(int& numroot, 
			   Real& coeffP, 
			   multi1d<Real>& resP,
			   multi1d<Real>& rootQ, 
			   int& NEig, 
			   multi1d<Real>& EigValFunc,
			   const OverlapConnectState<LatticeFermion>& state ) const
{
  /* A scale factor which should bring the spectrum of the hermitian
     Wilson Dirac operator H into |H| < 1. */
  Real scale_fac;
  XMLBufferWriter my_writer;
  
  /* Contains all the data necessary for Zolotarev partial fraction */
  /* -------------------------------------------------------------- */
  zolotarev_data *rdata ;
  /* The lower (positive) interval bound for the approximation 
     interval [-1,-eps] U [eps,1] */

  Real eps;
  /* The type of the approximation R(x): 
     type = 0 -> R(x) = 0        at x = 0 
     type = 1 -> R(x) = infinity at x = 0 */

  int type;
  /* The maximal error of the approximation in the interval 
     [-1,-eps] U [eps,1]*/

  Real maxerr;

  // The residual for the solutions of the multi-shift linear system
  // I put this in the class constructor 

  //  RsdCGinner = 1.0e-7;  // Hardwired the accuracy


  /* Hermitian 4D overlap operator 1/2 ( 1 + m_q + (1 - m_q) gamma5 * sgn(H)) 
     using a partial fraction expansion of the optimal rational function
     approximation to sgn. Here, H = 1/2 * gamma_5 * (1/kappa - D'). 
     The coefficients are computed by Zolotarev's formula. */

  int NEigVal = state.getEigVal().size();

  /* The operator gamma_5 * M with the M constructed here has its eigenvalues
     in the range m/(m + Nd) <= |gamma_5 * M| <= (m + 2*Nd)/(m + Nd) (in the 
     free case) where here m is arbitrary.
     So if we multiply M by a factor scale_fac = (m + Nd)/(m + 2*Nd) we have
     |gamma_5 * M| <= 1. */
  if(NEigVal == 0 ) 
  {
    NEig = 0;
  }
  else
  {
      NEig = NEigVal - 1;
  }

  scale_fac = Real(1) / state.getApproxMax();
  eps = state.getApproxMin() * scale_fac;


  push(my_writer, "Zolotarev4D");
  write(my_writer, "MaxCGinner", MaxCGinner);
  write(my_writer, "RsdCGinner", RsdCGinner);
  write(my_writer, "NEigVal", NEigVal);
  write(my_writer, "NEig", NEig);

  /* Below, when we fill in the coefficents for the partial fraction, 
     we include this factor, say t, appropriately, i.e.
     R(x) = alpha[da] * t * x + sum(alpha[j] * t * x / (t^2 * x^2 - ap[j]), 
     j = 0 .. da-1)
     = (alpha[da] + + sum(alpha[j] / (x^2 - ap[j] / t^2) ) / t^2 ) * t * x 
  */
  
  /* ZOLOTAREV_4D uses Zolotarev's formula for the coefficients. 
     The coefficents produced are for an optimal uniform approximation
     to the sign-function in the interval [-1,-eps] U [eps,1] and of order n. 
     type can be set to 0 or 1 corresponding to an approximation which is 
     is zero or infinite at x = 0, respectively. 
     Here we are interested in the partial fraction form 
     
     R(x) = alpha[da] * x + sum(alpha[j] * x / (x^2 - ap[j]), j = 0 .. da-1) 
     
     where da = dd for type 0 and da = dd + 1 with ap[dd] = 0 for type 1. 
  */
  type = 0;
  rdata = zolotarev(toFloat(eps), RatPolyDeg, type);
  maxerr = (Real)(rdata -> Delta);
  
  push(my_writer, "ZolotarevApprox");
  write(my_writer, "eps", eps);
  write(my_writer, "scale_fac", scale_fac);
  write(my_writer, "RatPolyDeg", RatPolyDeg);
  write(my_writer, "type", type);
  write(my_writer, "maxerr", maxerr);
  pop(my_writer);
  
  /* The number of residuals and poles */
  /* Allocate the roots and residua */
  numroot = rdata -> dd;
  /* The roots, i.e., the shifts in the partial fraction expansion */
  rootQ.resize(numroot);
  
  /* The residuals in the partial fraction expansion */
  resP.resize(numroot);
  
  /* Fill in alpha[0] = alpha[da] if it is not zero*/
  coeffP = 0;
  coeffP = rdata -> alpha[rdata -> da - 1];
  /* The coefficients from the partial fraction.
     Here, we write them out for the sake of bookkeeping. */
  resP = 0;
  rootQ = 0;
  for(int n=0; n < numroot; ++n) {
    resP[n] = rdata -> alpha[n];
    rootQ[n] = rdata -> ap[n];
    rootQ[n] = -rootQ[n];
  }
  
  
  push(my_writer,"ZolotarevPartFrac");
  write(my_writer, "scale_fac", scale_fac);
  write(my_writer, "coeffP", coeffP);
  write(my_writer, "resP", resP);
  write(my_writer, "rootQ", rootQ);
  pop(my_writer);
  
  /* Now fill in the coefficients for real, i.e., taking the rescaling
     into account */
  /* Fill in alpha[0] = alpha[da] if it is not zero*/
  coeffP = rdata -> alpha[rdata -> da - 1] * scale_fac;
  /* Fill in the coefficients for the roots and the residua */
  /* Make sure that the smallest shift is in the last value rootQ(numroot-1)*/
  resP = 0;
  rootQ = 0;
  Real t = Real(1) / (scale_fac * scale_fac);
  for(int n=0; n < numroot; ++n) {
    
    resP[n] = rdata -> alpha[n] / scale_fac;
    rootQ[n] = rdata -> ap[n];
    rootQ[n] = -(rootQ[n] * t);
  }
  
  
  /* Write them out into the namelist */
  push(my_writer,"ZolotarevPartFracResc");
  write(my_writer, "scale_fac", scale_fac);
  write(my_writer, "coeffP", coeffP);
  write(my_writer, "resP", resP);
  write(my_writer, "rootQ", rootQ);
  pop(my_writer);
  
  pop(my_writer);
  
  
  QDPIO::cout << "ZOLOTAREV 4d n=" << RatPolyDeg << " scale=" << scale_fac
	      << " coeff=" << coeffP << " Nwils= " << NEigVal <<" m_q="
	      << m_q << " Rsd=" << RsdCGinner << endl;
  
  QDPIO::cout << "Approximation on [-1,-eps] U [eps,1] with eps = " << eps <<
    endl;
  QDPIO::cout << "Maximum error |R(x) - sqn(x)| <= " << maxerr << endl;
  
  if(type == 0) {
    QDPIO::cout << "Approximation type " << type << " with R(0) = 0"
		<< endl;
  }
  else {
    QDPIO::cout << "Approximation type " << type << " with R(0) =  infinity"                    << endl;
  }
  
  /* We will also compute the 'function' of the eigenvalues */
  /* for the Wilson vectors to be projected out. */
  if (NEig > 0)
  {
    for(int i = 0; i < NEigVal; i++)
    {
      if (toBool(state.getEigVal()[i] > 0.0))
	EigValFunc[i] = 1.0;
      else if (toBool(state.getEigVal()[i] < 0.0))
	EigValFunc[i] = -1.0;
      else
	EigValFunc[i] = 0.0;
    }
  }

  writer << my_writer;

  // Free the arrays allocate by Tony's zolo
  free( rdata->a );
  free( rdata->ap );
  free( rdata->alpha );
  free( rdata->beta );
  free( rdata );

}

//! Produce a linear operator for this action
/*!
 * The operator acts on the entire lattice
 *
 * \param state_	 gauge field state  	 (Read)
 */
const LinearOperator<LatticeFermion>* 
Zolotarev4DFermAct::linOp(Handle<const ConnectState> state_) const
{
  START_CODE("Zolotarev4DLinOp::create");

  const OverlapConnectState<LatticeFermion>& state = dynamic_cast<const OverlapConnectState<LatticeFermion>&>(*state_);

  if (state.getEigVec().size() != state.getEigVal().size())
    QDP_error_exit("Zolotarev4DLinOp: inconsistent sizes of eigenvectors and values");

  int NEigVal = state.getEigVal().size();

  /* The actual number of eigenvectors to project out.
     The highest of the valid low eigenmodes is not
     projected out. So we will put NEig = NEigVal - 1 */  
  int NEig;

  /* The number of residuals and poles */
  int numroot;

  /* The roots, i.e., the shifts in the partial fraction expansion */
  multi1d<Real> rootQ;

  /* The residuals in the partial fraction expansion */
  multi1d<Real> resP;

  /* This will be our alpha(0) which can be 0 depending on type */
  /* an even- or oddness of RatPolyDeg*/
  Real coeffP; 

  /* Array of values of the sign function evaluated on the eigenvectors of H */
  multi1d<Real> EigValFunc(NEigVal);

  // Common initialization
  init(numroot, coeffP, resP, rootQ, NEig, EigValFunc, state);

  
  /* Finally construct and pack the operator */
  /* This is the operator of the form (1/2)*[(1+mu) + (1-mu)*gamma_5*eps] */
  
  return new lovlapms(*Mact, state_, m_q,
		      numroot, coeffP, resP, rootQ, 
		      NEig, EigValFunc, state.getEigVec(),
		      MaxCGinner, RsdCGinner, ReorthFreqInner);
  
  END_CODE("Zolotarev4DLinOp::create");
}

//! Produce a linear operator for this action
/*!
 * The operator acts on the entire lattice
 *
 * \param state_	 gauge field state  	 (Read)
 */
const LinearOperator<LatticeFermion>* 
Zolotarev4DFermAct::lMdagM(Handle<const ConnectState> state_, const Chirality& ichiral) const
{

  // If chirality is none, return traditional MdagM
  if ( ichiral == CH_NONE ) {
    return lMdagM(state_);
  }
  else { 
    const OverlapConnectState<LatticeFermion>& state = dynamic_cast<const OverlapConnectState<LatticeFermion>&>(*state_);
    
    if (state.getEigVec().size() != state.getEigVal().size())
      QDP_error_exit("Zolotarev4DLinOp: inconsistent sizes of eigenvectors and values");

    int NEigVal = state.getEigVal().size();

    /* The actual number of eigenvectors to project out.
       The highest of the valid low eigenmodes is not
       projected out. So we will put NEig = NEigVal - 1 */  
    int NEig;
    
    /* The number of residuals and poles */
    int numroot;

    /* The roots, i.e., the shifts in the partial fraction expansion */
    multi1d<Real> rootQ;
    
    /* The residuals in the partial fraction expansion */
    multi1d<Real> resP;
    
    /* This will be our alpha(0) which can be 0 depending on type */
    /* an even- or oddness of RatPolyDeg*/
    Real coeffP; 
    
    /* Array of values of the sign function evaluated on the eigenvectors of H */
    multi1d<Real> EigValFunc(NEigVal);
    
  // Common initialization
    init(numroot, coeffP, resP, rootQ, NEig, EigValFunc, state);

  
    // Finally construct and pack the operator 
    // This is the operator of the form (1/2)*[(1+mu) + (1-mu)*gamma_5*eps]
    return new lovddag(*Mact, state_, m_q,
		       numroot, coeffP, resP, rootQ, 
		       NEig, EigValFunc, state.getEigVec(),
		       MaxCGinner, RsdCGinner, ReorthFreqInner, ichiral);
  
  }
  END_CODE("Zolotarev4DlMdagM");
}

//! Produce a conventional MdagM operator for this action
/*!
 * This is the operator which you can always use
 * The operator acts on the entire lattice
 *
 * \param state_	 gauge field state  	 (Read)
 */
const LinearOperator<LatticeFermion>* 
Zolotarev4DFermAct::lMdagM(Handle<const ConnectState> state_) const
{
  // linOp news the linear operator and gives back pointer, 
  // We call lmdagm with this pointer.
  // lmdagm is the only owner
  // No need to grab linOp with handle at this stage.
  return new lmdagm<LatticeFermion>( linOp(state_) );
}


const OverlapConnectState<LatticeFermion>*
Zolotarev4DFermAct::createState(const multi1d<LatticeColorMatrix>& u_,
				  const Real& approxMin_) const 
{
  if ( toBool( approxMin_ < Real(0) )) { 
    ostringstream error_str;
    error_str << "Zolotarev4DFermAct: approxMin_ has to be positive" << endl;
    throw error_str.str();
  }
 

  // First put in the BC
  multi1d<LatticeColorMatrix> u_tmp = u_;
  getFermBC().modifyU(u_tmp);

  Real approxMax = Real(2)*Real(Nd);
  return new OverlapConnectState<LatticeFermion>(u_tmp, approxMin_, approxMax);
}


const OverlapConnectState<LatticeFermion>*
Zolotarev4DFermAct::createState(const multi1d<LatticeColorMatrix>& u_,
				  const Real& approxMin_,
				  const Real& approxMax_) const
{
  ostringstream error_str;
  
 
  if ( toBool(approxMin_ < 0 )) { 
    error_str << "Zolotarev4DFermAct: approxMin_ has to be positive" << endl;
    throw error_str.str();
  }

  if ( toBool(approxMax_ < approxMin_) ) { 
    error_str << "Zolotarev4DFermAct: approxMax_ has to be larger than approxMin_" << endl;
    throw error_str.str();
  }
 

  // First put in the BC
  multi1d<LatticeColorMatrix> u_tmp = u_;
  getFermBC().modifyU(u_tmp);

  return new OverlapConnectState<LatticeFermion>(u_tmp, approxMin_, approxMax_);
}



const OverlapConnectState<LatticeFermion>*
Zolotarev4DFermAct::createState(const multi1d<LatticeColorMatrix>& u_,
				  const multi1d<Real>& lambda_lo_, 
				  const multi1d<LatticeFermion>& evecs_lo_,
				  const Real& lambda_hi_) const
{
  ostringstream error_str;

  if ( lambda_lo_.size() == 0 ) {
    error_str << "Attempt to createState with 0 e-values and no approxMin" << endl;
    throw error_str.str();
  }

  if ( lambda_lo_.size() != evecs_lo_.size() ) {
    error_str << "Attempt to createState with no of low eigenvalues != no of low eigenvectors" << endl;
    throw error_str.str();
  }

  Real approxMax = lambda_hi_;
  Real approxMin = fabs(lambda_lo_[ lambda_lo_.size() - 1 ]);

  // First put in the BC
  multi1d<LatticeColorMatrix> u_tmp = u_;
  getFermBC().modifyU(u_tmp);

  return new OverlapConnectState<LatticeFermion>(u_tmp, lambda_lo_, evecs_lo_, lambda_hi_, approxMin, approxMax);
}

  

const OverlapConnectState<LatticeFermion>*
Zolotarev4DFermAct::createState(const multi1d<LatticeColorMatrix>& u_,
				const OverlapStateInfo& state_info,
				XMLWriter& xml_out,
				Real wilsonMass) const
{
  push(xml_out, "Zolo4DCreateState");


  // If No eigen values specified use min and max
  if ( state_info.getNWilsVec() == 0 ) { 
    write(xml_out, "ApproxMin", state_info.getApproxMin());
    write(xml_out, "ApproxMax", state_info.getApproxMax());
    pop(xml_out);

    return createState(u_,
		       state_info.getApproxMin(),
		       state_info.getApproxMax());
  }
  else {

    // If there are eigen values, either load them, 
    if( state_info.loadEigVec() ) {
      ChromaWilsonRitz_t ritz_header;
      multi1d<Real> lambda_lo;
      multi1d<LatticeFermion> eigv_lo;
      Real lambda_hi;
      const EigenIO_t& eigen_io = state_info.getEigenIO();

      push(xml_out, "EigenSystem");
      if( eigen_io.eigen_filefmt == EVEC_TYPE_SCIDAC ) { 
	readEigen(ritz_header, lambda_lo, eigv_lo, lambda_hi, 
		  eigen_io.eigen_file,
		  state_info.getNWilsVec(),
		QDPIO_SERIAL);
	write(xml_out, "OriginalRitzHeader", ritz_header);
      }
      else if ( eigen_io.eigen_filefmt == EVEC_TYPE_SZIN ) { 

	if( toBool( fabs(wilsonMass) > 8 ) ){
	  QDPIO::cerr << "WilsonMass unspecified, or | WilsonMass | > 8" << endl;
	  QDPIO::cerr << "The wilson mass is needed to rescale the eigenvalues" << endl;
	  QDP_abort(1);
	}

	readEigenSzin(lambda_lo, eigv_lo, lambda_hi, state_info.getNWilsVec(), eigen_io.eigen_file);
	
	// Now I need to scale things by the wilson mass (Nd + m)
	for(int i=0; i < lambda_lo.size(); i++) { 
	  lambda_lo[i] *= (Real(Nd) + wilsonMass);
	}

	lambda_hi *= (Real(Nd) + wilsonMass);

      }
      else {
	QDPIO::cerr << "Unsupported Eigenvector format for reading " << endl;
	QDP_abort(1);
      }

      write(xml_out, "lambda_lo", lambda_lo);
      write(xml_out, "lambda_high", lambda_hi);
      
      Handle< const ConnectState > wils_connect_state = Mact->createState(u_);
      Handle< const LinearOperator<LatticeFermion> > H = Mact->gamma5HermLinOp(wils_connect_state);

      	      
      multi1d<Double> check_norm(state_info.getNWilsVec());
      multi1d<Double> check_norm_rel(state_info.getNWilsVec());
      for(int i=0; i < state_info.getNWilsVec() ; i++) { 
	LatticeFermion Me;
	(*H)(Me, eigv_lo[i], PLUS);
	
	LatticeFermion lambda_e;
	
	lambda_e = lambda_lo[i]*eigv_lo[i];
	LatticeFermion r_norm = Me - lambda_e;
	check_norm[i] = sqrt(norm2(r_norm));
	check_norm_rel[i] = check_norm[i]/fabs(Double(lambda_lo[i]));
	
	QDPIO::cout << "Eigenpair " << i << " Resid Norm = " 
		    << check_norm[i] << " Resid Rel Norm = " << check_norm_rel[i] << endl;
      }
      write(xml_out, "eigen_norm", check_norm);
      write(xml_out, "eigen_rel_norm", check_norm_rel);
      
      pop(xml_out); // Eigensystem

      pop(xml_out); // Zolo4DCreateState
      return createState(u_, lambda_lo, eigv_lo, lambda_hi);
    }
    else if( state_info.computeEigVec() ) {
      QDPIO::cerr << "Recomputation of eigensystem not yet implemented" << endl;
      QDP_abort(1);
    }
    else {
      QDPIO::cerr << "I have to create a state without min/max, loading/computing eigenvectors/values. How do I do that? "<< endl;
      QDP_abort(1);
    }
  }
}
