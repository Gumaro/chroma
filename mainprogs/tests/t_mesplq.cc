// $Id: t_mesplq.cc,v 1.9 2003-10-08 18:54:00 edwards Exp $

#include <iostream>
#include <cstdio>

#include "chroma.h"

using namespace QDP;

int main(int argc, char *argv[])
{
  // Put the machine into a known state
  QDP_initialize(&argc, &argv);

  // Setup the layout
  const int foo[] = {4,4,4,4};
  multi1d<int> nrow(Nd);
  nrow = foo;  // Use only Nd elements
  Layout::setLattSize(nrow);
  Layout::create();

  XMLFileWriter xml("t_mesplq.xml");
  push(xml, "t_mesplq");

  push(xml,"lattis");
  Write(xml,Nd);
  Write(xml,Nc);
  Write(xml,nrow);
  pop(xml);

  //! Example of calling a plaquette routine
  /*! NOTE: the STL is *not* used to hold gauge fields */
  multi1d<LatticeColorMatrix> u(Nd);
  Double w_plaq, s_plaq, t_plaq, link;

  cerr << "Start gaussian\n";
  for(int m=0; m < u.size(); ++m)
    gaussian(u[m]);

  // Reunitarize the gauge field
  for(int m=0; m < u.size(); ++m)
    reunit(u[m]);

  // Try out the plaquette routine
  MesPlq(u, w_plaq, s_plaq, t_plaq, link);
  cout << "w_plaq = " << w_plaq << endl;
  cout << "link = " << link << endl;

  // Test polyakov routine
  multi1d<DComplex> pollp(Nd);
  for(int mu = 0; mu < Nd; ++mu)
    polylp(u, pollp[mu], mu);

  // Write out the results
  push(xml,"Observables");
  Write(xml,w_plaq);
  Write(xml,link);
  Write(xml,pollp);
  pop(xml);


  // Test gauge invariance
  rgauge(u);

  MesPlq(u, w_plaq, s_plaq, t_plaq, link);
  for(int mu = 0; mu < Nd; ++mu)
    polylp(u, pollp[mu], mu);

  cout << "w_plaq = " << w_plaq << endl;
  cout << "link = " << link << endl;

  push(xml,"Observables_after_gt");
  Write(xml,w_plaq);
  Write(xml,link);
  Write(xml,pollp);
  pop(xml);

  pop(xml);

  // Time to bolt
  QDP_finalize();

  exit(0);
}
