#
#  $Header: /home/bjoo/fromJLAB/cvsroot/chroma_base/scripts/run_xmldiff.pl,v 1.2 2004-12-05 18:28:09 mcneile Exp $
#
#  This is wrapper script to run the xmldiff application from
#  a makefile
#
#  Homepage for xmldiff application
#  https://forge.nesc.ac.uk/projects/xmldiff/
#
#
# More work:
#   At the moment this script assumes that the 
#   xmldiff application is in the users path.
#   Perhaps, this should be incorporated in the autoconf
#   tool chain.

# location of xmldiff
$xmldiff = "xmldiff" ;


# check whether xmldiff is available

$xml_check = `xmldiff` ; 
chop ($xml_check) ; 

if( $xml_check =~ /Usage/ )
{
#    print "xmldiff found\n" ; 
}
else
{
    print "Error:".$0." needs the xmldiff utility in your path\n" ; 
    print "Download it from http://forge.nesc.ac.uk/projects/xmldiff/ \n" ; 
    exit (0) ;
}



# at some stage this should be factored into another perl script
 %HoH = (
	  t_propagator_fuzz_s => {
                       output      => "t_propagator_fuzz_s.xml",
                       metric       => "../../tests/t_asqtad_prop/t_propagator_fuzz_s_METRIC.xml" ,
                       controlfile  => "../../tests/t_asqtad_prop/t_propagator_fuzz_s.xml" ,
		   },

	  t_propagator_s => {
                       output      => "t_propagator_s.xml",
                       metric       => "../../tests/t_asqtad_prop/t_propagator_s_METRIC.xml" ,
                       controlfile  => "../../tests/t_asqtad_prop/t_propagator_s.xml" ,
		   },



#	  t_mesons_w    => {
#                       output       => "./t_mesons_w.xml",
#                       metric       => "control/metric_t_mesons_w.xml" ,
#                       controlfile  => "control/t_mesons_w.xml" ,
#		   },
#	 t_io    => {
#                       output       =>  "./t_io.xml",
#                       metric       =>  "control/metric_t_io.xml" ,
#                       controlfile  =>  "control/t_io.xml" ,
#		   },
	 ) ; 


$role = "output" ; 



print "Test   conclusion\n" ; 
# run the tests
foreach $execute ( keys %HoH ) 
{
    print "$execute \t\t";
    $candidate =  $HoH{$execute}{"output"} ; 
    $metric    =  $HoH{$execute}{"metric"} ; 
    $control   =  $HoH{$execute}{"controlfile"} ; 

    if( ! -f  $execute )
    {
	$log_make  = "log_make_".$execute  ; 
	system("make $execute >& $log_make ") ; 
    }


    if(  -f  $execute )
    {
	$log = "log_".$execute  ; 
	$status_tmp = system("$execute  >& $log") / 256 ; 
	if( $status_tmp != 0  ) 
        {
		print "   RUN_FAIL\n"  ; 
	}
	else
	{
	    $log_xml = "log_xml_".$execute  ; 

#    $status_tmp = system("$xmldiff $control $candidate $metric $log_xml -v") ; 
	    $status_tmp = system("$xmldiff $control $candidate $metric $log_xml ") ; 

	    $status = $status_tmp / 256  ;   ## some perl feature

	    if( $status == 0 ) 
	    {
		print "   PASS\n"  ; 
	    }
	    else
	    {
		print "   FAIL\n"  ; 
	    }
	}
    }
    else
    {
	printf("   FAIL (compile)\n"); 

    }



}
