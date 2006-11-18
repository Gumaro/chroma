/*
 *  \brief Group Theory based Baryon Operator Construction with All2All
 */
#include "chroma.h"
#include "chromabase.h"
#include "meas/hadron/group_baryon_operator_w.h"
#include "meas/sources/source_smearing_aggregate.h"
#include "meas/sources/source_smearing_factory.h"
#include "meas/sinks/sink_smearing_aggregate.h"
#include "meas/sinks/sink_smearing_factory.h"
using namespace Chroma;
using namespace GroupBaryonOperatorEnv;

  void write( XMLWriter& xml, const string& path, 
	            const GroupBaryonOperatorEnv::BaryonOperator_t::BaryonOperatorInsertion_t::BaryonOperatorIndex_t::BaryonOperatorElement_t& input )
  {
    push( xml, path );
    write( xml, "BaryonOpElement", input );
    pop( xml );
  }
  void write( XMLWriter& xml, const string& path, 
	            const GroupBaryonOperatorEnv::BaryonOperator_t::BaryonOperatorInsertion_t::BaryonOperatorIndex_t& input )
  {
    push( xml, path );
    write( xml, "BaryonOpIndex", input );
    pop( xml );
  }
  void write( XMLWriter& xml, const string& path, 
	            const GroupBaryonOperatorEnv::BaryonOperator_t::BaryonOperatorInsertion_t& input )
  {
    push( xml, path );
    write( xml, "BaryonOpIns", input );
    pop( xml );
  }
  void write( XMLWriter& xml, const string& path, 
	            const GroupBaryonOperatorEnv::Params::NamedObject_t& input )
  {
    push( xml, path );
    write( xml, "gauge_id", input.gauge_id );
    //write( xml, "BaryonOp", input.Bops );
    pop( xml );
  }
  //! BaryonOperator header writer
  void write( XMLWriter& xml, const string& path, const BaryonOperator_t& param )
  {
    if( path != "." ) push( xml, path );
    int version = 1;
    write( xml, "version", version );
    write( xml, "mom2_max", param.mom2_max );
    write( xml, "j_decay", param.j_decay );
    write( xml, "seed_l", param.seed_l );
    write( xml, "seed_m", param.seed_m );
    write( xml, "seed_r", param.seed_r );
    write( xml, "perms", param.quarkOrderings );
    if( path != "." ) pop( xml );
  }

bool linkageHack(void)
{
  bool foo = true;
  // Inline Measurements
  foo &= InlineAggregateEnv::registerAll();
  return foo;
}

//
//! Make a group theory based baryon operator with all-to-all quark 
//! propagators using the dilution method of TrinLat (2004)
//
/*! \defgroup GroupBaryonOperator
 *  \ingroup main
 *
 * Main program for making a group baryon operator
 */
int main( int argc, char *argv[] )
{
	// ==================================
  // Put the machine into a known state
	// ==================================
  Chroma::initialize( &argc, &argv );
  START_CODE();
  QDPIO::cout << "GroupBaryonAll2All: Group Baryon Operators with All2All" << endl;
  QDPIO::cout << "Linkage = " << linkageHack() << endl;
	StopWatch swatch;
	// ===============
  // Read input data
	// ===============
  // Instantiate xml reader
  XMLReader xml_in( Chroma::getXMLInputFileName() );
	const std::string path = "/GroupBaryonAll2All";
  Params params( xml_in, path );
	// ==================
  // Setup lattice size
	// ==================
  Layout::setLattSize( params.nrow );
  Layout::create();
	// ============
  // XML business
	// ============
	XMLFileWriter& xml_out = Chroma::getXMLOutputInstance(); // XMLDAT by default
  push( xml_out, "GroupBaryonAll2AllRun" );
  	proginfo( xml_out ); // Print out basic program info
  	write( xml_out, "InputCopy", xml_in ); // save a copy of the input
  	xml_out.flush();
	// ============
  // Startup gauge
	// =============
  XMLReader gauge_file_xml, gauge_xml;
  gaugeStartup(gauge_file_xml, gauge_xml, params.gaugestuff.u, params.gaugestuff.cfg);
		// test the config
		Double w_plaq, s_plaq, t_plaq, link;
  	MesPlq(params.gaugestuff.u, w_plaq, s_plaq, t_plaq, link);
		QDPIO::cout<< "Gauge field measurements " << w_plaq <<" "<< s_plaq <<" "<< t_plaq <<" "<< link <<" "<<endl;	
#if 1
	// Smear the gauge field if needed
	swatch.start();
  std::istringstream xml_l( params.gaugestuff.link_smearing.xml );
  XMLReader linktop( xml_l );
  const string link_path = "/LinkSmearing";
  QDPIO::cout << "Link smearing type = " << params.gaugestuff.link_smearing.id << endl;
	
  Handle< LinkSmearing >
  linkSmearing( TheLinkSmearingFactory::Instance().createObject( 
	              params.gaugestuff.link_smearing.id,
                linktop,
                link_path ) );
  ( *linkSmearing ) ( params.gaugestuff.u );
	swatch.stop();
	QDPIO::cout << "Gauge links smeared : time = " << swatch.getTimeInSeconds() << " secs" << endl;
  	MesPlq(params.gaugestuff.u, w_plaq, s_plaq, t_plaq, link);
		QDPIO::cout<< "gauge meas " << w_plaq <<" "<< s_plaq <<" "<< t_plaq <<" "<< link <<" "<<endl;	
#endif
	// ====================
	// qqq's and baryonop's
	// ====================
	multi1d<GroupBaryonQQQ> AQQQ; // annihilation
	multi1d<GroupBaryonQQQ> CQQQ; // creation
	multi1d<GroupBaryonOp>  AOp;
	multi1d<GroupBaryonOp>  COp;
	// ====================================
	// Read in the text file produced by  
	//   Gen_Input_For_All2All_Baryons2.pl
	// called "Chroma_Input.txt"
	// ====================================
	ReadTextInput( params, AOp, COp, AQQQ, CQQQ );
	// ===============
	// Noise Solutions
	// ===============
  multi1d<Params::QuarkSourceSolutions_t>  quarks( params.qprop.solns.size() );
#if 1
  try
  {
		swatch.start();
    for(int n=0; n < quarks.size(); ++n)
    {
			QDPIO::cout << "Attempt to read solutions for source number=" << n << endl;
			quarks[n].dilutions.resize( params.qprop.solns[n].soln_file_names.size() );
			QDPIO::cout << "dilutions.size= " << quarks[n].dilutions.size() << endl;
			for(int i=0; i < quarks[n].dilutions.size(); ++i)
			{
				XMLReader file_xml, record_xml;
				QDPFileReader from(file_xml, params.qprop.solns[n].soln_file_names[i], QDPIO_SERIAL);
					read(from, record_xml, quarks[n].dilutions[i].soln);
				close(from);
				// need the seed
				read(record_xml, "/Propagator/PropSource/Source/ran_seed", quarks[n].seed);
				params.dilution[ n ].ran_seed = quarks[ n ].seed;
				// read headers ... may not be necessary
	  		read(record_xml, "/Propagator/PropSource",  quarks[n].dilutions[i].source_header);
	  		read(record_xml, "/Propagator/ForwardProp", quarks[n].dilutions[i].prop_header);
			}
    }
		swatch.stop();
  }
  catch (const string& e) 
  {
    QDPIO::cerr << "Error extracting headers: " << e << endl;
    QDP_abort(1);
  }
	QDPIO::cout << "Read SOLUTION vectors from files : time = " << swatch.getTimeInSeconds() << " secs" << endl;
#endif
	// =============
	// Noise Sources ... re-generate from the rng seeds
	// =============
  QDPIO::cout << "Re-generating the noise SOURCES" << endl;
	try
	{
		swatch.start();
	  int N;
		for(int n=0; n < quarks.size(); ++n)
	  {
	    bool first = true;
	    LatticeFermion quark_noise;      // noisy source on entire lattice
	    for(int i=0; i < quarks[ n ].dilutions.size(); ++i)
	    {
	      std::istringstream xml_s( quarks[ n ].dilutions[ i ].source_header.source.xml );
	      XMLReader sourcetop( xml_s );
	      //QDPIO::cout << "Source = " << quarks[n].dilutions[i].source_header.source.id << endl;
	      if ( quarks[ n ].dilutions[ i ].source_header.source.id != DiluteZNQuarkSourceConstEnv::name )
	      {
	        QDPIO::cerr << "Expected source_type = " << DiluteZNQuarkSourceConstEnv::name << endl;
	        QDP_abort( 1 );
	      }
	      //QDPIO::cout << "Quark num= " << n << "  dilution num= " << i << endl;
	      DiluteZNQuarkSourceConstEnv::Params srcParams( sourcetop,
	                                                     quarks[ n ].dilutions[ i ].source_header.source.path );
	      DiluteZNQuarkSourceConstEnv::SourceConst<LatticeFermion> srcConst( srcParams );
	      if ( first )
	      {
	        first = false;
	        quarks[ 0 ].j_decay = srcParams.j_decay;
	        // Grab N
	        N = srcParams.N;
	        // Set the seed to desired value
	        quarks[ n ].seed = srcParams.ran_seed;
	        QDP::RNG::setrn( quarks[ n ].seed );
	        // Create the noisy quark source on the entire lattice
	        zN_src( quark_noise, N );
	      }
	      // The seeds must always agree - here the seed is the unique id of the source
	      if ( toBool( srcParams.ran_seed != quarks[ n ].seed ) )
	      {
	        QDPIO::cerr << "quark source=" << n << "  dilution=" << i << " seed does not match" << endl;
	        QDP_abort( 1 );
	      }
	      // The N's must always agree
	      if ( toBool( srcParams.N != N ) )
	      {
	        QDPIO::cerr << "quark source=" << n << "  dilution=" << i << " N does not match" << endl;
	        QDP_abort( 1 );
	      }
	      // Use a trick here, create the source and subtract it from the global noisy
	      // Check at the end that the global noisy is zero everywhere.
	      // NOTE: the seed will be set every call
	      quarks[ n ].dilutions[ i ].source = srcConst( params.gaugestuff.u );
	      quark_noise -= quarks[ n ].dilutions[ i ].source;
	    } // end for i
	    Double dcnt = norm2( quark_noise );
	    if ( toDouble( dcnt ) != 0.0 )   // problematic - seems to work with unnormalized sources
	    {
	      QDPIO::cerr << "Noise not saturated by all potential solutions: dcnt=" << dcnt << endl;
	      QDP_abort( 1 );
	    }
	  } // end for n
		swatch.stop();
	} // end try
	catch ( const std::string& e )
	{
	  QDPIO::cerr << ": Caught Exception creating source: " << e << endl;
	  QDP_abort( 1 );
	}
	QDPIO::cout << "SOURCE vectors reconstructed from Seeds : time = " << swatch.getTimeInSeconds() << " secs" << endl;
	//
	// setting the seeds for output (baryon_operator file)
	// should also put the mass of the quarks in here as well
	//
	for(int n=0; n < quarks.size(); ++n)
	{
		COp[ n ].baryonoperator.seed_l = quarks[0].seed;
		COp[ n ].baryonoperator.seed_m = quarks[1].seed;
		COp[ n ].baryonoperator.seed_r = quarks[2].seed;
		AOp[ n ].baryonoperator.seed_l = quarks[0].seed;
		AOp[ n ].baryonoperator.seed_m = quarks[1].seed;
		AOp[ n ].baryonoperator.seed_r = quarks[2].seed;
	}
	// ====================================
	//
	//       Main Part of Program
	//
	// Run through the QQQ combinations and
	// multiply by appropriate coefficients
	// to make the various operators
	//
	// ====================================
	multi1d<LatticeComplex> result(1);
	// don't average over equivalent momenta
  SftMom phases(params.mom2_max, false, params.j_decay);
	multi2d<DComplex> elem( params.Nmomenta, params.nrow[3] );
#if 1
	try
	{ //
		// Annihilation Operator (plus) first (only one term)
		//
  	QDPIO::cout << "Making the SINK operators" << endl;
		swatch.start();
		int ordering = 0;
		for(int qqq=0; qqq < params.NQQQs; ++qqq)
		{
			for(int i=0; i < params.NH[ordering][0]; ++i)
			for(int j=0; j < params.NH[ordering][1]; ++j)
			for(int k=0; k < params.NH[ordering][2]; ++k)
			{ 
				result = AQQQ[ qqq ]( 
	    	                      quarks[ 0 ].dilutions[ i ].soln,
	    	                      quarks[ 1 ].dilutions[ j ].soln,
	    	                      quarks[ 2 ].dilutions[ k ].soln,
	    	                      PLUS 
														);
		    for(int b=0; b < AQQQ[ qqq ].NBaryonOps; ++b)
				{
					elem = phases.sft( result[ 0 ] );
		    	for(int p=0; p < params.Nmomenta; ++p)
					{
		    		for(int t=0; t < params.nrow[ 3 ]; ++t)
						{ 
							AQQQ[ qqq ].baryon[ b ]->baryonoperator.orderings[ 0 ].op( i, j, k ).ind[ 0 ].elem( p, t ) 
							+= ( AQQQ[ qqq ].coef[ b ] * elem( p, t ) );
						} // t 
					} // p
				} // Bop_index b
			} // i,j,k
		} // qqq
		swatch.stop();
	} // try
	catch ( const std::string& e )
	{
	  QDPIO::cerr << ": Caught Exception creating sink baryon operator: " << e << endl;
	  QDP_abort( 1 );
	}
	QDPIO::cout << "SINK operators done: time= " << swatch.getTimeInSeconds() << " secs" << endl;
#endif // end annihilation operators

#if 1
	try
	{ //
		// Creation Operator (MINUS) 
		//
		for(int ordering=0; ordering < params.NsrcOrderings; ++ordering)
		{
		  QDPIO::cout << "SOURCE Operator: ordering = " << ordering << endl;
			swatch.start();
			Real signs;
			const int L=0,M=1,R=2;
			//
			// t = quarks[n].dilutions[i].source_header.t_source = timeslice of source
			//
			int qL = DilSwap( ordering, 0,1,2, L );
			int qM = DilSwap( ordering, 0,1,2, M );
			int qR = DilSwap( ordering, 0,1,2, R );
			switch ( ordering )																			   
			{ 																									   
			  case 0: 			// [012]
					signs =  2.0; break;
				case 1: 			// [210]
					//signs =  2.0; break;
					signs = -2.0; break;
				case 2: 			// [120]
					signs = -1.0; break;
				case 3: 			// [201]
					signs = -1.0; break;
				case 4: 			// [021]
					//signs = -1.0; break;
					signs =  1.0; break;
				case 5: 			// [102]
					//signs = -1.0; break;
					signs =  1.0; break;
			}

			for(int qqq=0; qqq < params.NQQQs; ++qqq)
			{
				for(int iL=0; iL < params.NH[ordering][L]; ++iL)
				for(int iM=0; iM < params.NH[ordering][M]; ++iM)
				for(int iR=0; iR < params.NH[ordering][R]; ++iR)
				{
					if(   (quarks[qL].dilutions[iL].source_header.t_source
					    == quarks[qM].dilutions[iM].source_header.t_source)
					    &&(quarks[qM].dilutions[iM].source_header.t_source
						  == quarks[qR].dilutions[iR].source_header.t_source) ) 
					{
						int i0 = DilSwapInv( ordering, iL, iM, iR, 0 );
						int i1 = DilSwapInv( ordering, iL, iM, iR, 1 );
						int i2 = DilSwapInv( ordering, iL, iM, iR, 2 );
											
						result = CQQQ[ qqq ]( 
									                quarks[ qL ].dilutions[ iL ].source,
					  			                quarks[ qM ].dilutions[ iM ].source,
		    	  			                quarks[ qR ].dilutions[ iR ].source,
		    	  			                MINUS 
		    	  			              );
						
						for(int b=0; b < CQQQ[ qqq ].NBaryonOps; ++b)
						{
							elem = phases.sft( result[ 0 ] );
			    		for(int p=0; p < params.Nmomenta; ++p)
							{
			    			for(int t=0; t < params.nrow[ 3 ]; ++t)
								{
									CQQQ[ qqq ].baryon[ b ]->baryonoperator.orderings[ 0 ].op( i0, i1, i2 ).ind[ 0 ].elem( p, t ) 
									+= ( CQQQ[ qqq ].coef[ b ] * signs * elem( p, t ) );
								} // t 
							} // p
						} // Bop_index
			    } // if t_sources are all the same
				} // iL,iM,iR
			} // qqq
			swatch.stop();
			QDPIO::cout << "SOURCE operator " << ordering << " done: time= " << swatch.getTimeInSeconds() << " secs" << endl;
		} // ordering
	} // try
	catch ( const std::string& e )
	{
	  QDPIO::cerr << ": Caught Exception creating source baryon operator: " << e << endl;
	  QDP_abort( 1 );
	}
	QDPIO::cout << "All SOURCE operators done" << endl;

	#ifdef PRINTDEBUG
	for(int b=0; b < params.Noperators; ++b)
	{
	  for(int t=0; t < params.nrow[ 3 ]; ++t)
		{
			QDPIO::cout<<"op("<<b<<") t:"<<t<<" "<<COp[ b ].baryonoperator.orderings[ 0 ].op(0,0,0).ind[ 0 ].elem(0,t)<<endl;
		}
	}
	#endif

#endif // end of creation operators

	QDPIO::cout<<"end of group baryon operator calculation"<<endl;

	// Now writing the operators out ... inline_stoch_baryon_w.cc

  // Save the operators
  // ONLY SciDAC output format is supported!
  swatch.start();
	for(int b=0; b < params.Noperators; ++b)
  {
    XMLBufferWriter file_xml;
    push( file_xml, params.Names[ b ] );
    file_xml << params.param.baryon_operator;
    write( file_xml, "Config_info", gauge_xml );
    pop( file_xml );
    QDPFileWriter to( file_xml, params.Names[ b ],
                      QDPIO_SINGLEFILE, QDPIO_SERIAL, QDPIO_OPEN );
    // Write the scalar data
    {
      XMLBufferWriter record_xml;
      write( record_xml, "SourceBaryonOperator", COp[ b ].baryonoperator );
      write( to, record_xml, COp[ b ].baryonoperator.serialize() );
    }
    // Write the scalar data
    {
      XMLBufferWriter record_xml;
      write( record_xml, "SinkBaryonOperator", AOp[ b ].baryonoperator );
      write( to, record_xml, AOp[ b ].baryonoperator.serialize() );
    }
    close( to );
  }
  swatch.stop();
  QDPIO::cout << "Operators written: time= " << swatch.getTimeInSeconds() << " secs" << endl;

  // Close the namelist output file XMLDAT
  pop( xml_out );   // GroupBaryonAll2All
  END_CODE();
  // Time to bolt
  Chroma::finalize();
  exit( 0 );
}
