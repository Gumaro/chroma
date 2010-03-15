// $Id: t_invert4_precwilson.cc,v 3.4 2009-10-09 13:59:46 bjoo Exp $

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include <cstdio>

#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#include "chroma.h"
#include <string>
using namespace Chroma;
using namespace std;

typedef LatticeFermionF  TF;
typedef LatticeColorMatrixF  UF;
typedef multi1d<LatticeColorMatrixF> QF;
typedef multi1d<LatticeColorMatrixF> PF;

#include "quda.h"
#include "util_quda.h"

bool testQudaDslash3D(const QF& u, enum PlusMinus isign, int cb)
{
  // I want to test the QUDA dslash
  // First make a reference dslash: 4D for now
  multi1d<int> boundary(4); boundary[0]=boundary[1]=boundary[2]=1;
  boundary[3]=-1;

  Handle< FermBC<TF,PF,QF> > bc_handle(new SimpleFermBC<TF,PF,QF>(boundary));

  Handle< FermState<TF,PF,QF> > fs_handle(new SimpleFermState<TF,PF,QF>(bc_handle, u));

  WilsonDslash3D D_me(fs_handle);

  // Now set up a QUDA Dslash
  // ----------**************************--------------
  QudaGaugeParam q_gauge_param=newQudaGaugeParam();
  QudaInvertParam quda_inv_param=newQudaInvertParam();

  QudaPrecision_s cpu_prec=QUDA_SINGLE_PRECISION;
  QudaPrecision_s gpu_prec=QUDA_SINGLE_PRECISION;
  QudaPrecision_s gpu_half_prec=QUDA_SINGLE_PRECISION;

  // Setup Boundaries
  if(boundary[3]==-1){ 
    q_gauge_param.t_boundary = QUDA_ANTI_PERIODIC_T;
  }
  else { 
    q_gauge_param.t_boundary = QUDA_PERIODIC_T;
  }

  // Setup Gauge Order 
  q_gauge_param.gauge_order = QUDA_QDP_GAUGE_ORDER;
  q_gauge_param.reconstruct = QUDA_RECONSTRUCT_12;
  q_gauge_param.reconstruct_sloppy = QUDA_RECONSTRUCT_12;

  // Setup precisions
  q_gauge_param.cpu_prec=cpu_prec;
  q_gauge_param.cuda_prec=gpu_prec;
  q_gauge_param.cuda_prec_sloppy=gpu_half_prec;

  // Setup lattice size
  const multi1d<int>& latdims = Layout::lattSize();      
  q_gauge_param.X[0] = latdims[0];
  q_gauge_param.X[1] = latdims[1];
  q_gauge_param.X[2] = latdims[2];
  q_gauge_param.X[3] = latdims[3];
  
  // Setup gauge fixing
  q_gauge_param.gauge_fix = QUDA_GAUGE_FIXED_NO;

  // Setup padding
  unsigned int vol = latdims[0]*latdims[1]*latdims[2]*latdims[3];
  unsigned int vol2 = vol/2;
  if( vol % (1<<14) == 0) { 
    // Commended out -- padding this way seems to cause some divergence
    // Will revert when Mike tells me to. For now
    // I'll take the hit
    //quda_inv_param.sp_pad = q_gauge_param.ga_pad = quda_inv_param.cl_pad =( latdims[0]*latdims[1]*latdims[2])/2;
    quda_inv_param.sp_pad = q_gauge_param.ga_pad = quda_inv_param.cl_pad =0;
    
  }
  else {
    quda_inv_param.sp_pad = q_gauge_param.ga_pad = quda_inv_param.cl_pad =0;
  }

  // Setup gauge anisotropy
  q_gauge_param.anisotropy = 1.0;

  
  // Make the links
  QF links_single(Nd);
  
  // Repack gauge, so that for links_single[mu] has gauge field with 3d cb being contiguous
  int lin;
  for(int mu=0; mu < Nd; mu++) {
    lin=0;
    for(int cb3=0; cb3 < 2; cb3++) { 
      const int* tab=rb3[cb3].siteTable().slice();

      for(int site=0; site < vol2; site++) {
	for(int r=0; r < 3; r++) { 
	  for(int c=0; c < 3; c++) { 
	    links_single[mu].elem(lin).elem().elem(r,c).real() = ((fs_handle->getLinks())[mu]).elem(tab[site]).elem().elem(r,c).real();
	    links_single[mu].elem(lin).elem().elem(r,c).imag() = ((fs_handle->getLinks())[mu]).elem(tab[site]).elem().elem(r,c).imag();

	  }
	}
	lin++;
      }
    }
  }
  // Set up the links
  void* gauge[4];
  for(int mu=0; mu < Nd; mu++) { 
    gauge[mu] = (void *)&(links_single[mu].elem(all.start()).elem().elem(0,0).real());
  }
  loadGaugeQuda((void *)gauge, &q_gauge_param);
  
  // Now set up inverter params.
  quda_inv_param.dslash_type = QUDA_WILSON_DSLASH; // Sets Wilson Matrix
  quda_inv_param.cpu_prec = cpu_prec;
  quda_inv_param.cuda_prec = gpu_prec;
  quda_inv_param.cuda_prec_sloppy = gpu_half_prec;
  quda_inv_param.dirac_order = QUDA_DIRAC_ORDER;

  TF src, dst1,dst2;
  gaussian(src);
  gaussian(dst1); // Junk these
  gaussian(dst2);
  D_me.apply(dst1,src,isign,cb); // fills out dst[rb[0]]

  // Compute buffers for transfer to GPU

  unsigned int h_size=4*3*2*vol2;
  multi1d<float> buffer_in(h_size);
  multi1d<float> buffer_out(h_size);



  int otherCB=1-cb;
  int daggerBit = (isign == MINUS ? 1 : 0);

  // Pack buffer_in: output cb = rb3[0], input cb=rb3[1]
  lin=0;
  for(int site=0; site < rb3[1].siteTable().size(); site++) { 
    for(int spin=0; spin < 4; spin++) {		      
      for(int col=0; col < 3; col++) {
	buffer_in[lin++] = src.elem(rb3[otherCB].siteTable()[site]).elem(spin).elem(col).real();
	buffer_in[lin++] = src.elem(rb3[otherCB].siteTable()[site]).elem(spin).elem(col).imag();
      }
    }
  }


  dslash3DQuda((void *)buffer_out.slice(),
	       (void *)buffer_in.slice(),
	       &quda_inv_param,
	       cb,      // source parity =1, dst parity=0
	       daggerBit);     // no dagger

  // Unpack
  lin=0;
  for(int site=0; site < rb3[1].siteTable().size(); site++) { 
    for(int spin=0; spin < 4; spin++) {		      
      for(int col=0; col < 3; col++) {
	dst2.elem(rb3[cb].siteTable()[site]).elem(spin).elem(col).real() = buffer_out[lin++];
	dst2.elem(rb3[cb].siteTable()[site]).elem(spin).elem(col).imag() = buffer_out[lin++];
      }
    }
  }

  TF diff=zero;
  diff[rb3[cb]] = dst1-dst2;

  // Free QUDA data structures
  Double diff_norm = sqrt(norm2(diff,rb3[cb]))/Double(vol2);
  
  bool ret_val;
  if ( toBool( diff_norm < Double(2.0e-8)  ) ) {
    ret_val = true;
  }
  else {
    ret_val = false; 
  }

  return ret_val;
}


bool testQudaDslash(const QF& u, enum PlusMinus isign, int cb)
{
  // I want to test the QUDA dslash
  // First make a reference dslash: 4D for now
  multi1d<int> boundary(4); boundary[0]=boundary[1]=boundary[2]=1;
  boundary[3]=-1;

  Handle< FermBC<TF,PF,QF> > bc_handle(new SimpleFermBC<TF,PF,QF>(boundary));

  Handle< FermState<TF,PF,QF> > fs_handle(new SimpleFermState<TF,PF,QF>(bc_handle, u));

  WilsonDslashF D_me(fs_handle);

  // Now set up a QUDA Dslash
  // ----------**************************--------------
  QudaGaugeParam q_gauge_param=newQudaGaugeParam();
  QudaInvertParam quda_inv_param=newQudaInvertParam();

  QudaPrecision_s cpu_prec=QUDA_SINGLE_PRECISION;
  QudaPrecision_s gpu_prec=QUDA_SINGLE_PRECISION;
  QudaPrecision_s gpu_half_prec=QUDA_SINGLE_PRECISION;

  // Setup Boundaries
  if(boundary[3]==-1){ 
    q_gauge_param.t_boundary = QUDA_ANTI_PERIODIC_T;
  }
  else { 
    q_gauge_param.t_boundary = QUDA_PERIODIC_T;
  }

  // Setup Gauge Order 
  q_gauge_param.gauge_order = QUDA_QDP_GAUGE_ORDER;
  q_gauge_param.reconstruct = QUDA_RECONSTRUCT_12;
  q_gauge_param.reconstruct_sloppy = QUDA_RECONSTRUCT_12;

  // Setup precisions
  q_gauge_param.cpu_prec=cpu_prec;
  q_gauge_param.cuda_prec=gpu_prec;
  q_gauge_param.cuda_prec_sloppy=gpu_half_prec;

  // Setup lattice size
  const multi1d<int>& latdims = Layout::lattSize();      
  q_gauge_param.X[0] = latdims[0];
  q_gauge_param.X[1] = latdims[1];
  q_gauge_param.X[2] = latdims[2];
  q_gauge_param.X[3] = latdims[3];
  
  // Setup gauge fixing
  q_gauge_param.gauge_fix = QUDA_GAUGE_FIXED_NO;

  // Setup padding
  unsigned int vol = latdims[0]*latdims[1]*latdims[2]*latdims[3];
  if( vol % (1<<14) == 0) { 
    // Commended out -- padding this way seems to cause some divergence
    // Will revert when Mike tells me to. For now
    // I'll take the hit
    //quda_inv_param.sp_pad = q_gauge_param.ga_pad = quda_inv_param.cl_pad =( latdims[0]*latdims[1]*latdims[2])/2;
    quda_inv_param.sp_pad = q_gauge_param.ga_pad = quda_inv_param.cl_pad =0;
    
  }
  else {
    quda_inv_param.sp_pad = q_gauge_param.ga_pad = quda_inv_param.cl_pad =0;
  }

  // Setup gauge anisotropy
  q_gauge_param.anisotropy = 1.0;


  // Make the links
  QF links_single(Nd);
  
  for(int mu=0; mu < Nd; mu++) {
    links_single[mu] = (fs_handle->getLinks())[mu];
  }
  // Set up the links
  void* gauge[4];
  for(int mu=0; mu < Nd; mu++) { 
    gauge[mu] = (void *)&(links_single[mu].elem(all.start()).elem().elem(0,0).real());
  }
  loadGaugeQuda((void *)gauge, &q_gauge_param);
  
  // Now set up inverter params.
  quda_inv_param.dslash_type = QUDA_WILSON_DSLASH; // Sets Wilson Matrix
  quda_inv_param.cpu_prec = cpu_prec;
  quda_inv_param.cuda_prec = gpu_prec;
  quda_inv_param.cuda_prec_sloppy = gpu_half_prec;
  quda_inv_param.dirac_order = QUDA_DIRAC_ORDER;

  TF src, dst1,dst2;
  gaussian(src);
  gaussian(dst1); // Junk these
  gaussian(dst2);
  D_me.apply(dst1,src,isign,cb);

  // Compute buffers for transfer to GPU
  unsigned int vol2 = vol/2;
  unsigned int h_size=4*3*2*vol2;
  multi1d<float> buffer_in(h_size);
  multi1d<float> buffer_out(h_size);



  int otherCB=1-cb;
  int daggerBit = (isign == MINUS ? 1 : 0);

  // Pack buffer_in: output cb = rb[0], input cb=rb[1]
  int lin=0;
  for(int site=0; site < rb[1].siteTable().size(); site++) { 
    for(int spin=0; spin < 4; spin++) {		      
      for(int col=0; col < 3; col++) {
	buffer_in[lin++] = src.elem(rb[otherCB].siteTable()[site]).elem(spin).elem(col).real();
	buffer_in[lin++] = src.elem(rb[otherCB].siteTable()[site]).elem(spin).elem(col).imag();
      }
    }
  }


  dslashQuda((void *)buffer_out.slice(),
	     (void *)buffer_in.slice(),
	     &quda_inv_param,
	     cb,      // source parity =1, dst parity=0
	     daggerBit);     // no dagger

  // Unpack
  lin=0;
  for(int site=0; site < rb[1].siteTable().size(); site++) { 
    for(int spin=0; spin < 4; spin++) {		      
      for(int col=0; col < 3; col++) {
	dst2.elem(rb[cb].siteTable()[site]).elem(spin).elem(col).real() = buffer_out[lin++];
	dst2.elem(rb[cb].siteTable()[site]).elem(spin).elem(col).imag() = buffer_out[lin++];
      }
    }
  }

  TF diff=zero;
  diff[rb[cb]] = dst1-dst2;

  // Free QUDA data structures
  Double diff_norm = sqrt(norm2(diff,rb[cb]))/Double(vol2);
  
  bool ret_val;
  if ( toBool( diff_norm < Double(2.0e-8)  ) ) {
    ret_val = true;
  }
  else {
    ret_val = false; 
  }

  return ret_val;
}

bool linkageHack(void)
{
  bool foo = true;

  // Inline Measurements
  foo &= InlineAggregateEnv::registerAll();
  foo &= GaugeInitEnv::registerAll();

  return foo;
}

int main(int argc, char **argv)
{
  // Put the machine into a known state
  Chroma::initialize(&argc, &argv);
  QDPIO::cout << "Linkage = " << linkageHack() << endl;


  //  AppParams params;
  const int lsize[4]={12,12,12,32};
  multi1d<int> nrow(4);
  nrow=lsize;
  Layout::setLattSize(nrow);
  Layout::create();
  
  multi1d<LatticeColorMatrix> u(Nd);
#if 0
  XMLReader gauge_file_xml, gauge_xml;
  Cft_t inputCfg;
  inputCfg.cfg_type=WEAK_FIELD;
  gaugeStartup(gauge_file_xml, gauge_xml, u, params.inputCfg);
#endif
  for(int mu=0; mu < Nd; mu++){ 
    gaussian(u[mu]);
    reunit(u[mu]);
  }

  unitarityCheck(u);

  // Setup the lattice
 
  XMLFileWriter& xml_out = Chroma::getXMLOutputInstance();
  push(xml_out,"t_invert");
  push(xml_out,"params");
  write(xml_out, "nrow", nrow);
  pop(xml_out); // Params

  // Measure the plaquette on the gauge
  MesPlq(xml_out, "Observables", u);
  xml_out.flush();

  // Write code here?
  QDPIO::cout << "Howdy" << endl;
  bool result;


  QDPIO::cout << "Test: Dslash PLUS, cb=0";
  if ( ! testQudaDslash(u, PLUS, 0)  ) { 
    QDPIO::cout << "\t FAILED" << endl;
    QDP_abort(1);
  }
  else { 
    QDPIO::cout << "\t OK" << endl;
  }

  QDPIO::cout << "Test: Dslash MINUS, cb=0";
  if ( ! testQudaDslash(u, MINUS, 0)  ) { 
    QDPIO::cout << "\t FAILED" << endl;
    QDP_abort(1);
  }
  else { 
    QDPIO::cout << "\t OK" << endl;
  }

  QDPIO::cout << "Test: Dslash PLUS, cb=1" ;
  if ( ! testQudaDslash(u, PLUS, 1)  ) { 
    QDPIO::cout << "\t FAILED" << endl;
    QDP_abort(1);
  }
  else { 
    QDPIO::cout << "\t OK" << endl;
  }

  QDPIO::cout << "Test: Dslash MINUS, cb=1" ;
  if ( ! testQudaDslash(u, MINUS, 1)  ) { 
    QDPIO::cout << "\t FAILED" << endl;
    QDP_abort(1);
  }
  else { 
    QDPIO::cout << "\t OK" << endl;
  }

  // 3D Dslash testsd

  QDPIO::cout << "Test: Dslash3D PLUS, cb=0";
  if ( ! testQudaDslash3D(u, PLUS, 0)  ) { 
    QDPIO::cout << "\t FAILED" << endl;
    QDP_abort(1);
  }
  else { 
    QDPIO::cout << "\t OK" << endl;
  }

  QDPIO::cout << "Test: Dslash3D MINUS, cb=0";
  if ( ! testQudaDslash3D(u, MINUS, 0)  ) { 
    QDPIO::cout << "\t FAILED" << endl;
    QDP_abort(1);
  }
  else { 
    QDPIO::cout << "\t OK" << endl;
  }

  QDPIO::cout << "Test: Dslash3D PLUS, cb=1" ;
  if ( ! testQudaDslash3D(u, PLUS, 1)  ) { 
    QDPIO::cout << "\t FAILED" << endl;
    QDP_abort(1);
  }
  else { 
    QDPIO::cout << "\t OK" << endl;
  }

  QDPIO::cout << "Test: Dslash3D MINUS, cb=1" ;
  if ( ! testQudaDslash3D(u, MINUS, 1)  ) { 
    QDPIO::cout << "\t FAILED" << endl;
    QDP_abort(1);
  }
  else { 
    QDPIO::cout << "\t OK" << endl;
  }




  QDPIO::cout << "All tests passed" << endl;
  pop(xml_out);
  xml_out.close();

  Chroma::finalize();
    
  exit(0);
}
