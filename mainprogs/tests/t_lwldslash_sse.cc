// $Id: t_lwldslash_sse.cc,v 1.22 2004-03-25 23:18:10 edwards Exp $

#include <iostream>
#include <cstdio>

#include "chroma.h"

using namespace QDP;


int main(int argc, char **argv)
{
  // Put the machine into a known state
  QDP_initialize(&argc, &argv);

  std::string root="";

  if(argc == 2 ) { 
	root += argv[1];
        root += "/";
  }

  // Read parameters
  XMLReader xml_in((root+"input.xml"));

  // Lattice Size
  multi1d<int> nrow(Nd);
  read(xml_in, "/param/nrow", nrow);

  xml_in.close();

  // Setup the layout
  Layout::setLattSize(nrow);
  Layout::create();

  XMLFileWriter xml(root+"t_lwldslash.xml");

  push(xml,"t_lwldslash");

  proginfo(xml);    // Print out basic program info

  // Make up a random gauge field.
  multi1d<LatticeColorMatrix> u(Nd);
  for(int m=0; m < u.size(); ++m)
    gaussian(u[m]);


  // Make up a gaussian source and a zero result vector
  LatticeFermion psi, chi, chi2;
  gaussian(psi);
  chi = zero;

  //! Create a linear operator
  QDPIO::cout << "Constructing naive QDPWilsonDslash" << endl;

  // Naive Dslash
  QDPWilsonDslash D(u);

  QDPIO::cout << "Done" << endl;

#undef DEBUG

  push(xml,"Unoptimized_test");

  int isign, cb, loop, iter=1;
  bool first = true;
  for(isign = 1; isign >= -1; isign -= 2) {
    for(cb = 0; cb < 2; ++cb) { 

      clock_t myt1;
      clock_t myt2;
      double mydt;
     
#ifndef DEBUG
      if (first) 
      {
	for(iter=1; ; iter <<= 1)
	{
	  QDPIO::cout << "Applying D " << iter << " times" << endl;

	  myt1=clock();
	  for(int i=iter; i-- > 0; ) {
	    D.apply(chi, psi, (isign == 1 ? PLUS : MINUS), cb);
	  }
	  myt2=clock();

	  mydt=double(myt2-myt1)/double(CLOCKS_PER_SEC);
	  if (mydt > 1) {
	    first = false;
	    break;
	  }
	}
      }
#endif
	
      QDPIO::cout << "Applying D for timings" << endl;
      
      myt1=clock();
      for(int i=iter; i-- > 0; ) {
	D.apply(chi, psi, (isign == 1 ? PLUS : MINUS), cb);
      }
      myt2=clock();
      
      mydt=double(myt2-myt1)/double(CLOCKS_PER_SEC);
      mydt=1.0e6*mydt/double(iter*(Layout::sitesOnNode()/2));
      
      float mflops = float(1320.0f/mydt);
      QDPIO::cout << "cb = " << cb << " isign = " << isign << endl;
      QDPIO::cout << "The time per lattice point is "<< mydt 
		  << " micro sec (" <<  mflops << ") Mflops " << endl;

      push(xml,"test");
      write(xml,"cb",cb);
      write(xml,"isign",isign);
      write(xml,"mflops",mflops);
      pop(xml);
    }
  }
  
  pop(xml);

  //! Create a linear operator
  QDPIO::cout << "Constructing (possibly optimized) WilsonDslash" << endl;

  WilsonDslash D_opt(u);

  QDPIO::cout << "Done" << endl;

  push(xml,"Optimized_test");

  first = true;
  for(isign = 1; isign >= -1; isign -= 2) {
    for(cb = 0; cb < 2; ++cb) { 

      clock_t myt1;
      clock_t myt2;
      double mydt;
      
#ifndef DEBUG
      if (first) 
      {
	for(iter=1; ; iter <<= 1)
	{
	  QDPIO::cout << "Applying D " << iter << " times" << endl;

	  myt1=clock();
	  for(int i=iter; i-- > 0; ) {
	    D_opt.apply(chi, psi, (isign == 1 ? PLUS : MINUS ) , cb); // NOTE: for timings throw away return value
	  }
	  myt2=clock();

	  mydt=double(myt2-myt1)/double(CLOCKS_PER_SEC);
	  if (mydt > 1) {
	    first = false;
	    break;
	  }
	}
      }
#endif

      QDPIO::cout << "Applying D for timings" << endl;
      
      myt1=clock();
      for(int i=iter; i-- > 0; ) {
	D_opt.apply(chi, psi, (isign == 1 ? PLUS : MINUS ) , cb); // NOTE: for timings throw away return value
      }
      myt2=clock();
      
      mydt=double(myt2-myt1)/double(CLOCKS_PER_SEC);
      mydt=1.0e6*mydt/double(iter*(Layout::sitesOnNode()/2));
      
      float mflops = float(1320.0f/mydt);
      QDPIO::cout << "cb = " << cb << " isign = " << isign << endl;
      QDPIO::cout << "After " << iter << " calls, the time per lattice point is "<< mydt 
		  << " micro sec (" <<  mflops << ") Mflops " << endl;

      push(xml,"test");
      write(xml,"cb",cb);
      write(xml,"isign",isign);
      write(xml,"mflops",mflops);
      pop(xml);
    }
  }

  pop(xml);

  LatticeFermion chi3;
  Double n2;

  gaussian(chi3);
  gaussian(psi);
  for(cb = 0; cb < 2; cb++) { 
    for(isign = 1; isign >= -1; isign -= 2) { 

      chi = chi3;
      chi2 = chi3;
      D.apply(chi, psi, (isign > 0 ? PLUS : MINUS), cb);
      D.apply(chi2, psi, (isign > 0 ? PLUS : MINUS), cb);
      
      n2 = norm2( chi2 - chi );

      QDPIO::cout << "Paranoia test: || D(psi, "
		  << (isign > 0 ? "+, " : "-, ") <<  cb 
		  << ") - D(psi, " 
		  << (isign > 0 ? "+, " : "-, ") <<  cb << " ) ||  = " << n2 
		  << endl;
    }
  }

  gaussian(chi3);
  gaussian(psi);
  for(cb = 0; cb < 2; cb++) { 
    for(isign = 1; isign >= -1; isign -= 2) { 

      chi = chi3;
      chi2 = chi3;
      D.apply(chi, psi, (isign > 0 ? PLUS : MINUS), cb);
      D_opt.apply(chi2, psi, (isign > 0 ? PLUS : MINUS), cb);
      
      n2 = norm2( chi2 - chi );

#if 0
      QDPIO::cout << "|D(psi, "
		  << (isign > 0 ? "+, " : "-, ") <<  cb << ") = " << norm2(chi) << endl
		  << "D_opt(psi, " 
		  << (isign > 0 ? "+, " : "-, ") <<  cb << ") = " << norm2(chi2) << endl; 
#endif

      QDPIO::cout << "OPT test: || D(psi, "
		  << (isign > 0 ? "+, " : "-, ") <<  cb 
		  << ") - D_opt(psi, " 
		  << (isign > 0 ? "+, " : "-, ") <<  cb << " ) ||  = " << n2 
		  << endl;

      push(xml,"OPT test");
      write(xml,"isign", isign);
      write(xml,"co", cb);
      write(xml,"norm2_diff",n2);
      pop(xml);
    }
  }
	  
  pop(xml);
  
  // Time to bolt
  QDP_finalize();

  exit(0);
}
