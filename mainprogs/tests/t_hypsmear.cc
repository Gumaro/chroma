// $Id: t_hypsmear.cc,v 1.5 2004-02-11 12:51:35 bjoo Exp $

#include <iostream>
#include <cstdio>

#include "chroma.h"

using namespace QDP;

int main(int argc, char *argv[])
{
  // Put the machine into a known state
  QDP_initialize(&argc, &argv);

  // Setup the layout
  const int foo[] = {4,4,4,8};
  multi1d<int> nrow(Nd);
  nrow = foo;  // Use only Nd elements
  Layout::setLattSize(nrow);
  Layout::create();

  NmlWriter nml("t_hypsmear.nml");

  push(nml,"lattis");
  write(nml,"Nd", Nd);
  write(nml,"Nc", Nc);
  write(nml,"nrow", nrow);
  pop(nml);

  //! Example of calling a plaquette routine
  /*! NOTE: the STL is *not* used to hold gauge fields */
  multi1d<LatticeColorMatrix> u(Nd);
  Double w_plaq, s_plaq, t_plaq, link;

//  QDPIO::cout << "Reading test_purgaug.cfg1\n";
//  Seed seed_old;
//  readSzin(u, 0, "../test_purgaug.cfg1", seed_old);
  QDPIO::cout << "Start gaussian\n";
  for(int m=0; m < u.size(); ++m)
    gaussian(u[m]);

  // Reunitarize the gauge field
  for(int m=0; m < u.size(); ++m)
    reunit(u[m]);

  // Try out the plaquette routine
  MesPlq(u, w_plaq, s_plaq, t_plaq, link);
  QDPIO::cout << "w_plaq = " << w_plaq << endl;
  QDPIO::cout << "link = " << link << endl;

  // Write out the results
  push(nml,"observables");
  write(nml,"w_plaq", w_plaq);
  write(nml,"link", link);
  pop(nml);

  // Now hyp smear
  multi1d<LatticeColorMatrix> u_hyp(Nd);
  Real BlkAccu = 1.0e-5;
  int BlkMax = 100;
  Real alpha1 = 0.7;
  Real alpha2 = 0.6;
  Real alpha3 = 0.3;
  Hyp_Smear(u, u_hyp, alpha1, alpha2, alpha3, BlkAccu, BlkMax);

  // Measure again
  MesPlq(u_hyp, w_plaq, s_plaq, t_plaq, link);
  QDPIO::cout << "w_plaq = " << w_plaq << endl;
  QDPIO::cout << "link = " << link << endl;

  // Write out the results
  push(nml,"HYP_observables");
  write(nml,"w_plaq", w_plaq);
  write(nml,"link", link);
  pop(nml);

  // Time to bolt
  QDP_finalize();

  exit(0);
}
