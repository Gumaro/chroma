// -*- C++ -*-
// $Id: lib.h,v 1.7 2005-01-12 13:57:09 bjoo Exp $

/*! \file
 * \brief Chroma Lattice Field Theory library
 *
 * Central include file for all pieces of the Chroma lattice field theory library
 */

/*! \defgroup lib Chroma Lattice Field Theory library
 *
 * Central include file for all pieces of the Chroma lattice field theory library
 */


#ifndef __lib_h__
#define __lib_h__

#include "init/chroma_init.h"

#include "fermact.h"
#include "fermbc.h"
#include "gaugeact.h"
#include "gaugebc.h"
#include "handle.h"
#include "linearop.h"
#include "state.h"

#include "actions/actions.h"
#include "update/update.h"
#include "util/util.h"
#include "meas/meas.h"

/* This causes bother with installations. Not functional yet so 
   I comment it out */
#include "update/update.h"
#include "io/io.h"

#endif


