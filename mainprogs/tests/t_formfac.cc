// $Id: t_formfac.cc,v 2.0 2005-09-25 21:04:47 edwards Exp $
//
//! \file
//  \brief Test the form-factor routine
//
// $Log: t_formfac.cc,v $
// Revision 2.0  2005-09-25 21:04:47  edwards
// Moved to version 2.0
//
// Revision 1.9  2005/03/02 00:44:19  edwards
// Changed to new Chroma initialize/finalize format. Changed
// all XMLReader("DATA") to use a command-line param arg.
// Changed all XMLFileWriter(XMLDAT) to use the singleton instance.
//
// Revision 1.8  2005/01/14 20:13:09  edwards
// Removed all using namespace QDP/Chroma from lib files. The library
// should now be 100% in the Chroma namespace. All mainprogs need a
// using namespace Chroma.
//
// Revision 1.7  2004/02/11 12:51:35  bjoo
// Stripped out Read() and Write()
//
// Revision 1.6  2003/10/09 20:36:49  edwards
// Changed all cout/cerr to QDPIO::cout/cerr. Changed QDP_info calls
// to QDPIO::cout.
//
// Revision 1.5  2003/09/11 00:46:04  edwards
// Changed all programs to exit(0) instead of return 0
//
// Revision 1.4  2003/08/27 22:08:41  edwards
// Start major push to using xml.
//
// Revision 1.3  2003/03/20 19:34:25  flemingg
// Evolved formfac_w.cc to use SftMom class, which included some bug fixes
// in features in SftMom which had been previously untested and evolution
// of the corresponding test program.
//
// Revision 1.2  2003/03/06 00:27:29  flemingg
// Added $Log: t_formfac.cc,v $
// Added Revision 2.0  2005-09-25 21:04:47  edwards
// Added Moved to version 2.0
// Added
// Added Revision 1.9  2005/03/02 00:44:19  edwards
// Added Changed to new Chroma initialize/finalize format. Changed
// Added all XMLReader("DATA") to use a command-line param arg.
// Added Changed all XMLFileWriter(XMLDAT) to use the singleton instance.
// Added
// Added Revision 1.8  2005/01/14 20:13:09  edwards
// Added Removed all using namespace QDP/Chroma from lib files. The library
// Added should now be 100% in the Chroma namespace. All mainprogs need a
// Added using namespace Chroma.
// Added
// Added Revision 1.7  2004/02/11 12:51:35  bjoo
// Added Stripped out Read() and Write()
// Added
// Added Revision 1.6  2003/10/09 20:36:49  edwards
// Added Changed all cout/cerr to QDPIO::cout/cerr. Changed QDP_info calls
// Added to QDPIO::cout.
// Added
// Added Revision 1.5  2003/09/11 00:46:04  edwards
// Added Changed all programs to exit(0) instead of return 0
// Added
// Added Revision 1.4  2003/08/27 22:08:41  edwards
// Added Start major push to using xml.
// Added
// Added Revision 1.3  2003/03/20 19:34:25  flemingg
// Added Evolved formfac_w.cc to use SftMom class, which included some bug fixes
// Added in features in SftMom which had been previously untested and evolution
// Added of the corresponding test program.
// Added to header comments
//

#include <iostream>
#include <cstdio>

#include "chroma.h"

using namespace Chroma;

int main(int argc, char *argv[])
{
  // Put the machine into a known state
  Chroma::initialize(&argc, &argv);

  // Setup the layout
  const int foo[] = {4,4,4,4};
  multi1d<int> nrow(Nd);
  nrow = foo;  // Use only Nd elements
  Layout::setLattSize(nrow);
  Layout::create();

  XMLFileWriter xml("t_formfac.xml");

  push(xml,"lattice");
  write(xml,"Nd", Nd);
  write(xml,"Nc", Nc);
  write(xml,"Ns", Ns);
  write(xml,"nrow", nrow);
  pop(xml);

  // Randomize the gauge field
  multi1d<LatticeColorMatrix> u(Nd);

  for(int m=0; m < u.size(); ++m)
    gaussian(u[m]);

  // Reunitarize the gauge field
  for(int m=0; m < u.size(); ++m)
    reunit(u[m]);

  LatticePropagator quark_prop_1, quark_prop_2;
  gaussian(quark_prop_1);
  gaussian(quark_prop_2);

  // propagation direction
  int j_decay = Nd-1;

  // source timeslice
  int t0 = 0 ;

  // sink momentum {1,0,0}
  multi1d<int> sink_mom(Nd-1) ;
  sink_mom    = 0 ;
  sink_mom[0] = 1 ;

  clock_t start_clock = clock() ;

  // create Fourier phases with (mom-sink_mom)^2 <= 10 (NO AVERAGING)
  SftMom phases(10, sink_mom, false, j_decay) ;

  FormFac(u, quark_prop_1, quark_prop_2, phases, t0, xml) ;

  clock_t end_clock = clock() ;

  QDPIO::cout << "Test took " << end_clock - start_clock << " clocks.\n" ;

  // Time to bolt
  Chroma::finalize();

  exit(0);
}

