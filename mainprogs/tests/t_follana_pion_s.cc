// $Id: t_follana_pion_s.cc,v 1.1 2003-10-23 15:28:17 bjoo Exp $

#include <iostream>
#include <cstdio>

#include "chroma.h"
#include "io/follana_io_s.h"
#include "meas/hadron/pions_follana_s.h"

using namespace QDP;



int main(int argc, char *argv[])
{
  // Put the machine into a known state
  QDP_initialize(&argc, &argv);

  XMLReader xml_in("input.xml");

  // XML File will look like this
  // 
  // <? xml version="1.0" ?>
  // <test>
  //    <nrow>X Y Z T</nrow>
  //    <propStem>stem_for_prop_filenames</propStem>
  // </test>
  //
  multi1d<int> nrow(Nd);
  string filename_stem;
  string filename;


  read(xml_in, "/test/nrow", nrow);
  read(xml_in, "/test/propStem", filename_stem);



  Layout::setLattSize(nrow);
  Layout::create();

  QDPIO::cout << "Lattice: Lx = " << nrow[0] << " Ly = " << nrow[1] << " Lz = " << nrow[2]
	      << " Lt =" << nrow[3] << endl;


  QDPIO::cout << "About to read propagators" << endl;

  int i;
  multi1d<LatticePropagator> props(NUM_STAG_PROPS);

  for(i = 0;  i < NUM_STAG_PROPS; i++) { 
    ostringstream istring;
    istring << "." << i;

    filename = filename_stem + istring.str();;
    QDPIO::cout << "Reading Propagator from file " << filename.c_str() << endl;

    // Try and read the propagator;

    props[i] = zero;
    readQpropFollana((char *)filename.c_str(), props[i], true);
  }

  QDPIO::cout << "Computing the meaning of life..." << endl;
  
  multi2d<DComplex> pions(NUM_STAG_PIONS, nrow[3]);
  staggeredPionsFollana(props, pions, Nd-1);

  XMLFileWriter xml_out("output.xml");

  push(xml_out, "follanaIO");
  for(i=0; i < NUM_STAG_PIONS; i++) { 
    ostringstream tag;
    tag << "pion" << i;
    push(xml_out, tag.str());
    Write(xml_out, pions[i]);
    pop(xml_out);
  }

  pop(xml_out);

  QDPIO::cout << "That's all folks" << endl;
  // Time to bolt
  QDP_finalize();
  exit(0);
}
