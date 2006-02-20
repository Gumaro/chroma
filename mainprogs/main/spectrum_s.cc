// $Id: spectrum_s.cc,v 2.2 2006-02-20 14:45:38 egregory Exp $
/*! \file
 * \brief Main code for spectrum measurements
 */

#include "chroma.h"
#include "meas/gfix/coulgauge.h"

using namespace Chroma;

//! Spectrum measurements
/*! \defgroup spectrumain Spectrum measurements
 *  \ingroup main
 *
 * Main program for spectrum measurements
 */

int main(int argc, char **argv)
{
  // Put the machine into a known state
  Chroma::initialize(&argc, &argv);

  START_CODE();

  QDPIO::cerr << "Staggered measurement code." <<  endl;

  XMLReader xml_in ;
  string in_name = Chroma::getXMLInputFileName() ;
  try
    {
      xml_in.open(in_name);
    }
  catch (...)
    {
      QDPIO::cerr << "Error reading input file " << in_name << endl;
      QDPIO::cerr << "The input file name can be passed via the -i flag " << endl;
      QDPIO::cerr << "The default name is ./DATA" << endl;
      throw;
    }

  // Read data
  InlineSpectrumParams_s input(xml_in, "/spectrum_s");
  InlineSpectrum_s  meas(input);

  // Specify lattice size, shape, etc.
  Layout::setLattSize(input.param.nrow);
  Layout::create();

  // Read gauge field info
  Cfg_t  cfg;
  try
  {
    read(xml_in, "/spectrum_s/Cfg", cfg);
  }
  catch(const string& e)
  {
    QDP_error_exit("Error reading in spectrum_s: %s", e.c_str());
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

  bool do_gauge_transform = false;
  read(xml_in, "/spectrum_s/Param/do_gauge_transform",do_gauge_transform );
  if( do_gauge_transform )
    {
      rgauge(u); 
      QDPIO::cout << "Random gauge transform on gauge fields done." << endl;
    }


  // Output
  XMLFileWriter& xml_out = Chroma::getXMLOutputInstance();


  bool gauge_invar_oper = false;
  read(xml_in, "/spectrum_s/Param/gauge_invar_oper",gauge_invar_oper );

    // GAUGE FIX 
    if( !gauge_invar_oper){
      //not using covariant shifts, should gauge-fix

      //read some input the sloppy way
      Real GFAccu;  // Gauge fixing tolerance 
      Real OrPara;  // Gauge fixing over-relaxation param
      int  GFMax;   // Maximum gauge fixing iterations
      int  j_decay; // the t-direction

      read(xml_in, "/spectrum_s/Param/GFAccu", GFAccu );
      read(xml_in, "/spectrum_s/Param/OrPara", OrPara );
      read(xml_in, "/spectrum_s/Param/GFMax", GFMax );
      read(xml_in, "/spectrum_s/Param/j_decay",j_decay  );
      int  n_gf;


      QDPIO::cout << "Not using covariant shift operators.";
      QDPIO::cout << "Must gauge-fix" <<  endl;

      QDPIO::cout << "Starting to fix to Coulomb gauge" <<  endl;
      coulGauge(u, n_gf, j_decay, GFAccu, GFMax, true,	OrPara);
      QDPIO::cout << "No. of gauge fixing iterations =" << n_gf << endl;
    }



  unsigned long cur_update = 0;
  meas(u, config_xml, cur_update, xml_out);

  xml_out.flush();
  xml_out.close();

  END_CODE();

  // Time to bolt
  Chroma::finalize();

  exit(0);
}
