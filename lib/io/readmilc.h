// $Id: readmilc.h,v 3.2 2009-05-13 13:12:19 edwards Exp $

/*! \file
 *  \brief Read a MILC gauge configuration written in the 1997 format
 */

#ifndef __readmilc_h__
#define __readmilc_h__

#include "io/milc_io.h"

namespace Chroma {

//! Read a MILC gauge configuration written in the 1997 format
/*!
 * \ingroup io
 *
 * \param xml        xml reader holding config info ( Modify )
 * \param u          gauge configuration ( Modify )
 * \param cfg_file   path ( Read )
 */    

void readMILC(XMLReader& xml, multi1d<LatticeColorMatrixF>& u, const string& cfg_file);

//! Read a MILC gauge configuration written in the 1997 format
/*!
 * \ingroup io
 *
 * \param header     structure holding config info ( Modify )
 * \param u          gauge configuration ( Modify )
 * \param cfg_file   path ( Read )
 */    

void readMILC(MILCGauge_t& header, multi1d<LatticeColorMatrixF>& u, const string& cfg_file);

//! Read a MILC gauge configuration written in the 1997 format
/*!
 * \ingroup io
 *
 * \param header     structure holding config info ( Modify )
 * \param u          gauge configuration ( Modify )
 * \param cfg_file   path ( Read )
 */    

void readMILC(MILCGauge_t& header, multi1d<LatticeColorMatrixD>& u, const string& cfg_file);

}  // end namespace Chroma

#endif
