//  $Id: mesons_w.cc,v 1.19 2004-07-28 02:38:04 edwards Exp $
//  $Log: mesons_w.cc,v $
//  Revision 1.19  2004-07-28 02:38:04  edwards
//  Changed {START,END}_CODE("foo") to {START,END}_CODE().
//
//  Revision 1.18  2004/02/11 12:51:34  bjoo
//  Stripped out Read() and Write()
//
//  Revision 1.17  2004/02/03 20:47:24  edwards
//  Small code tweaks.
//
//  Revision 1.16  2003/10/01 03:01:39  edwards
//  Removed extraneous include.
//
//  Revision 1.15  2003/09/29 21:31:36  edwards
//  Tiny cosmetic change.
//
//  Revision 1.14  2003/06/24 03:25:27  edwards
//  Changed from nml to xml.
//
//  Revision 1.13  2003/04/02 22:28:22  edwards
//  Changed proto.h to qdp_util.h
//
//  Revision 1.12  2003/04/01 03:27:26  edwards
//  Added const to sftmom.
//
//  Revision 1.11  2003/04/01 02:38:26  edwards
//  Added doxygen comments.
//
//  Revision 1.10  2003/03/14 21:51:54  flemingg
//  Changes the way in which the nml data is output to match what's done
//  in szin.
//
//  Revision 1.9  2003/03/14 17:16:13  flemingg
//  Variant 1 is now working with SftMom::sft().  In arbitrary units,
//  the relative performance seems to be: V1) 7.5, V2) 10, V3) 100.
//
//  Revision 1.8  2003/03/14 05:14:32  flemingg
//  rewrite of mesons_w.cc to use the new SftMom class.  mesons_w.cc still
//  needs to be cleaned up once the best strategy is resolved.  But for now,
//  the library and test program compiles and runs.
//
//  Revision 1.7  2003/03/06 03:38:35  edwards
//  Added start/end_code.
//
//  Revision 1.6  2003/03/06 02:07:12  flemingg
//  Changed the MomList class to eliminate an unneeded class member.
//
//  Revision 1.5  2003/03/06 00:30:14  flemingg
//  Complete rewrite of lib/meas/hadron/mesons_w.cc, including a simple test
//  program in mainprogs/tests built with 'make check' and various other
//  changes to autoconf/make files to support this rewrite.
//

#include "chromabase.h"
#include "util/ft/sftmom.h"
#include "meas/hadron/mesons_w.h"

using namespace QDP;

//! Meson 2-pt functions
/*!
 * \ingroup hadron
 *
 * This routine is specific to Wilson fermions!
 *
 * Construct meson propagators
 * The two propagators can be identical or different.
 *
 * \param quark_prop_1 -- first quark propagator ( Read )
 * \param quark_prop_2 -- second (anti-) quark propagator ( Read )
 * \param t0 -- timeslice coordinate of the source ( Read )
 * \param phases -- object holds list of momenta and Fourier phases ( Read )
 * \param xml -- namelist file object ( Read )
 * \param xml_group -- string used for writing xml data ( Read )
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
	    const string& xml_group)
{
  START_CODE();

  // Length of lattice in decay direction
  int length = phases.numSubsets();

  // Construct the anti-quark propagator from quark_prop_2
  int G5 = Ns*Ns-1;
  LatticePropagator anti_quark_prop =  Gamma(G5) * quark_prop_2 * Gamma(G5);

  // This variant uses the function SftMom::sft() to do all the work
  // computing the Fourier transform of the meson correlation function
  // inside the class SftMom where all the of the Fourier phases and
  // momenta are stored.  It's primary disadvantage is that it
  // requires more memory because it does all of the Fourier transforms
  // at the same time.

  // Loop over gamma matrix insertions
  XMLArrayWriter xml_gamma(xml,Ns*Ns);
  push(xml_gamma, xml_group);

  for (int gamma_value=0; gamma_value < (Ns*Ns); ++gamma_value)
  {
    push(xml_gamma);     // next array element
    write(xml_gamma, "gamma_value", gamma_value);

    // Construct the meson correlation function
    LatticeComplex corr_fn;
    corr_fn = trace(adj(anti_quark_prop) * Gamma(gamma_value) *
                    quark_prop_1 * Gamma(gamma_value));

    multi2d<DComplex> hsum;
    hsum = phases.sft(corr_fn);

    // Loop over sink momenta
    XMLArrayWriter xml_sink_mom(xml_gamma,phases.numMom());
    push(xml_sink_mom, "momenta");

    for (int sink_mom_num=0; sink_mom_num < phases.numMom(); ++sink_mom_num) 
    {
      push(xml_sink_mom);
      write(xml_sink_mom, "sink_mom_num", sink_mom_num);
      write(xml_sink_mom, "sink_mom", phases.numToMom(sink_mom_num));

      multi1d<Real> mesprop(length);
      for (int t=0; t < length; ++t) 
      {
        int t_eff = (t - t0 + length) % length;
	mesprop[t_eff] = real(hsum[sink_mom_num][t]);
      }

      write(xml_sink_mom, "mesprop", mesprop);
      pop(xml_sink_mom);

    } // end for(sink_mom_num)
 
    pop(xml_sink_mom);
    pop(xml_gamma);
  } // end for(gamma_value)

  pop(xml_gamma);

  END_CODE();
}
