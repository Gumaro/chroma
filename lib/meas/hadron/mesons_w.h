// -*- C++ -*-
// $Id: mesons_w.h,v 1.7 2003-06-24 03:25:27 edwards Exp $

#ifndef __mesons_h__
#define __mesons_h__

//! Meson 2-pt functions
/* This routine is specific to Wilson fermions!
 *
 * Construct meson propagators
 * The two propagators can be identical or different.
 *
 * \param quark_prop_1 -- first quark propagator ( Read )
 * \param quark_prop_2 -- second (anti-) quark propagator ( Read )
 * \param t0 -- timeslice coordinate of the source ( Read )
 * \param phases -- object holds list of momenta and Fourier phases ( Read )
 * \param nml -- namelist file object ( Read )
 * \param nml_group -- string used for writing nml data ( Read )
 *
 *        ____
 *        \
 * m(t) =  >  < m(t_source, 0) m(t + t_source, x) >
 *        /
 *        ----
 *          x
 */

void mesons(const LatticePropagator& quark_prop_1,
            const LatticePropagator& quark_prop_2,
            const SftMom& phases,
            int t0,
            XMLWriter& xml,
            const string& xml_group) ;

#endif
