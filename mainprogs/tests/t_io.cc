// $Id: t_io.cc,v 1.2 2003-01-04 05:09:27 edwards Exp $

#include <iostream>
#include <cstdio>

#include "szin.h"

using namespace QDP;


int main(int argc, char **argv)
{
  // Put the machine into a known state
  QDP_initialize(&argc, &argv);

  // Setup the layout
  const int foo[] = {2,2,2,2};
  multi1d<int> nrow(Nd);
  nrow = foo;  // Use only Nd elements
  Layout::create(nrow);

  LatticeReal a;
  Double d = 17;
  random(a);

  NmlWriter tonml("cat");
  Write(tonml,a);
  tonml.close();

  BinaryWriter tobinary("dog");
  write(tobinary, a);
  write(tobinary, d);
  tobinary.close();

  float x;
  cerr << "Read some data from file input\n";
  TextReader from("input");
  from >> x;
  from.close();

  cerr << "you entered :" << x << ":\n";
  
  // Time to bolt
  QDP_finalize();
}
