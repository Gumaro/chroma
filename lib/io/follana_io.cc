/*
 *  $Id: follana_io.cc,v 1.1 2003-09-11 11:12:21 bjoo Exp $
 *
 *  These are a few simple I/O routines that we can use until QIO makes its appearance
 *  I have tried to include a simple header by means of a structure.
 */

#include "chromabase.h"
#include "common_io.h"
#include "io/follana_io.h"
#include "primitives.h"
#include "qdp_util.h"


#include <string>

using std::string;

void readQpropFollana(char file[], LatticePropagator& quark_prop){

  /*
   *  Now the local variables
   */
  multi1d<Real32> buf( Layout::vol()*Nc*Nc*2 );
  BinaryReader prop_in(file);
 
  read(prop_in,buf);

  int x, y, z, t, src_col, snk_col, src_spin, snk_spin;
  Propagator site_prop;
 
  Complex tmp_cmpx;
  const multi1d<int>& latt_size = Layout::lattSize();
  multi1d<int> coords(Nd);

  /* Get size of the lattice */
  /* Read the file somehow */

  int index=0;
  for( t = 0; t < latt_size[3]; t++) {  
    for( z = 0; z < latt_size[2]; z++) { 
      for( y = 0; y < latt_size[1]; y++ ) {
	for( x = 0; x < latt_size[0]; x++) {

	  coords[0] = x;
	  coords[1] = y;
	  coords[2] = z;
	  coords[3] = t;
	
	  for( snk_spin = 0; snk_spin < Ns; snk_spin++) { 
	    for( src_spin = 0; src_spin < Ns; src_spin++) { 

	      ColorMatrix tmp_col;

	      for( snk_col = 0; snk_col < Nc; snk_col++) { 
		for( src_col = 0; src_col < Nc; src_col++) { 

		  Real32 re, im;
		  
		  re = buf[index];
		  index++;
		  im = buf[index];
		  index++;

		  tmp_cmpx = cmplx(Real(re), Real(im));
		  
		  tmp_col = pokeColor(tmp_col,
			    tmp_cmpx,
			    snk_col,
			    src_col);
		}
	      }
		
	      site_prop = pokeSpin(site_prop,
		       tmp_col,
		       snk_spin,
		       src_spin);
	    }
	  }

	  pokeSite(quark_prop,
		   site_prop,
		   coords);

	}
      }
    }
  }
}

