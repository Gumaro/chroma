// $Id: gauge_io.cc,v 1.2 2004-05-27 00:47:18 edwards Exp $
/*! \file
 * \brief Routines associated with Chroma propagator gauge IO
 */

#include "chromabase.h"
#include "io/gauge_io.h"

using std::string;

// Read a Chroma propagator
/*
 * \param file_xml     xml reader holding config info ( Modify )
 * \param record_xml   xml reader holding config info ( Modify )
 * \param u            gauge configuration ( Modify )
 * \param file         path ( Read )
 * \param serpar       either QDPIO_SERIAL, QDPIO_PARALLEL ( Read )
 */    
void readGauge(XMLReader& file_xml,
	       XMLReader& record_xml, 
	       multi1d<LatticeColorMatrix>& u, 
	       const string& file, 
	       QDP_serialparallel_t serpar)
{
  QDPFileReader to(file_xml,file,serpar);

  /* 
   * This is problematic - the size should
   * come from the read - a resize. Currently, QDPIO does not 
   * support this
   */
  multi1d<LatticeColorMatrixF> u_f(u.size());
  read(to,record_xml,u_f);      // Always read in single precision!

  for(int mu=0; mu < u.size(); ++mu)
    u[mu] = u_f[mu];

  close(to);
}


// Write a Gauge field in QIO format
/*
 * \param file_xml    xml reader holding config info ( Modify )
 * \param record_xml  xml reader holding config info ( Modify )
 * \param u           gauge configuration ( Modify )
 * \param file        path ( Read )
 * \param volfmt      either QDPIO_SINGLEFILE, QDPIO_MULTIFILE ( Read )
 * \param serpar      either QDPIO_SERIAL, QDPIO_PARALLEL ( Read )
 */    
void writeGauge(XMLBufferWriter& file_xml,
		XMLBufferWriter& record_xml, 
		const multi1d<LatticeColorMatrix>& u, 
		const string& file, 
		QDP_volfmt_t volfmt, 
		QDP_serialparallel_t serpar)
{
  QDPFileWriter to(file_xml,file,volfmt,serpar,QDPIO_OPEN);

  multi1d<LatticeColorMatrixF> u_f(u.size());
  for(int mu=0; mu < u.size(); ++mu)
    u_f[mu] = u[mu];

  write(to,record_xml,u_f);         // Always save in single precision!
  close(to);
}




