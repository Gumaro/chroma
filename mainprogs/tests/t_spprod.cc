// $Id: t_spprod.cc,v 1.1 2003-05-09 20:07:23 edwards Exp $

#include <iostream>
#include <cstdio>

#include "chroma.h"

using namespace QDP;


int main(int argc, char **argv)
{
  // Put the machine into a known state
  QDP_initialize(&argc, &argv);

  // Setup the layout
  const int foo[] = {2,2,2,2};
  multi1d<int> nrow(Nd);
  nrow = foo;  // Use only Nd elements
  Layout::setLattSize(nrow);
  Layout::create();

  NmlWriter nml("t_spprod.nml");
  Write(nml,Nd);
  Write(nml,Nc);
  Write(nml,Ns);
  Write(nml,nrow);

  LatticeFermion fpsi;
  LatticeFermion fchi;
  LatticeFermion fa1;
  LatticePropagator ppsi;
  LatticePropagator pchi;
  LatticePropagator pa1;

  /* Test 1 */
  printf("Fill fermions with gaussians and spprod\n");
  gaussian(fpsi);
  gaussian(fchi);

  push(nml,"here_is_psi");
  Write(nml, fpsi);
  pop(nml);

  /* fa1 = (gamma(n))*psi */
  for(int n = 0; n < Ns*Ns; ++n)
  {
    fa1 = zero;
    fa1[rb[0]] = Gamma(n) * fpsi;

    printf("print the fa1 fields in direction n= %d\n", n);
    push(nml,"Basis");
    Write(nml, n);
    pop(nml);
    push(nml,"fa1_is_spin");
    Write(nml, fa1);
    pop(nml);
  }

      
  /* Test 2 */
  printf("Fill fermions with gaussians and spprod/trace\n");
  gaussian(fpsi);
  gaussian(fchi);

  /* fa1 = (gamma(n))*psi */
  for(int n = 0; n < Ns*Ns; ++n)
  {
    DComplex dcsum = innerProduct(fchi, Gamma(n) * fpsi);

    printf("print the fa1 fields in direction n= %d\n", n);
    push(nml,"Fermion_inner_product");
    Write(nml, n);
    Write(nml, dcsum);
    pop(nml);
  }

          
  /* Test 3 */
  printf("Fill propagators with gaussians and spprod/trace\n");
  gaussian(ppsi);
  gaussian(pchi);

  /* pa1 = (gamma(n))*psi */
  for(int n = 0; n < Ns*Ns; ++n)
  {
    DComplex dcsum = innerProduct(pchi, Gamma(n) * ppsi);

    printf("print the pa1 fields in direction n= %d\n", n);
    push(nml,"Propagator_inner_product");
    Write(nml, n);
    Write(nml, dcsum);
    pop(nml);
  }

  // Time to bolt
  QDP_finalize();

  return 0;
}
