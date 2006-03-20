#
#  $Id: regres.pl,v 1.6 2006-03-20 04:23:23 edwards Exp $
#
#  This is the portion of a script this is included recursively
#

#
# Each test has a name, input file name, output file name,
# and the good output that is tested against.
#
@regres_list = 
    (
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/unprec-zolo-ev-multi.ini.xml" , 
	 output      => "unprec-zolo-ev-multi.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/unprec-zolo-ev-multi.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/unprec-zolo-ev-multi.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/prec_wilson.ini.xml" , 
	 output      => "prec_wilson.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/prec_wilson.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/prec_wilson.out.xml" ,
     },

     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/prec_clover.ini.xml" , 
	 output      => "prec_clover.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/prec_clover.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/prec_clover.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/unprec_clover.ini.xml" , 
	 output      => "unprec_clover.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/unprec_clover.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/unprec_clover.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/prec_wilson-twisted.ini.xml" , 
	 output      => "prec_wilson-twisted.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/prec_wilson-twisted.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/prec_wilson-twisted.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/unprec-ovlap-zolo4d.ini.xml" , 
	 output      => "unprec-ovlap-zolo4d.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/unprec-ovlap-zolo4d.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/unprec-ovlap-zolo4d.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/unprec_hamberwu.ini.xml" , 
	 output      => "unprec_hamberwu.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/unprec_hamberwu.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/unprec_hamberwu.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/prec_dwf.ini.xml" , 
	 output      => "prec_dwf.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/prec_dwf.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/prec_dwf.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/prec_parwilson.ini.xml" , 
	 output      => "prec_parwilson.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/prec_parwilson.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/prec_parwilson.out.xml" ,
     }
     );
