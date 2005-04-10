// $Id: propagator.cc,v 1.96 2005-04-10 15:04:36 edwards Exp $
/*! \file
 *  \brief Main code for propagator generation
 */

#include <iostream>
#include <cstdio>

#include "chroma.h"

using namespace Chroma;


//! To insure linking of code, place the registered code flags here
/*! This is the bit of code that dictates what fermacts are in use */

bool linkage_hack()
{
  bool foo = true;

  // All actions
  foo &= WilsonTypeFermActsEnv::registered;

  return foo;
}


//! Propagator generation
/*! \defgroup propagator Propagator generation
 *  \ingroup main
 *
 * Main program for propagator generation. 
 */

int main(int argc, char **argv)
{
  // Put the machine into a known state
  Chroma::initialize(&argc, &argv);

  QDPIO::cout << "linkage=" << linkage_hack() << endl;

  START_CODE();

  // Instantiate xml reader for DATA
  XMLReader xml_in(Chroma::getXMLInputFileName());

  // Read data
  InlinePropagatorParams input(xml_in, "/propagator");
  InlinePropagator  meas(input);

  // Specify lattice size, shape, etc.
  Layout::setLattSize(input.param.nrow);
  Layout::create();

  QDPIO::cout << "PROPAGATOR" << endl;

  // Read gauge field info
  Cfg_t  cfg;
  try
  {
    read(xml_in, "/propagator/Cfg", cfg);
  }
  catch(const string& e)
  {
    QDP_error_exit("Error reading in propagator: %s", e.c_str());
  }

  // Start up the gauge field
  multi1d<LatticeColorMatrix> u(Nd);
  XMLBufferWriter config_xml;
  {
    XMLReader gauge_file_xml, gauge_xml;

    QDPIO::cout << "Initialize Gauge field" << endl;
    gaugeStartup(gauge_file_xml, gauge_xml, u, cfg);
    QDPIO::cout << "Gauge field initialized!" << endl;

    config_xml << gauge_xml;
  }

  // Check if the gauge field configuration is unitarized
  unitarityCheck(u);

  // Output
  XMLFileWriter& xml_out = Chroma::getXMLOutputInstance();

  unsigned long cur_update = 0;
  meas(u, config_xml, cur_update, xml_out);

  xml_out.flush();

  END_CODE();

  // Time to bolt
  Chroma::finalize();

  exit(0);
}
