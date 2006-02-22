#
#  $Id: regres.pl,v 1.3 2006-02-22 03:24:34 edwards Exp $
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
	 input       => "$test_dir/chroma/hadron/make_source/make_source-v6.ini.xml" , 
	 output      => "make_source-v6.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/make_source/make_source-v6.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/make_source/make_source-v6.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/make_source/dilute.ini.xml" , 
	 output      => "dilute.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/make_source/dilute.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/make_source/dilute.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/make_source/source-displace.ini.xml" , 
	 output      => "source-displace.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/make_source/source-displace.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/make_source/source-displace.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/make_source/source-deriv.ini.xml" , 
	 output      => "source-deriv.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/make_source/source-deriv.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/make_source/source-deriv.out.xml" ,
     }
     );
