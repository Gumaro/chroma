#
#  $Id: regres.pl,v 3.5 2007-05-01 14:54:42 bjoo Exp $
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
	 input       => "$test_dir/chroma/hadron/propagator/prec_wilson-smearing.ini.xml" , 
	 output      => "prec_wilson-smearing.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/prec_wilson-smearing.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/prec_wilson-smearing.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/prec_wilson.ferm.ini.xml" , 
	 output      => "prec_wilson.ferm.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/prec_wilson.ferm.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/prec_wilson.ferm.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/prec_parwilson.ini.xml" , 
	 output      => "prec_parwilson.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/prec_parwilson.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/prec_parwilson.out.xml" ,
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
	 input       => "$test_dir/chroma/hadron/propagator/prec_clover-stout3d.ini.xml" , 
	 output      => "prec_clover-stout3d.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/prec_clover-stout3d.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/prec_clover-stout3d.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/prec_clover-stout3d.bicgstab.ini.xml" , 
	 output      => "prec_clover-stout3d.bicgstab.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/prec_clover-stout3d.bicgstab.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/prec_clover-stout3d.bicgstab.out.xml" ,
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
	 input       => "$test_dir/chroma/hadron/propagator/prec_ht_contfrac5d.ini.xml" , 
	 output      => "prec_ht_contfrac5d.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/prec_ht_contfrac5d.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/prec_ht_contfrac5d.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/prec_ovlap_contfrac5d.ini.xml" , 
	 output      => "prec_ovlap_contfrac5d.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/prec_ovlap_contfrac5d.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/prec_ovlap_contfrac5d.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/unprec_ovlap_contfrac5d.ini.xml" , 
	 output      => "unprec_ovlap_contfrac5d.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/unprec_ovlap_contfrac5d.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/unprec_ovlap_contfrac5d.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/prec_zolo_nef.ini.xml" , 
	 output      => "prec_zolo_nef.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/prec_zolo_nef.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/prec_zolo_nef.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/unprec_zolo_nef.ini.xml" , 
	 output      => "unprec_zolo_nef.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/unprec_zolo_nef.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/unprec_zolo_nef.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/prec_ovext.ini.xml" , 
	 output      => "prec_ovext.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/prec_ovext.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/prec_ovext.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/unprec_ovext.ini.xml" , 
	 output      => "unprec_ovext.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/unprec_ovext.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/unprec_ovext.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/unprec_dwftransf.ini.xml" , 
	 output      => "unprec_dwftransf.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/unprec_dwftransf.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/unprec_dwftransf.out.xml" ,
     },
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/hadron/propagator/unprec_wilson.ini.xml" , 
	 output      => "unprec_wilson.candidate.xml",
	 metric      => "$test_dir/chroma/hadron/propagator/unprec_wilson.metric.xml" ,
	 controlfile => "$test_dir/chroma/hadron/propagator/unprec_wilson.out.xml" ,
     }
# Commented these out so they don't break nightlies. They work but need
# time to be local...  Will fix later by mucking with the run script
#    ,
#    {
#	 exec_path   => "$top_builddir/mainprogs/main" , 
#	 execute     => "chroma" , 
#	 input       => "$test_dir/chroma/hadron/propagator/unprec_s_cprec_t_wilson.ini.xml" , 
#	 output      => "unprec_s_cprec_t_wilson.candidate.xml",
#	 metric      => "$test_dir/chroma/hadron/propagator/unprec_s_cprec_t_wilson.metric.xml" ,
#	 controlfile => "$test_dir/chroma/hadron/propagator/unprec_s_cprec_t_wilson.out.xml" ,
#     },
#     {
#        exec_path   => "$top_builddir/mainprogs/main" ,
#        execute     => "chroma" ,
#        input       => "$test_dir/chroma/hadron/propagator/iluprec_s_cprec_t_wilson.ini.xml" ,
#        output      => "iluprec_s_cprec_t_wilson.candidate.xml",
#        metric      => "$test_dir/chroma/hadron/propagator/iluprec_s_cprec_t_wilson.metric.xml" ,
#        controlfile => "$test_dir/chroma/hadron/propagator/iluprec_s_cprec_t_wilson.out.xml" ,
#     }
#

     );
